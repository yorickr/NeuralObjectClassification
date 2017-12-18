#pragma once

#include <string>
#include <vector>

class TrainingSet {
    public:
    TrainingSet(std::string directoryPath);
    loadFiles();

    std::string label;
    std::string directoryPath;
    std::vector<string> filePaths;

    private:
    /*Set the directory name as label for this set*/
    void setLabel();
}