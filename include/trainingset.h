#pragma once

#include "includes.h"
#include "matrix.h"

// Object features waar we naar gaan kijken:
// Oppervlakte          : int
// Rond                 : bool
// Vierkant             : bool
// Kleur                : int (HSV waarde) // misschien niet.
// Binding energy       : int
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

    pair<Matrix, Matrix> compute();

    string get_label(int i);

    int calculate_surface_area(Mat &img, int thresh);
    bool calculate_if_square(Mat &img, int thresh);
    bool calculate_if_circle(Mat &img, int thresh);

private:

};
