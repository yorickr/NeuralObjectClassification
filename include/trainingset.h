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
    void loadFiles();

    string label;
    string directoryPath;
    vector<tuple<string, string>> filePaths;

    int calculate_surface_area(Mat &img);

private:
    /*Set the directory name as label for this set*/
    void setLabel();
};
