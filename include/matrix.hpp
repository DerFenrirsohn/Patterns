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
    std::pair<int,int> dimensions_;
    std::vector<std::vector<double>> components_;
    static int spaceDimension;

    Matrix(){};
    Matrix(std::pair<int,int> dim,int seed=time(0),bool nullMatrix=false);

    Matrix operator*=(Matrix rhs);
    Matrix operator+=(Matrix rhs);
    Matrix operator-=(Matrix rhs);
    Matrix hadamard(Matrix rhs);
    Matrix transpose();


};
Matrix operator*(Matrix lhs,Matrix rhs);
Matrix operator*(double lhs,Matrix rhs);
Matrix operator+(Matrix lhs,Matrix rhs);
Matrix operator-(Matrix lhs,Matrix rhs);
Matrix hadamard(Matrix lhs,Matrix rhs);
std::vector<Matrix> split(Matrix data);
Matrix concatenate(std::vector<Matrix> data);
double distance(Matrix m1, Matrix m2);
std::ostream& operator<<(std::ostream& os, const Matrix& m);

class Vec : public Matrix
{
    public:
    Vec():Vec(spaceDimension,time(0),false){};
    Vec(int dim,int seed=time(0),bool nullVec=false):Matrix(std::make_pair(dim,1),seed,nullVec){};
    Vec(std::vector<double> v);
};

#endif