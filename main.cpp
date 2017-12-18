#include <iostream>

#include "./include/bpn.h"
#include "./include/matrix.h"

#include "./include/includes.h"

using namespace std;

int main(int argc, char** argv) {
	srand(time(NULL));
	cout << "Running with the following args: ";
	for (int i = 1; i < argc; i++) {
		cout << argv[i] << " ";
	}
	cout << endl;
	size_t size_input = 2;
	size_t size_hidden = 5;
	size_t size_output = 1;

	// 0 0 		0
	// 0 1 		0
	// 1 0 		0
	// 1 1 		1
	Matrix input(4, size_input);
	input.mat[1][1] = 1;
	input.mat[2][0] = 1;
	input.mat[3][0] = 1;
	input.mat[3][1] = 1;
	cout << endl << endl;
	cout << "Training input" << endl << input << endl;

	Matrix output(4, size_output);
	output.mat[3][0] = 1;
	cout << "Training expected output" << endl << output << endl;

	BPN bpn(input, output, size_hidden);
	// cout << bpn << endl;

	bpn.train(10000);
	// cout << bpn << endl;

	for (size_t row = 0; row < input.rows; row++) {
		Matrix r(input[row], true); // transpose
		bpn.guess(r);
	}

	return 0;
}
