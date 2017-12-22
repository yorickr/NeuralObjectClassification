#include <iostream>

#include "./include/bpn.h"
#include "./include/matrix.h"
#include "./include/trainingset.h"

#include "./include/includes.h"
#include "./include/calibratie.h"

using namespace std;

double mmPerPixel = -1;
string cameraCalibrationFilePath = "C:\\School\\ueyecallib.yml";
TrainingSet s("C:\\Users\\remco\\Documents\\School\\Minor robotics and vision\\Periode 2\\Vision c++ voor Gevorderden\\NeuralObjectClassification\\NeuralObjectClassification\\NeuralObjectClassification\\images\\training_plaatjes");
Ptr<ml::ANN_MLP> mlp;
Mat input;

bool sort_p (pair<float, int> i, pair<float, int> j) { return (i.first>j.first); }

int runBasicTrainingSet() {
	cout << endl;
	
	pair<Mat, Mat> out = s.compute(mmPerPixel);
	input = out.first;
	Mat output = out.second;
	cout << "Input testset" << endl;
	cout << input << endl;
	cout << "Output testset" << endl;
	cout << output << endl;
	int networkInputSize = input.cols;
	int networkOutputSize = output.cols;
	mlp = ml::ANN_MLP::create();
	vector<int> layerSizes = { networkInputSize, 10, networkOutputSize };
	mlp->setLayerSizes(layerSizes);
	mlp->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM);
	mlp->setTrainMethod(ml::ANN_MLP::BACKPROP);
	mlp->train(input, ml::ROW_SAMPLE, output);
	cout << "MLP is trained: " << mlp->isTrained() << endl;

	return 1;
}

int liveDetection() {
	FeatureExtractor fe(mmPerPixel);
	int amount_of_features = 9, i = 0;
	if (!mlp->isTrained()) {
		cout << "First train the mlp" << endl;
		return -1;
	}

	VideoCapture capture = VideoCapture(1);
	Mat frame, gray_image;
	//frame = imread("C:/School/sizeOfPixel.jpg");
	capture >> frame;

	//check if oke

	int thresh = 25;
	while (true) {
		cvtColor(frame, gray_image, CV_BGR2GRAY);

		int key = waitKey(1);
		if (key == 27) break;
		if (key == ' ') {
			Mat input_set = Mat::zeros(Size(amount_of_features, 1), CV_32F);
			bool circle = fe.calculate_if_circle(gray_image, thresh);
			bool square = fe.calculate_if_square(gray_image, thresh);
			int surface_area = fe.calculate_surface_area(gray_image, thresh);
			int length = fe.calculate_length(gray_image, thresh);
			int width = fe.calculate_width(gray_image, thresh);
			double bending_energy = fe.calculate_bending_energy(gray_image, thresh);
			double keypoint_count = fe.calculate_keypoints(gray_image, thresh);
			double aspect_ratio = fe.calculate_aspect_ratio(gray_image, thresh);
			double avg_gray = fe.calculate_average_gray_value(gray_image, thresh);

			input_set.at<float>(i, 0) = (float)circle;
			input_set.at<float>(i, 1) = (float)square;
			input_set.at<float>(i, 2) = (float)surface_area;
			input_set.at<float>(i, 3) = (float)length;
			input_set.at<float>(i, 4) = (float)width;
			input_set.at<float>(i, 5) = bending_energy;
			input_set.at<float>(i, 6) = aspect_ratio;
			input_set.at<float>(i, 7) = keypoint_count;
			input_set.at<float>(i, 8) = avg_gray;
			
			Mat outp;
			mlp->predict(input_set, outp);
			cout << outp << endl;

			int j = 0;

			for (auto const &o : s.image_groups) {
				cout << o.label << ", " << o.id << ", " << j << endl;
				j++;
			}
			vector<pair<float, int>> vec;
			float max = numeric_limits<float>::min();
			int max_index = 0;
			
			for (int i = 0; i < outp.cols; i++) {
				float val = outp.at<float>(0, i);
				vec.push_back(make_pair(val, i));
			}

			sort(vec.begin(), vec.end(), sort_p);
			for (size_t i = 0; i < 3; i++) {
				max = vec[i].first;
				max_index = vec[i].second;
				if (i == 0) {
					cout << "There is a " << (max * 100) << "% chance that this is a " << s.get_label(max_index) << endl;
				}
				else {
					cout << "It could also be a " << s.get_label(max_index) << " with a " << (max * 100) << "% chance." << endl;
				}

			}	
		}

		imshow("Live feed", frame);

		capture >> frame;
	}

	return 1;
}

void testTrainingsSet() {
	for (int row = 0; row < input.rows; row++) {
		Mat r = Mat::zeros(Size(input.cols, 1), CV_32F);
		// cout << "input" << endl << input << endl;
		for (int j = 0; j < input.cols; j++) {
			float val = input.at<float>(row, j);
			r.at<float>(0, j) = val;
		}
		cout << r << endl;
		Mat outp;
		mlp->predict(r, outp);
		cout << outp << endl;
		vector<pair<float, int>> vec;
		float max = numeric_limits<float>::min();
		int max_index = 0;
		// top 3;
		vec.push_back(make_pair(max, max_index));
		vec.push_back(make_pair(max, max_index));
		vec.push_back(make_pair(max, max_index));

		for (int i = 0; i < outp.cols; i++) {
			float val = outp.at<float>(0, i);
			vec.push_back(make_pair(val, i));
		}
		sort(vec.begin(), vec.end(), sort_p);
		for (size_t i = 0; i < 3; i++) {
			max = vec[i].first;
			max_index = vec[i].second;
			if (i == 0) {
				cout << "There is a " << (max * 100) << "% chance that this is a " << s.get_label(max_index) << endl;
			}
			else {
				cout << "It could also be a " << s.get_label(max_index) << " with a " << (max * 100) << "% chance." << endl;
			}

		}
		cout << "Showing you the image" << endl;
		imshow("Object", s.get_image(row));
		waitKey(30);

		cin.ignore();
	}
}

void loadCalibrationData() {
	FileStorage fs(cameraCalibrationFilePath, FileStorage::READ);
	// callibratie data ophalen
	Mat intrinsic, distCoeffs;
	fs["intrinsic"] >> intrinsic;
	fs["distCoeffs"] >> distCoeffs;
	fs["mm_per_pixel"] >> mmPerPixel;

	// callibratie matrices tonen op het scherm
	cout << "intrinsic matrix: " << intrinsic << endl;
	cout << "distortion coeffs: " << distCoeffs << endl;
	cout << "mm per pixel: " << mmPerPixel << endl;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	cout << boolalpha;

	loadCalibrationData();

	string key;
	while (true) {
		cout << "Enter a key: c for calibration, r for item detection, t for training basic set, i for test trainingsset or q to quit" << endl;
		cin >> key;

		if (key == "c") {
			calibrate(cameraCalibrationFilePath);
			cout << "Calibration done" << endl;
			destroyAllWindows();
			loadCalibrationData();
			cin.ignore();
		}
		else if (key == "r") { 
			liveDetection();
			destroyAllWindows();
		}
		else if (key == "t") { runBasicTrainingSet(); }
		else if (key == "i") { 
			testTrainingsSet(); 
			destroyAllWindows();
		}
		else if (key == "q") { cout << "Good bye" << endl; break; }
	}

	return 0;
}


