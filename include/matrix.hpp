#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <list>
#include <stdexcept>
#include <iostream>
#include <random>

class Matrix
{
    public:
    Matrix(std::pair<int,int> dim,int seed=time(0),bool nullMatrix=false);

    Matrix operator*=(Matrix rhs);
    Matrix operator+=(Matrix rhs);
    Matrix operator-=(Matrix rhs);
    Matrix hadamard(Matrix rhs);
    Matrix transpose();

    std::pair<int,int> dimensions_;
    std::vector<std::vector<double>> components_;
};
Matrix operator*(Matrix lhs,Matrix rhs);
Matrix operator*(double lhs,Matrix rhs);
Matrix operator+(Matrix lhs,Matrix rhs);
Matrix operator-(Matrix lhs,Matrix rhs);
Matrix hadamard(Matrix lhs,Matrix rhs);
std::ostream& operator<<(std::ostream& os, const Matrix& m);

class Vec : public Matrix
{
    public:
    Vec(int dim,int seed=time(0),bool nullVec=false):Matrix(std::make_pair(dim,1),seed,nullVec){};
    Vec(std::vector<double> v);
};

#endif