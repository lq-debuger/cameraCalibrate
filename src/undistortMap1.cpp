#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    FileStorage fs("./chessboard.yml", FileStorage::READ);
    cv::Size imageSize;
    fs["imageSize"] >> imageSize;

    Mat cameraMatrix, distCoeffs;
    fs["cameraMatrix"] >> cameraMatrix;
    fs["distCoeffs"] >> distCoeffs;

    std::cout << "imageSize: " << imageSize << std::endl;
    std::cout << "cameraMatrix: \n" << cameraMatrix << std::endl;
    std::cout << "distCoeffs: " << distCoeffs << std::endl;

    // 初始化两个map
    Mat map1, map2;
    initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), cameraMatrix, imageSize, CV_16SC2, map1, map2);


    const Mat image0 = cv::imread("./data/image_02.jpg", FileStorage::READ);

    Mat image;
    // 执行映射转换
    remap(image0, image, map1, map2,
          cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());

    bool showUndistorted = true;
    while (true) {

        if (showUndistorted) {
            // 显示已经去畸变的图undistorted
            printf("显示已经去畸变的图undistorted\n");
            imshow("image", image);
        }else {
            // 显示原图original
            printf("显示原图original\n");
            imshow("image", image0);
        }
        int action = waitKey(500) & 255;
        if (action == 27) {
            break;
        } else if (action == 32) {
            showUndistorted = !showUndistorted;
        }

    }
    destroyAllWindows();

    return 0;
}