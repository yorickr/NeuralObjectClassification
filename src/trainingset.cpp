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
            image_groups.push_back(SetEntry(count, thresh, label, images)); // pass a copy;
            images.clear();

            last_label = label;
            count++;
        }
        images.push_back(imread(file_path, CV_LOAD_IMAGE_COLOR));
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

pair<Matrix, Matrix> TrainingSet::compute() {
    int amount_of_features = 3;
    int amount_of_objects = image_groups.size();
    int amount_of_images = amount_of_objects * image_groups.at(0).images.size();

    Matrix input_set(amount_of_images, amount_of_features);
    Matrix output_set(amount_of_images, amount_of_objects);
    int i = 0;
    for (SetEntry &m : image_groups) {
        // cout << "Going through SetEntry " << m.label << " " << m.id << " which has a thresh value of " << m.threshold_value << endl;
        int count = 0;
        for (Mat &img: m.images) {
            // cout << "Image " << count << endl;
            // cout << "circle\t---\tsquare\t---\tsurface_area\t---" << endl;
            // cout << "----------------------------------------------------" << endl;
            Mat gray_image;
        	cvtColor(img, gray_image, CV_BGR2GRAY);
            bool circle = calculate_if_circle(gray_image, m.threshold_value);
            bool square = calculate_if_square(gray_image, m.threshold_value);
            int surface_area = calculate_surface_area(gray_image, m.threshold_value);
            // cout << circle << "\t\t" << square << "\t\t" << surface_area << endl;
            // cout << "----------------------------------------------------" << endl;
            // cout << endl;


            // add to input_set
            input_set[i][0] = (double) circle;
            input_set[i][1] = (double) square;
            input_set[i][2] = (double) surface_area;
            output_set[i][m.id] = 1;

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
    Mat bin;
    threshold(img, bin, thresh, 255, THRESH_BINARY);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( bin, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) ); // only find external contours.

    Mat drawing = Mat::zeros( bin.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color( rand()&255, rand()&255, rand()&255 );
        // drawContours( drawing, contours, i, color, FILLED, 8, hierarchy );
    }
    // imshow("Contour", drawing);

    vector<Point> approx;
    vector<vector<Point>> squares;

    // calculate squares.
    for( size_t i = 0; i < contours.size(); i++ )
    {
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
        if(approx.size() == 4 && fabs(contourArea(Mat(approx))) > 1000 && isContourConvex(Mat(approx)) )
        {
            double maxCosine = 0;

            for( int j = 2; j < 5; j++ )
            {
                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                maxCosine = MAX(maxCosine, cosine);
            }
            if( maxCosine < 0.3 )
                squares.push_back(approx);
        }
    }

    // drawing = Mat::zeros( bin.size(), CV_8UC3 );

    // drawSquares(drawing, squares);
    bool square = squares.size() > 0;
    // cout << "Is square? " << square << endl;
    return square;

}

bool TrainingSet::calculate_if_circle(Mat &img, int thresh) {
    Mat gray_image(img);
    // GaussianBlur( gray_image, gray_image, Size(9, 9), 2, 2 );

    // Mat drawing = Mat::zeros(gray_image.size(), CV_8UC3);
    vector<Vec3f> circles;
    HoughCircles(gray_image, circles, HOUGH_GRADIENT, 1, 50, thresh, thresh*3, 30, 100);
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Vec3i c = circles[i];
        // circle( drawing, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, LINE_AA);
        // circle( drawing, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, LINE_AA);
    }

    // imshow("Found circles", drawing);
    bool circle = circles.size() > 0;
    // cout << "Is circle? " << circle << endl;
    return circle;
}
