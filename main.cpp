#include <iostream>

#include "./include/bpn.h"
#include "./include/matrix.h"
#include "./include/trainingset.h"

#include "./include/includes.h"

#include "./include/trainingset.h"

using namespace std;

int main(int argc, char** argv) {
	srand(time(NULL));
	cout << boolalpha;
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

	// bpn.train(10000);
	// cout << bpn << endl;

	for (size_t row = 0; row < input.rows; row++) {
		Matrix r(input[row], true); // transpose
		// bpn.guess(r);
	}

	TrainingSet s("./images/training_plaatjes/");
	// Mat img = imread("./images/training_plaatjes/dobbelsteen/dobbelsteen (6).jpg");
	// if (!img.data)
	// {
	// 	cout << "Could not open or find the image" << endl;
	// 	return -1;
	// }
	Mat img = get<1>(s.image_groups.at(0));
	imshow("Image", img);
	Mat gray_image;
	cvtColor(img, gray_image, CV_BGR2GRAY);
	imshow("Gray", gray_image);
	waitKey(1);
	Mat bin;
	Mat bin_inv;
	int thresh = 60;
	while (true) {
		cin >> thresh;
		s.calculate_surface_area(gray_image, thresh);
		waitKey(1);
	}

	// s.calculate_surface_area(img);

	return 0;
}
