#include "../include/trainingset.h"


double angle( Point pt1, Point pt2, Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void drawSquares( Mat& image, const vector<vector<Point> >& squares ) {
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, LINE_AA);
    }

    imshow("Square", image);
}

void read_directory(const string& name, vector<string>& v) {
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if ((strcmp(dp->d_name, ".DS_Store") != 0) && (strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, ".."))) {
            v.push_back(dp->d_name);
        }
    }
    closedir(dirp);
}

SetEntry::SetEntry(int id, int threshold_value, string label, vector<Mat> images) {
    this->id = id;
    this->threshold_value = threshold_value;
    this->images = images;
    this->label = label;
}

bool greatestVector(const vector<cv::Point>& lhs, const vector<cv::Point>& rhs)
{
	return lhs.size() < rhs.size();
}

vector<Point> getGreatestVector(vector<vector<Point>> vectors) {
	auto result = max_element(vectors.begin(), vectors.end(), greatestVector);
	return vectors[distance(vectors.begin(), result)];

}

TrainingSet::TrainingSet(std::string directoryPath) {
    this->directoryPath = directoryPath;

    vector<string> directories;
    read_directory(directoryPath, directories);
    for (auto &m : directories) {
        vector<string> files;
        read_directory(directoryPath + "/" + m, files);
        for (auto &n : files) {
            filePaths.push_back(make_tuple<string,string>(string(m), string(directoryPath + "/" + m + "/" + n)));
        }
    }

    string last_label = get<0>(filePaths.at(0));
    int count = 0;
    vector<Mat> images;
    for (auto &m : filePaths) {
        string &label = get<0>(m);
        string &file_path = get<1>(m);
        if (strcmp(label.c_str(), last_label.c_str()) != 0) {
            // cout << "Label and last " << label << " " << last_label << endl;
            int thresh = 25;
            image_groups.push_back(SetEntry(count, thresh, last_label, images)); // pass a copy;
            images.clear();

            last_label = label;
            count++;
        }
        Mat imag = imread(file_path, CV_LOAD_IMAGE_COLOR);
        images.push_back(imag);
        imgs.push_back(imag);
    }
    // catch the final one.
    int thresh = 25;
    image_groups.push_back(SetEntry(count, thresh, last_label, images)); // pass a copy;
}

string TrainingSet::get_label(int i) {
    for (SetEntry &s : image_groups) {
        if (s.id == i) {
            return s.label;
        }
    }
    return "";
}

Mat TrainingSet::get_image(int i) {
    return imgs.at(i);
}

pair<Mat, Mat> TrainingSet::compute() {
    int amount_of_features = 9;
    int amount_of_objects = image_groups.size();
    int amount_of_images = amount_of_objects * image_groups.at(0).images.size();

    // Matrix input_set(amount_of_images, amount_of_features);
    // Matrix output_set(amount_of_images, amount_of_objects);
    Mat input_set = Mat::zeros( Size(amount_of_features, amount_of_images), CV_32F);
    Mat output_set = Mat::zeros( Size(amount_of_objects, amount_of_images), CV_32F);
    int i = 0;
    for (SetEntry &m : image_groups) {
        // cout << "Going through SetEntry " << m.label << " " << m.id << " which has a thresh value of " << m.threshold_value << endl;
        int count = 0;
        for (Mat &img: m.images) {
            Mat gray_image;
        	cvtColor(img, gray_image, CV_BGR2GRAY);
            bool circle = calculate_if_circle(gray_image, m.threshold_value);
            bool square = calculate_if_square(gray_image, m.threshold_value);
            int surface_area = calculate_surface_area(gray_image, m.threshold_value);
            int length = calculate_length(gray_image, m.threshold_value);
            int width = calculate_width(gray_image, m.threshold_value);
            double bending_energy = calculate_bending_energy(gray_image, m.threshold_value);
            double keypoint_count = calculate_keypoints(gray_image, m.threshold_value);
            double aspect_ratio = calculate_aspect_ratio(gray_image, m.threshold_value);
            double avg_gray = calculate_average_gray_value(gray_image, m.threshold_value);
            // cout << "Image " << count << endl;
            // cout << "circle\t---\tsquare\t---\tsurface_area\t---\tlength\t---\twidth" << endl;
            // cout << "----------------------------------------------------" << endl;
            // cout << circle << "\t\t" << square << "\t\t" << surface_area << "\t\t\t" << length << "\t\t" << width << endl;
            // cout << "----------------------------------------------------" << endl;
            // cout << endl;


            // add to input_set
            input_set.at<float>(i, 0) = (float) circle;
            input_set.at<float>(i, 1) = (float) square;
            input_set.at<float>(i, 2) = (float) surface_area;
            input_set.at<float>(i, 3) = (float) length;
            input_set.at<float>(i, 4) = (float) width;
            input_set.at<float>(i, 5) = bending_energy;
            input_set.at<float>(i, 6) = aspect_ratio;
            input_set.at<float>(i, 7) = keypoint_count;
            input_set.at<float>(i, 8) = avg_gray;
            output_set.at<float>(i, m.id) = (float) 1;

            // input_set[i][0] = (double) circle;
            // input_set[i][1] = (double) square;
            // input_set[i][2] = (double) surface_area;
            // input_set[i][3] = (double) length;
            // input_set[i][4] = (double) width;
            // output_set[i][m.id] = 1;

            i++;
            count++;
        }
        // cout << endl << endl;
    }
    return make_pair(input_set, output_set);
}

