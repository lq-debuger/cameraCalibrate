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
    FileStorage fs("./inCaliFileACirclePathVideo.yml", FileStorage::READ);
    cv::Size imageSize;
    fs["imageSize"] >> imageSize;

    Mat cameraMatrix,distCoeffs;
    fs["cameraMatrix"]>> cameraMatrix;
    fs["distCoeffs"]>> distCoeffs;

    cout << "imageSize" << imageSize << endl;
    cout << "cameraMatrix" << cameraMatrix << endl;
    cout << "distCoeffs" << distCoeffs << endl;

    //初始化两个map
    Mat map1,map2;
    initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), cameraMatrix, imageSize, CV_16SC2, map1, map2);
    VideoCapture capture(0);
    if (!capture.isOpened()) {
        cout << "could'nt open the camera" << endl;
        return -1;
    }
    while (true) {
        Mat image,image0;
        capture >> image0;
        if (image0.empty()) {
            break;
        }
        //执行映射转换
        remap(image0, image, map1, map2, INTER_LINEAR, BORDER_CONSTANT, Scalar());
        imshow("original", image0);
        imshow("undistorted", image);
        if ((waitKey(30) & 255) == 27) {
            break;
        }
    }
    destroyAllWindows();
    return 0;
}