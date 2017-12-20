// #include "opencv/cv.h"
// #include "opencv/highgui.h"
// #include <iostream>
// #include <fstream>
//
// using namespace cv;
// using namespace std;

#include "../include/calibratie.h"

int calibrate(string cameraCalibrationDataFilePath) {
	// The number of boards you want to capture, the number of internal corners horizontally
	// and the number of internal corners vertically (That's just how the algorithm works).
	int numBoards;
	int numCornersHor;
	int numCornersVer;

	cout << "Enter number of corners along width: ";
	cin >> numCornersHor;

	cout << "Enter number of corners along height: ";
	cin >> numCornersVer;

	cout << "Enter number of boards: ";
	cin >> numBoards;

	// We also create some additional variables that we'll be using later on.
	int numSquares = numCornersHor * numCornersVer;
	Size board_sz = Size(numCornersHor, numCornersVer);

	// We want live feed for our calibration!
	VideoCapture capture = VideoCapture(1);

	// - object_points is the physical position of the corners (in 3D space).
	//    This has to be measured by us.
	// - image_points is the location of the corners in the image (in 2 dimensions).
	// - Once the program has actual physical locations and locations on the image, it can calculate
	//    the relation between the two. Because we'll use a chessboard, these points have a definite
	//    relations between them (they lie on straight lines and on squares).
	// - So the "expected" - "actual" relation can be used to correct the distortions in the image.
	vector<vector<Point3f>> object_points;
	vector<vector<Point2f>> image_points;

	// Next, we create a list of corners. This will temporarily hold the current snapshot's chessboard corners.
	// keep track of the number of successfully captured chessboards
	vector<Point2f> corners;
	int successes = 0;

	// - Create a list of coordinates (0,0,0), (0,1,0), (0,2,0)...(1,4,0)... so on.
	//   Each corresponds to a particular vertex.
	// - You're essentially setting up the units of calibration.
	//   Suppose the squares in your chessboards were 30 mm in size and you supplied these
	//   coordinates as (0,0,0), (0, 30, 0), etc, you'd get all unknowns in millimeters.
	vector<Point3f> obj;
	for (int j = 0; j < numSquares; j++)
		obj.push_back(Point3f(j / numCornersHor, j%numCornersHor, 0.0f));

	// Then we create two images and get the first snapshot from the camera:
	Mat image;
	Mat gray_image;
	capture >> image;

	// As long as the number of successful entries has been less than the number required,
	// we keep looping:
	while (successes < numBoards)
	{
		// convert to gray scale
		cvtColor(image, gray_image, CV_BGR2GRAY);

		// And we're here. The key functions:
		// findChessboardCorners tries to find a chessboard in the image.
		// IF found THEN the rough corners are returned.
		bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if (found)
		{
			// cornerSubPix refines the found corners.
			// De rough corners returned by findChessbooardCorners and the gray_image are input,
			// the refined corners are output.
			cornerSubPix(gray_image, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));

			// draw the refined corners on the image i.e. chessboard.
			drawChessboardCorners(gray_image, board_sz, corners, found);
		}

		// show results
		imshow("orginal", image);
		imshow("gray image", gray_image);

		capture >> image;

		int key = waitKey(1);

		// Leave the program by pressing ESC-key
		if (key == 27) return 0;

		// spacebar and chessboard found ==> save the snap
		if (key == ' ' && found != 0)
		{
			image_points.push_back(corners);
			object_points.push_back(obj);

			printf("Snap stored!");

			successes++;

			if (successes >= numBoards)
				break;
		}
	} // while

	// Next, we get ready to do the calibration. We declare variables that will hold the unknowns:
	// Matrix intrinsic contains cx,cy,fx,fy
	// Matrix disCoeffs contains the distortion coefficients: 3 numbers radial distortion and 2 numbers tangential distortion
	Mat intrinsic = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;

	// We modify the intrinsic matrix with whatever we know.
	// The camera's aspect ratio is 1 (that's usually the case...
	// i.e. fx = fy = f. If not, change it as required.
	// Elements (0,0) and (1,1) are the focal lengths along the X and Y axis.
	intrinsic.ptr<float>(0)[0] = 1;
	intrinsic.ptr<float>(1)[1] = 1;

	// Determine the intrinsic matrix, distortion coefficients and the rotation+translation vectors.
	// Note: The calibrateCamera function converts all matrices into 64F format even if you
	// initialize it to 32F.
	calibrateCamera(object_points, image_points, image.size(), intrinsic, distCoeffs, rvecs, tvecs);

	cout << endl << endl << "intrinsic = " << intrinsic << endl << endl;
	cout << "distCoeffs = " << distCoeffs << endl;

	// Now that we have the distortion coefficients, we can undistort the images.
	// Here's a small loop that will do this:
	// Now we will gonna calibrate the size of a pixel per millimeter
	Mat imageUndistorted, bin;

	int thresh = 25;
	double mmPerPixel = -1;

	Scalar cc(255, 0, 0);
	while (1)
	{
		capture >> image;
		undistort(image, imageUndistorted, intrinsic, distCoeffs);

		//make the frame gray and make a binary image of the frame;		
		cvtColor(imageUndistorted, gray_image, CV_BGR2GRAY);
		threshold(gray_image, bin, thresh, 255, THRESH_BINARY);

		//looking the contour of the round object
		vector<vector<Point>> contours;
		findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

		if (contours.size() > 1) {
			putText(imageUndistorted, "Please no more than 1 rond object", Point(20, imageUndistorted.rows / 2), FONT_HERSHEY_SIMPLEX, 1, cc);
		}
		else if (contours.size() == 0) {
			putText(imageUndistorted, "No round object found", Point(20, imageUndistorted.rows / 2), FONT_HERSHEY_SIMPLEX, 1, cc);
		}
		else {
			drawContours(imageUndistorted, contours, 0, cc, 2);
			putText(imageUndistorted, "Press Space to give object size and go to console", Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, cc);
		}

		imshow("orginal", imageUndistorted);
		imshow("gray image", gray_image);
		imshow("bin", bin);

		int key = waitKey(1);
		if (key == 27) break;
		else if (key == ' ' && contours.size() == 1) {
			auto contour = contours[0];

			//get real diameter of round object
			double realDiameter = -1;
			while (realDiameter < 0) {
				cout << "Fill in the largest diameter in millimeters of the object" << endl;
				cin >> realDiameter;
			}

			//calculate the min en max values of x and y of the round object in pixels
			auto mmx = minmax_element(contour.begin(), contour.end(), [](const Point &l, const Point &r) {return l.x < r.x; });
			auto mmy = minmax_element(contour.begin(), contour.end(), [](const Point &l, const Point &r) {return l.y < r.y; });

			//calculate the width and height of the object in pixels
			auto width = mmx.second->x - mmx.first->x;
			auto height = mmy.second->y - mmy.first->y;

			//define the diameter of the object in pixels, the largest dimension
			double diameterInPixel = (double)width;
			if (width < height) { diameterInPixel = (double)height; }

			//calculate the millimeters per pixel value
			mmPerPixel = realDiameter / diameterInPixel;

			//show the calulated values
			cout << "Diameter in pixels: " << diameterInPixel << endl;
			cout << "Size per pixel: " << mmPerPixel << endl;
			cout << "Diameter in millimeters: " << (diameterInPixel * mmPerPixel) << endl;
		}
	}

	/***** saven van de callibratie data *****/

	// YML-file aanmaken
	FileStorage fs(cameraCalibrationDataFilePath, FileStorage::WRITE);

	// wegschrijven van callibratie data naar de YML-file 
	fs << "intrinsic" << intrinsic << "distCoeffs" << distCoeffs << "mm_per_pixel" << mmPerPixel;

	// de file afsluiten
	fs.release();

	return 1;
}