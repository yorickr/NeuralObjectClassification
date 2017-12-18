#pragma once

#include <iostream>
#include <vector>

using namespace std;

class Matrix {
public:
    vector<vector<double>> mat;
    size_t cols;
    size_t rows;

    Matrix();
    Matrix(size_t cols, size_t rows);
    Matrix(const Matrix &m);
    Matrix(vector<double> v, bool transpose = false);

    void randomize(double start, double end);
    Matrix transpose();

    vector<double>& operator[](int i);
    Matrix operator*(const Matrix &o);
    Matrix operator*(double d);
    Matrix operator-(const Matrix &o);
    Matrix operator+(const Matrix &o);

    friend ostream& operator<< (ostream & out, const Matrix &data);

};