// give this a gray_image
int TrainingSet::calculate_surface_area(Mat &img, int thresh) {
    Mat bin;
    threshold(img, bin, thresh, 255, THRESH_BINARY);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) ); // only find external contours.

    int sum = 0;
    // Mat drawing = Mat::zeros( bin.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        // Scalar color( rand()&255, rand()&255, rand()&255 );
        // drawContours( drawing, contours, i, color, FILLED, 8, hierarchy );
        sum+= contourArea(contours[i]);
    }
    // imshow("Contour", drawing);
    // cout << "Sum is " << sum << endl;
    return sum;
}

// give this a gray_image
bool TrainingSet::calculate_if_square(Mat &img, int thresh) {
    double aspect_ratio = calculate_aspect_ratio(img, thresh);
    bool square = false;
    if ((aspect_ratio < 1.15) && (aspect_ratio > 0.85)) {
        square = true;
    }
    return square;
}

bool TrainingSet::calculate_if_circle(Mat &img, int thresh) {
    Mat gray_image(img);
    // GaussianBlur( gray_image, gray_image, Size(9, 9), 2, 2 );

    // Mat drawing = Mat::zeros(gray_image.size(), CV_8UC3);
    vector<Vec3f> circles;
    HoughCircles(gray_image, circles, HOUGH_GRADIENT, 1, 50, thresh, thresh*3, 30, 100);
    // for( size_t i = 0; i < circles.size(); i++ )
    // {
        // Vec3i c = circles[i];
        // circle( drawing, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, LINE_AA);
        // circle( drawing, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, LINE_AA);
    // }

    // imshow("Found circles", drawing);
    bool circle = circles.size() > 0;
    // cout << "Is circle? " << circle << endl;
    return circle;
}

int TrainingSet::calculate_length(Mat & img, int thresh)
{
	Mat bin;
	threshold(img, bin, thresh, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

	auto contour = getGreatestVector(contours);
	RotatedRect boundaryRotatedBox = minAreaRect(contour);

	return boundaryRotatedBox.size.height;
}

int TrainingSet::calculate_width(Mat & img, int thresh)
{
	Mat bin;
	threshold(img, bin, thresh, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

	auto contour = getGreatestVector(contours);
	RotatedRect boundaryRotatedBox = minAreaRect(contour);

	return boundaryRotatedBox.size.width;
}

double TrainingSet::calculate_keypoints(Mat &img, int thresh) {

    SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = thresh;
    params.maxThreshold = 255;
    vector<KeyPoint> keypoints;
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    detector->detect( img, keypoints);

    // Mat im_with_keypoints;
	// drawKeypoints( img, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    // cout << keypoints.size() << endl;
	// Show blobs
	// imshow("keypoints", im_with_keypoints );
    // waitKey(300);

    return (double) keypoints.size();
}

double TrainingSet::calculate_aspect_ratio(Mat &img, int thresh) {
    Mat bin;
    threshold(img, bin, thresh, 255, THRESH_BINARY);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) ); // only find external contours.

    vector<Point> largest_contour;
    // Mat drawing = Mat::zeros( bin.size(), CV_8UC3 );
    int last_contour_area = numeric_limits<int>::min();
    for( size_t i = 0; i< contours.size(); i++ )
    {
        // Scalar color( rand()&255, rand()&255, rand()&255 );
        // drawContours( drawing, contours, i, color, FILLED, 8, hierarchy );
        int area = contourArea(contours[i]);
        if (area > last_contour_area) {
            largest_contour = contours[i];
            last_contour_area = area;
        }
    }

    RotatedRect r = minAreaRect(largest_contour);
    // cout << "Rect " << r.size << endl;
    int max = r.size.width;
    int min = r.size.height;
    if (min > max) {
        int temp = max;
        max = min;
        min = temp;
    }
    return (double)max / min;
}

double TrainingSet::calculate_average_gray_value(Mat &img, int thresh) {
    int total = img.rows * img.cols;
    double avg = 0.0;
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            avg += img.at<uchar>(r, c);
        }
    }

    return (double) avg/total;
}

double TrainingSet::calculate_bending_energy(Mat & img, int thresh)
{
	Mat bin;
	threshold(img, bin, thresh, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
/*
	Mat drawing = Mat::zeros(bin.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++) {
		drawContours(drawing, contours, (int)i, Scalar(rand() % 255, rand() % 255, rand() % 255));
	}*/

	auto contour = getGreatestVector(contours);

	//imshow("Contour: " + to_string(contour.size()), drawing);

	return bendingEnergy(contour);
}
