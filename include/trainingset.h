#pragma once

#include "includes.h"

// Object features waar we naar gaan kijken:
// Oppervlakte          : int
// Rond                 : bool
// Vierkant             : bool
// Kleur                : int (HSV waarde) // misschien niet.
// Binding energy       : int
// Lengte               : int
// Breedte              : int

class TrainingSet {
public:
    TrainingSet(std::string directoryPath);

    string directoryPath;
    vector<tuple<string, string>> filePaths;

    vector<tuple<int, Mat>> image_groups;

    int calculate_surface_area(Mat &img, int thresh);
	int calculate_length(Mat &img, int thresh);
	int calculate_width(Mat &img, int thresh);

private:

};
