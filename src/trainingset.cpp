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

    string last_label;
    int count = 0;
    for (auto &m : filePaths) {
        string &label = get<0>(m);
        string &file_path = get<1>(m);
        if (count == 0) {
            last_label = label;
            count++;
        }
        if (strcmp(label.c_str(), last_label.c_str()) != 0) {
            // cout << "Label and last " << label << " " << last_label << endl;
            last_label = label;
            count++;
        }
        // cout << file_path << endl;
        image_groups.push_back(make_tuple(count, imread(file_path)));
    }

}

int TrainingSet::calculate_surface_area(Mat &img) {
    (void) img;
    return -1;
}
