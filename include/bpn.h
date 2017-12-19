#pragma once

#include <iostream>
#include <cmath>

#include "matrix.h"

using namespace std;

class BPN {
public:
    size_t size_input;
    size_t size_output;
    size_t size_hidden;

    // 0: value_m
    // 1: weight_m
    // 2: delta_value_m
    // 3: delta_weight_m
    vector<Matrix> matrices;

    Matrix expected_input;
    Matrix expected_output;

    double EPSILON = 1E-10;

    BPN(Matrix input, Matrix output, size_t hidden);

    bool train(unsigned int MAX_RUNS);
    double guess(Matrix &input_row);

    Matrix calculate_output_hidden_layer(Matrix &current_training_input, Matrix &value_m);
    Matrix calculate_output_output_layer(Matrix &output_of_hidden_layer, Matrix &weight_m);
    void adapt_values_and_weights(Matrix &current_training_output, Matrix &current_training_input, Matrix &output_of_output_layer, Matrix &output_of_hidden_layer, Matrix &delta_values, Matrix &delta_weights);
    double calculate_output_error(Matrix &current_training_output, Matrix &output_of_output_layer);

    friend ostream& operator<< (ostream & out, const BPN &data);
};
