#include <iostream>

#include "./include/bpn.h"
#include "./include/matrix.h"
#include "./include/trainingset.h"

#include "./include/includes.h"

#include "./include/trainingset.h"

using namespace std;

bool sort_p (pair<float, int> i, pair<float, int> j) { return (i.first>j.first); }

int main(int argc, char** argv) {
	srand(time(NULL));
	cout << boolalpha;
	cout << "Running with the following args: ";
	for (int i = 1; i < argc; i++) {
		cout << argv[i] << " ";
	}

	cout << endl;
	// size_t size_input = 5;
	// size_t size_hidden = 2;
	// size_t size_output = 10;
	//
	// // 	0,	1,	20724,	126,	179,
	// // 	0,	1,	21354,	181,	129,
	// // 	0,	1,	22377,	131,	181,
	// Matrix input(4, size_input);
	//
	// input[0][0] = 0;
	// input[0][1] = 1;
	// input[0][2] = 20724;
	// input[0][3] = 126;
	// input[0][4] = 179;
	//
	// input[1][0] = 0;
	// input[1][1] = 1;
	// input[1][2] = 21354;
	// input[1][3] = 181;
	// input[1][4] = 129;
	//
	// input[2][0] = 0;
	// input[2][1] = 1;
	// input[2][2] = 22377;
	// input[2][3] = 131;
	// input[2][4] = 181;
	//
	// input[3][0] = 0;
	// input[3][1] = 1;
	// input[3][2] = 22276;
	// input[3][3] = 133;
	// input[3][4] = 129;
	//
	// cout << endl << endl;
	// cout << "Training input" << endl << input << endl;
	//
	// // 		1,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	// // 		1,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	// // 		1,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	// // 		1,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	// Matrix output(4, size_output);
	// for (size_t row = 0; row < output.rows; row++) {
	// 	output[row][0] = 1;
	// }
	// cout << "Training expected output" << endl << output << endl;
	//
	// BPN bpn(input, output, size_hidden);
	// cout << bpn << endl;
	//
	// bpn.train(10000);
	//
	// for (size_t row = 0; row < input.rows; row++) {
	// 	Matrix r(input[row], true); // transpose
	// 	bpn.guess(r);
	// }

	TrainingSet s("./images/training_plaatjes/");
	// Mat img = s.image_groups.at(1).images.at(0);
	// int thresh = 60;
	// while (true) {
	// 	cin >> thresh;
	// 	Mat gray_image;
	// 	cvtColor(img, gray_image, CV_BGR2GRAY);
	// 	imshow("Gray", gray_image);
	// 	s.calculate_keypoints(gray_image, thresh);
	// 	waitKey(30);
	// }

	pair<Mat,Mat> out = s.compute();
	Mat input = out.first;
	Mat output = out.second;
	cout << "Input testset" << endl;
	cout << input << endl;
	cout << "Output testset" << endl;
	cout << output << endl;
	int networkInputSize = input.cols;
	int networkOutputSize = output.cols;
	Ptr<ml::ANN_MLP> mlp = ml::ANN_MLP::create();
	vector<int> layerSizes = { networkInputSize, 10, networkOutputSize };
	mlp->setLayerSizes(layerSizes);
	mlp->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM);
	mlp->setTrainMethod(ml::ANN_MLP::BACKPROP);
	mlp->train(input, ml::ROW_SAMPLE, output);
	cout << mlp->isTrained() << endl;

	cin.ignore();
	for (int row = 0; row < input.rows; row++) {
		Mat r = Mat::zeros( Size(input.cols, 1), CV_32F);
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
			} else {
				cout << "It could also be a " << s.get_label(max_index) << " with a " << (max*100) << "% chance."<< endl;
			}

		}
		cout << "Showing you the image" << endl;
		imshow("Object", s.get_image(row));
		waitKey(30);

		cin.ignore();
	}

	return 0;
}
