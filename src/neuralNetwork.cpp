#include <neuralNetwork.hpp>

// double activationFunction(double input)
// {
//     return (1-exp(-input))/(1+exp(-input));
// };

// double activationFunctionPrime(double input)
// {
//     return (2*exp(input)/(1+exp(input)*(1+exp(input))));
// };
double activationFunction(double input)
{
    return (log(input*input+10)*tanh(10*input));
};

double activationFunctionPrime(double input)
{
    return (2*input*tanh(10*input)/(input*input+10)+10*log(input*input+10)/(cosh(input)*cosh(input)));
};
Matrix activationFunction(Matrix input)
{
    Matrix m(input.dimensions_);
    for (int i=0;i<input.dimensions_.first;i++)
    {
       for (int j=0;j<input.dimensions_.second;j++)
        {
            m.components_[i][j]=activationFunction(input.components_[i][j]);
        }
    }
    return m;
};
Matrix activationFunctionPrime(Matrix input)
{
    Matrix m(input.dimensions_);
    for (int i=0;i<input.dimensions_.first;i++)
    {
       for (int j=0;j<input.dimensions_.second;j++)
        {
            m.components_[i][j]=activationFunctionPrime(input.components_[i][j]);
        }
    }
    return m;
};

Layer::Layer(int input,int output,int seed):weights_(Matrix(std::make_pair(output,input),seed)),biases_(Vec(output,seed)){};
Matrix Layer::propagate(Matrix input){return activationFunction(weights_*input+biases_);};

Network::Network(std::vector<int> dimensions):dimensions_(dimensions)
{
    int i=time(0);
    for(std::vector<int>::iterator it=dimensions.begin();it!=dimensions.end()-1;it++,i++)
    {
        layers_.push_back(Layer(*it,*(it+1),i));
    }
}
Matrix Network::backPropagate(Matrix x,Matrix y,double eta )
{
    // std::cout<<"(";
    // for (int i=0;i!=dimensions_.size();i++)
    // {

    // std::cout<<dimensions_[i]<<" ";
    // }
    // std::cout<<")("<< x.dimensions_.first<<" "<<x.dimensions_.second<<")("<< y.dimensions_.first<<" "<<y.dimensions_.second<<")"<<std::endl;
    

    std::list<Matrix> nabla_w;
    std::list<Matrix> nabla_b;
    //feedforward
    std::vector<Matrix> activations;
    std::vector<Matrix> zs;
    Matrix activation=x;
    activations.push_back(activation);
    for(std::vector<Layer>::iterator it=layers_.begin();it!=layers_.end();it++)
    {
        Matrix z = it->weights_*activation+it->biases_;
        zs.push_back(z);
        activation=activationFunction(z);
        activations.push_back(activation);
    }
    
    //backward pass
    Matrix delta=hadamard(*(activations.end()-1)-y,activationFunctionPrime(*(zs.end()-1)));;
    nabla_b.push_front(delta);
    nabla_w.push_front(delta*((*(activations.end()-2)).transpose()));

    for (int i = 1;i<layers_.size();i++)
    {
        Matrix z = *(zs.end()-i-1);
        Matrix sp = activationFunctionPrime(z);
        delta = hadamard((layers_.end()-i)->weights_.transpose()*delta,sp);
        nabla_b.push_front(delta);
        nabla_w.push_front(delta*((*(activations.end()-2-i)).transpose()));
    }
    std::list<Matrix>::iterator itw=nabla_w.begin();
    std::list<Matrix>::iterator itb=nabla_b.begin();
    for (int i = 0;i<layers_.size();i++,itw++,itb++)
    {
        layers_[i].weights_-=eta*(*itw);
        layers_[i].biases_-=eta*(*itb);
    }
    return *(activations.end()-1);





}
Matrix Network::propagate(Matrix input)
{
    Matrix output=input;
    for (std::vector<Layer>::iterator it=layers_.begin();it!=layers_.end();it++)
    {
        output=it->propagate(output);
    }
    return output;
};