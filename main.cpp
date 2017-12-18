#include <iostream>

#include "./include/bpn.h"
#include "./include/matrix.h"

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

	int steps = 5;

	Matrix n(1, 4);
	n[0][0] = 100;
	n[0][1] = 100;
	n[0][2] = 101;
	n[0][3] = 101;

	Matrix m(1, steps*2);
	for (size_t i = 0; i < m.rows; i++) {
		for (size_t j = 0; j < steps; j++) {
			double val = (j + 1) * (1.0 / steps);
			for (size_t x = 0; x < 2; x++) {
				m[i][2*j+x] = val;
			}
		}
	}
	cout << n << endl;
	cout << m << endl;

	BPN bpn(n, m, size_hidden);
	cout << bpn << endl;

	bpn.train(10000);
	cout << bpn << endl;

	cout << input << endl;

	for (size_t row = 0; row < n.rows; row++) {
		Matrix r(n[row], true);
		bpn.guess(r);
	}

	return 0;
}
