#include "../include/bpn.h"

BPN::BPN(const Matrix input, const Matrix output, size_t hidden) {
    this->size_input = input.cols;
    this->size_output = output.cols;
    this->size_hidden = hidden;

    this->expected_input = input;
    this->expected_output = output;

    Matrix value_m(size_input, size_hidden);
	value_m.randomize(0.1, 0.9);

	Matrix weight_m(size_hidden, size_output);
	weight_m.randomize(0.1, 0.9);

	Matrix delta_value_m(size_input, size_hidden);
	Matrix delta_weight_m(size_hidden, size_output);

    matrices.push_back(value_m);
    matrices.push_back(weight_m);
    matrices.push_back(delta_value_m);
    matrices.push_back(delta_weight_m);
}

// output of hidden layer
Matrix BPN::calculate_output_hidden_layer(Matrix &current_training_input, Matrix &value_m) {
    Matrix out_of_in(current_training_input);
    // input_of_hidden = transpose(value_m) * out_to_in
    Matrix input_of_hidden = value_m.transpose() * out_of_in;
    // calculate output of hidden layer.
    Matrix output_of_hidden(size_hidden, 1);
    for (size_t row = 0; row < size_hidden; row++) {
        double val = input_of_hidden[row][0] * -1;
        double end_val = (double)(1 / (1 + exp(val)));
        output_of_hidden.mat[row][0] = end_val;
    }
    return output_of_hidden;
}

//output of output layer
Matrix BPN::calculate_output_output_layer(Matrix &output_of_hidden_layer, Matrix &weight_m) {
    // calculate input of the outputlayer, transpose(weight_m) * output_of_hidden_layer
    Matrix in_of_out = weight_m.transpose() * output_of_hidden_layer;

    Matrix output_of_output_layer(size_output, 1);
    for (size_t row = 0; row < size_output; row++) {
        double val = in_of_out[row][0] * -1;
        double end_val = (double)(1 / (1 + exp(val)));
        output_of_output_layer.mat[row][0] = end_val;
    }
    return output_of_output_layer;
}

void BPN::adapt_values_and_weights(Matrix &current_training_output, Matrix &current_training_input, Matrix &output_of_output_layer, Matrix &output_of_hidden_layer, Matrix &value_m, Matrix &weight_m) {

    // calculate the error between output of training and current_training_output of output_of_output_layer
    Matrix output_layer_error = current_training_output - output_of_output_layer;

    Matrix deltas(size_output, 1);

    for (size_t row = 0; row < size_output; row++) {
        double delta_i;
        double output_layer_val = output_of_output_layer[row][0];
        delta_i = (current_training_output[row][0] - output_layer_val) * output_layer_val * (1 - output_layer_val);
        deltas[row][0] = delta_i;
    }


    Matrix why = output_of_hidden_layer * deltas.transpose();
    // TODO: use ETHA and ALPHA
    double ALPHA = 1;
    double ETHA = 0.6;
    Matrix delta_weights = matrices.at(3) * ALPHA + why * ETHA;

    Matrix hidden_layer_error = matrices.at(1) * deltas;

    Matrix delta_star(size_hidden, 1);
    for (size_t row = 0; row < size_hidden; row++) {
        double hidden_layer_val = output_of_hidden_layer[row][0];
        double delta = hidden_layer_error[row][0] * hidden_layer_val * (1 - hidden_layer_val);
        delta_star[row][0] = delta;
    }

    Matrix ex = current_training_input * delta_star.transpose();

    Matrix delta_values = matrices.at(2) * ALPHA + ex * ETHA;
    value_m = matrices.at(0) + delta_values;
    weight_m = matrices.at(1) + delta_weights;

}

double BPN::calculate_output_error(Matrix &current_training_output, Matrix &output_of_output_layer) {
    double err = 0;
    double diff = 0;
    for (size_t row = 0; row < size_output; row++) {
        diff = current_training_output[row][0] - output_of_output_layer[row][0];
        err += (diff*diff);
    }

    return 0.5*err;
}

