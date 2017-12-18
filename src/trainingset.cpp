#include "../include/trainingset.h"

void read_directory(const string& name, vector<string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if ((strcmp(dp->d_name, ".DS_Store") != 0) && (strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, ".."))) {
            v.push_back(dp->d_name);
        }
    }
    closedir(dirp);
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

    for (auto &m : filePaths) {
        cout << get<0>(m) << endl; // label
        cout << get<1>(m) << endl; // file
    }


	// this->setLabel();
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

int TrainingSet::calculate_surface_area(Mat &img) {
    (void) img;
    return -1;
}
