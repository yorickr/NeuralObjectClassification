#pragma once

#include "includes.h"
#include "bendingEnergy.h"

// Object features waar we naar gaan kijken:
// Oppervlakte          : int
// Rond                 : bool
// Vierkant             : bool
// Kleur                : int (HSV waarde) // misschien niet.
// Binding energy       : double
// Lengte               : int
// Breedte              : int

class SetEntry {
public:
    vector<Mat> images;
    string label;
    int id;
    int threshold_value;
    SetEntry(int id, int threshold_value, string label, vector<Mat> images);
};

class TrainingSet {
	public:
		TrainingSet(std::string directoryPath);

		string directoryPath;
		vector<tuple<string, string>> filePaths;

		vector<SetEntry> image_groups;
        vector<Mat> imgs;

        pair<Mat, Mat> compute(double mmPerPixel);

        Mat get_image(int i);
        string get_label(int i);		
	private:

};

class FeatureExtractor {
public:
	FeatureExtractor(double mmPerPixel);
	int calculate_surface_area(Mat &img, int thresh);

	bool calculate_if_square(Mat &img, int thresh);
	bool calculate_if_circle(Mat &img, int thresh);

	int calculate_length(Mat &img, int thresh);
	int calculate_width(Mat &img, int thresh);

	double calculate_keypoints(Mat &img, int thresh);

	double calculate_bending_energy(Mat &img, int thresh);
	double calculate_aspect_ratio(Mat &img, int thresh);
	double calculate_average_gray_value(Mat &img, int thresh);

	int calculate_perimeter(Mat &img, int thresh);
private:
	double mmPerPixel;
};
