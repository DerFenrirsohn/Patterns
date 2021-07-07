#include "matrix.hpp"
  

std::ostream& operator<<(std::ostream& os, const Matrix& m)
{
    for (int i=0;i!=m.dimensions_.first;i++)
    {
        
        for(int j=0;j!=m.dimensions_.second;j++)
        {
            os<<m.components_[i][j]<<" ";
        }
        os<<std::endl;
    }
    return os;
};

Matrix::Matrix(std::pair<int,int> dim,int seed,bool nullMatrix)
{
    dimensions_=dim;
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(0.0,1.0);
    for (int i=0;i!=dim.first;i++)
    {
        std::vector<double> vec;
        for(int j=0;j!=dim.second;j++)
        {
            double x=nullMatrix?0:distribution(generator);
            vec.push_back(x);
            
        }
        components_.push_back(vec);
    }
};

Matrix Matrix::operator*=( Matrix rhs)
{
    if ( dimensions_.second != rhs.dimensions_.first ) 
    {
        throw std::invalid_argument( "mismatching dimensions" );
    }
    else
    {       
        Matrix m(std::make_pair(dimensions_.first,rhs.dimensions_.second));
        for (int i=0;i!=m.dimensions_.first;i++)
        {
            for(int j=0;j!=m.dimensions_.second;j++)
            {
                double x=0;
                for(int k=0;k!=dimensions_.second;k++)
                {
                    x+=components_[i][k]*rhs.components_[k][j];
                }
                m.components_[i][j]=x;
            }
        }
        return m;
    }
    
};
Matrix Matrix::operator+=( Matrix rhs)
{
    if ( dimensions_.first != rhs.dimensions_.first || dimensions_.second != rhs.dimensions_.second) 
    {
        throw std::invalid_argument( "mismatching dimensions" );
    }
    else
    {       
        Matrix m(std::make_pair(dimensions_.first,dimensions_.second));
        for (int i=0;i!=m.dimensions_.first;i++)
        {
            for(int j=0;j!=m.dimensions_.second;j++)
            {
                m.components_[i][j]=components_[i][j]+rhs.components_[i][j];   
            }
        }
        return m;
    }
};
Matrix Matrix::operator-=( Matrix rhs)
{
    if ( dimensions_.first != rhs.dimensions_.first || dimensions_.second != rhs.dimensions_.second) 
    {
        throw std::invalid_argument( "mismatching dimensions" );
    }
    else
    {       
        Matrix m(std::make_pair(dimensions_.first,dimensions_.second));
        for (int i=0;i!=m.dimensions_.first;i++)
        {
            for(int j=0;j!=m.dimensions_.second;j++)
            {
                m.components_[i][j]=components_[i][j]-rhs.components_[i][j];   
            }
        }
        return m;
    }
};
Matrix Matrix::hadamard( Matrix rhs)
{
    if ( dimensions_.first != rhs.dimensions_.first || dimensions_.second != rhs.dimensions_.second) 
    {
        throw std::invalid_argument( "mismatching dimensions" );
    }
    else
    {       
        Matrix m(std::make_pair(dimensions_.first,dimensions_.second));
        for (int i=0;i!=m.dimensions_.first;i++)
        {
            for(int j=0;j!=m.dimensions_.second;j++)
            {
                m.components_[i][j]=components_[i][j]*rhs.components_[i][j];   
            }
        }
        return m;
    }
};
Matrix Matrix::transpose()
{
    Matrix m(std::make_pair(dimensions_.second,dimensions_.first));
    for (int i=0;i!=m.dimensions_.first;i++)
    {
        for(int j=0;j!=m.dimensions_.second;j++)
        {  
            m.components_[i][j]=components_[j][i]; 
        }
    }
    return m;
};
Matrix operator*(Matrix lhs,Matrix rhs)
{

    return lhs*=rhs;
};
Matrix operator*(double lhs,Matrix rhs)
{
    Matrix m(std::make_pair(rhs.dimensions_.first,rhs.dimensions_.second));
    for (int i=0;i!=m.dimensions_.first;i++)
    {
        for(int j=0;j!=m.dimensions_.second;j++)
        {
            m.components_[i][j]=lhs*rhs.components_[i][j];   
        }
    }
    return m;
};
Matrix operator+(Matrix lhs,Matrix rhs)
{
    return lhs+=rhs;
};
Matrix operator-(Matrix lhs,Matrix rhs)
{
    return lhs-=rhs;
};
Matrix hadamard(Matrix lhs,Matrix rhs)
{
    return lhs.hadamard(rhs);
};
    
Vec::Vec(std::vector<double> v):Matrix(std::make_pair(v.size(),1),0,true)
{
    for (int i=0;i!=v.size();i++)
    {
        components_[i][0]=v[i];
    }
};