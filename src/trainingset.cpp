#include "../include/trainingset.h"

TrainingSet::TrainingSet(std::string directoryPath) {
    this->directoryPath = directoryPath;
    this->filePaths = std::vector<string>();
}

void TrainingSet::setLabel() {

}
