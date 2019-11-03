#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;
int main() {
    std::cout << "Hello, World!" << std::endl;
    std::stringstream ss;
    Mat src(200, 330, CV_8UC3);
    ss << "./camera_" << 1 << ".jpg";
    imwrite(ss.str(), src);
    cout << ss.str() << endl;
    return 0;
}