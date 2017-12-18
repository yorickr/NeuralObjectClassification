#include "../include/matrix.h"

Matrix::Matrix() {
    this->cols = 0;
    this->rows = 0;
}

Matrix::Matrix(size_t rows, size_t cols) {
    this->cols = cols;
    this->rows = rows;
    for (size_t i = 0; i < rows; i++) {
        vector<double> row;
        for (size_t j = 0; j < cols; j++) {
            row.push_back(0.0);
        }
        mat.push_back(row);
    }
}

Matrix::Matrix(vector<double> v, bool transpose) {
    if (transpose) {
        for (size_t i = 0; i < v.size(); i++) {
            vector<double> row;
            row.push_back(v[i]);
            mat.push_back(row);
        }
        this->rows = v.size();
        this->cols = 1;
    } else {
        this->cols = v.size();
        mat.push_back(v);
        this->rows = 1;
    }
}

Matrix::Matrix(const Matrix &m) {
    this->cols = m.cols;
    this->rows = m.rows;
    for (size_t i = 0; i < rows; i++) {
        vector<double> row;
        for (size_t j = 0; j < cols; j++) {
            row.push_back(m.mat[i][j]);
        }
        mat.push_back(row);
    }
}


void Matrix::randomize(double start, double end) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            mat[i][j] = start + (rand() / (RAND_MAX / (end - start)));
        }
    }
}

Matrix Matrix::transpose() {
    Matrix ret_val(this->cols, this->rows);
    for (size_t i = 0; i < this->rows; i++) {
        for (size_t j = 0; j < this->cols; j++) {
            ret_val.mat[j][i] = this->mat[i][j];
        }
    }
    return ret_val;
}

// Note to self.
// This will happily return a copy of your vector if you don't use a ref.
// Please never do this again.
vector<double>& Matrix::operator[](int i) {
    return mat[i];
}

Matrix Matrix::operator*(const Matrix &o) {
    // use rows of s and cols of o.
    Matrix ret_val(this->rows, o.cols);
    for (size_t i = 0; i < this->rows; i++) {
        for (size_t j = 0; j < o.cols; j++) {
            for (size_t k = 0; k < this->cols; k++) {
                ret_val.mat[i][j] += this->mat[i][k] * o.mat[k][j];
            }
        }
    }

    return ret_val;
}

Matrix Matrix::operator*(double d) {
    // use rows of s and cols of o.
    Matrix ret_val(this->rows, this->cols);
    for (size_t i = 0; i < this->rows; i++) {
        for (size_t j = 0; j < this->cols; j++) {
            ret_val[i][j] = this->mat[i][j] * d;
        }
    }
    return ret_val;
}

Matrix Matrix::operator-(const Matrix &o) {
    if ((this->rows != o.rows) && (this->cols != o.cols)) {
        cerr << "BOI WE AINT THE SAME SIZE IN OPERATOR- OF MATRIX" << endl;
    }
    Matrix ret_val(this->rows, this->cols);

    for (size_t i = 0; i < this->rows; i++) {
        for (size_t j = 0; j < this->cols; j++){
            ret_val[i][j] = mat[i][j] - o.mat[i][j];
        }
    }
    return ret_val;
}

Matrix Matrix::operator+(const Matrix &o) {
    if ((this->rows != o.rows) && (this->cols != o.cols)) {
        cerr << "BOI WE AINT THE SAME SIZE IN OPERATOR- OF MATRIX" << endl;
    }
    Matrix ret_val(this->rows, this->cols);

    for (size_t i = 0; i < this->rows; i++) {
        for (size_t j = 0; j < this->cols; j++){
            ret_val[i][j] = mat[i][j] + o.mat[i][j];
        }
    }
    return ret_val;
}


ostream& operator<<(ostream & out, const Matrix &data) {
    for (size_t i = 0; i < data.rows; i++) {
        for (size_t j = 0; j < data.cols; j++) {
            out << data.mat[i][j] << "\t";
        }
        out << endl;
    }
    return out;
}
