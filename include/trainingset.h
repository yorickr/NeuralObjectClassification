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
    void setLabel();
};
