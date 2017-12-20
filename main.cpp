#include <iostream>

#include "./include/bpn.h"
#include "./include/matrix.h"
#include "./include/trainingset.h"

#include "./include/includes.h"
#include "include\calibratie.h"

using namespace std;

double mmPerPixel = -1;
string cameraCalibrationFilePath = "C:\\School\\ueyecallib.yml";

void showFeatures() {
	TrainingSet s("C:\\Users\\remco\\Documents\\School\\Minor robotics and vision\\Periode 2\\Vision c++ voor Gevorderden\\NeuralObjectClassification\\NeuralObjectClassification\\NeuralObjectClassification\\images\\training_plaatjes");

	for (int i = 0; i < s.image_groups.size(); i++) {
		cout << s.image_groups[i].id << " : " << s.image_groups[i].label << endl;
	}
	cout << "Choose a trainingset" << endl;

	int set, pictureId;
	cin >> set;

	while (true) {
		cout << "Choose a picture: 0 and " << s.image_groups[set].images.size() - 1 << endl;
		cin >> pictureId;

		while (pictureId < 0 || pictureId >= s.image_groups[set].images.size()) {
			cout << "Picture id is out of range, choose again between 0 and " << s.image_groups[set].images.size() - 1 << endl;
			cin >> pictureId;
		}

		Mat gray_image;
		cvtColor(s.image_groups[set].images[pictureId], gray_image, CV_BGR2GRAY);

		imshow("Original gray image", gray_image);

		cout << "Length: " << s.calculate_length(gray_image, 25) << endl;
		cout << "Width: " << s.calculate_width(gray_image, 25) << endl;
		cout << "Surface area: " << s.calculate_surface_area(gray_image, 25) << endl;
		cout << "Is circle: " << s.calculate_if_circle(gray_image, 25) << endl;
		cout << "Is square: " << s.calculate_if_square(gray_image, 25) << endl;
		cout << "Bending energy: " << s.calculate_bending_energy(gray_image, 25) << endl;
		cout << "Perimeter: " << s.calculate_perimeter(gray_image, 25) << endl;
		cout << endl;

		waitKey(0);
	}
}

int main(int argc, char** argv) {
	srand(time(NULL));
	//cout << boolalpha;
	//cout << "Running with the following args: ";
	//for (int i = 1; i < argc; i++) {
	//	cout << argv[i] << " ";
	//}

	//cout << endl;
	//size_t size_input = 2;
	//size_t size_hidden = 5;
	//size_t size_output = 1;

	//// 0 0 		0
	//// 0 1 		0
	//// 1 0 		0
	//// 1 1 		1
	//Matrix input(4, size_input);
	//input.mat[1][1] = 1;
	//input.mat[2][0] = 1;
	//input.mat[3][0] = 1;
	//input.mat[3][1] = 1;
	//cout << endl << endl;
	//cout << "Training input" << endl << input << endl;

	//Matrix output(4, size_output);
	//output.mat[3][0] = 1;
	//cout << "Training expected output" << endl << output << endl;

	//BPN bpn(input, output, size_hidden);
	//// cout << bpn << endl;

	//// bpn.train(10000);
	//// cout << bpn << endl;

	//for (size_t row = 0; row < input.rows; row++) {
	//	Matrix r(input[row], true); // transpose
	//	// bpn.guess(r);
	//}

	//TrainingSet s("./images/training_plaatjes/");
	//pair<Matrix,Matrix> out = s.compute();
	//cout << out.first << endl;
	//cout << out.second << endl;
	//BPN guesser(out.first, out.second, 3);
	//// cout << guesser << endl;
	//bpn.train(100000);
	//for (size_t row = 0; row < 2; row++) {
	//	Matrix r(out.first[row], true);
	//	cout << "Input is " << endl;
	//	cout << r.transpose() << endl;
	//	guesser.guess(r);
	//}
	//calibrateCamera(cameraCalibrationFilePath);

	/*
	int result;
	cout << "Do you want to calibrate the camera?" << endl;
	cin >> result;
	if (result == 1) { 
		destroyAllWindows();
	}

	cout << "Millimeters per pixel calibration" << endl;
	mmPerPixel = calibratePixelSize(cameraCalibrationFilePath);*/

	string key;
	while (true) {
		cout << "Enter a key: c for calibration, r for item recornition, n for making a trainingset or q to quit" << endl;
		cin >> key;

		if (key == "c") { 
			calibrate(cameraCalibrationFilePath); 
			cout << "Calibration done" << endl;
			destroyAllWindows();
			cin.ignore();
		}
		else if (key == "r") { cout << "live recornition" << endl; }
		else if (key == "n") { cout << "making a trainingset" << endl; }
		else if (key == "q") { cout << "Good bye" << endl; break; }
	}

	return 0;
}