bool BPN::train(unsigned int MAX_RUNS) {
    unsigned int runs = 0;
    // error_output_first is for value_m and weight_m
    // error_output_second is for the new value_m and weight_m
    cout << "Epsilon is " << EPSILON << endl;
    double error_output_first, error_output_second, error_diff = EPSILON + 1;
    cout << "Error diff is " << error_diff << endl;

    Matrix new_value_m;
    Matrix new_weight_m;

    while((error_diff > EPSILON) && (runs < MAX_RUNS)) {
        error_diff = 0.0;

        for (size_t row_nr = 0; row_nr < expected_input.rows; row_nr++) {
            Matrix current_training_input(expected_input.mat.at(row_nr), true); // also transposes
            Matrix current_training_output(expected_output.mat.at(row_nr), true); // also transposes
            // cout << "Training input" << endl << current_training_input;
            // cout << "Training output" << endl << current_training_output << endl;
            Matrix output_of_hidden_layer = calculate_output_hidden_layer(current_training_input, matrices.at(0));
            // cout << "Output of hidden layer " << endl << output_of_hidden_layer << endl;

            Matrix output_of_output_layer = calculate_output_output_layer(output_of_hidden_layer, matrices.at(1));
            // cout << "Output of output layer " << endl << output_of_output_layer << endl;

            // cout << "Output of outputlayer" << endl;
            // cout << output_of_output_layer << endl;

            // cout << "Pre" << endl;
			// cout << new_value_m << endl << new_weight_m << endl;
            adapt_values_and_weights(current_training_output, current_training_input, output_of_output_layer, output_of_hidden_layer, new_value_m, new_weight_m);

            // cout << "Post" << endl;
			// cout << new_value_m << endl << new_weight_m << endl;

            // cout << "New values and weights" << endl;
            // cout << new_value_m << endl << new_weight_m << endl;
            error_output_first = calculate_output_error(current_training_output, output_of_output_layer);

            // func
            // II is IT, V1 W1
            Matrix output_of_hidden_layer_second = calculate_output_hidden_layer(current_training_input, new_value_m);
            Matrix output_of_output_layer_second = calculate_output_output_layer(output_of_hidden_layer, new_weight_m);
            // use second output_of_output_layer with calculate_output_error

            error_output_second = calculate_output_error(current_training_output, output_of_output_layer_second);

            error_diff += (error_output_second - error_output_first) * (error_output_second - error_output_first);
            matrices.at(0) = new_value_m;
            matrices.at(1) = new_weight_m;
            // cout << "--------" << endl;
        }
        cout << "Run: " << runs << " error is " << error_diff << endl;
        runs++;
    }

    return runs < MAX_RUNS;
}

double BPN::guess(Matrix &input_row) {
    Matrix output_of_hidden_layer = calculate_output_hidden_layer(input_row, matrices.at(0));
    Matrix output_of_output_layer = calculate_output_output_layer(output_of_hidden_layer, matrices.at(1));
    cout << "My guess" << endl;
    for (size_t i = 0; i < output_of_output_layer.rows; i++) {
        for (size_t j = 0; j < output_of_output_layer.cols; j++) {
            double val = output_of_output_layer[i][j];
            cout << val;
        }
        cout << endl;
    }

    return 0.0f;
}

ostream& operator<< (ostream & out, const BPN &data) {
    out << "--------------- BPN ---------------" << endl;
    out << "Size input " << data.size_input << endl;
    out << "Size hidden " << data.size_hidden << endl;
    out << "Size output " << data.size_output << endl;
    out << "------------------------------" << endl;
    out << "Value matrix" << endl << data.matrices.at(0) << endl;
    out << "Weight matrix" << endl << data.matrices.at(1) << endl;
    out << "Delta value matrix" << endl << data.matrices.at(2) << endl;
    out << "Delta weight matrix" << endl << data.matrices.at(3) << endl;
    out << "--------------- END BPN ---------------" << endl;
    return out;
}
