#include "../include/trainingset.h"

TrainingSet::TrainingSet(std::string directoryPath) {
    this->directoryPath = directoryPath;
    this->filePaths = std::vector<string>();

	this->setLabel();
}

/*Set directory name as label of trainingset*/
void TrainingSet::setLabel() {
	string s = this->directoryPath;
	string delim = "/";
	cout << s << endl;

	int start = 0;
	int end = s.find(delim);
	while (end != string::npos)
	{
		start = end + delim.length();
		end = s.find(delim, start);
	}
	this->label = s.substr(start, end);
	//cout << "Label is " << this->label << endl;
}
