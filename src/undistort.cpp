#include <iostream>
#include "opencv2/opencv.hpp"

/*
 * 1.读取标定文件
 * 2.初始化两个map
 * 3.执行去畸变
 * */
using namespace std;
using namespace cv;

int main(){
    FileStorage fs("./chessboard.yml", FileStorage::READ);
    cv::Size imageSize;
    fs["imageSize"] >> imageSize;

    Mat cameraMatrix,distCoeffs;
    fs["cameraMatrix"]>> cameraMatrix;
    fs["distCoeffs"]>> distCoeffs;

    cout << "imageSize" << imageSize << endl;
    cout << "cameraMatrix" << cameraMatrix << endl;
    cout << "distCoeffs" << distCoeffs << endl;

    Mat image0 = imread("./data/image_12.jpg");
    Mat image;
    undistort(image0, image, cameraMatrix, distCoeffs);
    bool showUndistorted = true;
    while (true) {
        if (showUndistorted) {
            //显示已经去畸变的图
            cout << "显示去畸变图" << endl;
            imshow("image", image);
        } else {
            cout << "显示原图" << endl;
            imshow("image", image0);
        }
        int action = waitKey(500);
        if (action == 27) {
            break;
        } else if (action == 32) {
            showUndistorted = !showUndistorted;
        }
    }
    destroyAllWindows();
    return 0;
}