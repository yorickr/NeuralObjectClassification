#pragma once

#include "includes.h"

class TrainingSet {
    public:
    TrainingSet(std::string directoryPath);
    void loadFiles();

    std::string label;
    std::string directoryPath;
    std::vector<string> filePaths;

    private:
    /*Set the directory name as label for this set*/
    void setLabel();
};
