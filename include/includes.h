#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/ml.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <map>

#include <stdint.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#if defined(__APPLE__) || defined(__linux__)
#include <dirent.h>
#else
#include "../include/dirent.h"
#endif

#ifdef min

#undef min

#undef max

#endif

using namespace std;

using namespace cv;
