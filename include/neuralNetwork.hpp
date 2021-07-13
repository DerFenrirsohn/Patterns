#ifndef NN_HPP
#define NN_HPP
#include <matrix.hpp>
#include <math.h>
double activationFunction(double input);
double activationFunctionPrime(double input);
Matrix activationFunction(Matrix input);
Matrix activationFunctionPrime(Matrix input);

class Layer
{
    public:
    Matrix weights_;
    Matrix biases_;

    Layer(int input,int output,int seed=time(0));
    Matrix propagate(Matrix input);
};

class Network
{
    public:
    std::vector<Layer> layers_;
    std::vector<int> dimensions_;

    Network(){};
    Network(std::vector<int> dimensions);
    Matrix backPropagate(Matrix x, Matrix y,double eta);
    Matrix propagate(Matrix input);

};

#endif