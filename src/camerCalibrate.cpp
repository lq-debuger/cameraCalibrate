#include "opencv2/opencv.hpp"
#include <iostream>

#include "board.h"

// ./cameraCalibrate 4 11 20 15 --shape=circle --scale=1.0 -f

void printHelp(){
    cout << "进行相机标定" << endl;
    cout << "进行相机标定" << endl;
    cout << "参数说明： ./cameraCalibrateACircledVideo <board_width> <board_height> <square_size> [num_boards] [--shape = board_type] [--scale|-s = scale_factor] [-f] " << endl;
    cout << "使用示例： ././cameraCalibrateACircledVideo 4 11 20 15 --shape = chessboard|chessboardVideo|circleVideo|AcircleVideo --scale=1.0 -f" << endl;
}

int main(int argc, char **argv){
    //解析用户输入的参数
    cv::CommandLineParser parser(argc, argv,
                                 "{@arg1||}{@arg2||}{@arg3||}{@arg4|15|}{shape sh||}{help h||}{scale s|1.0|}{f ||}");

    if (parser.has("h")) {
        printHelp();
        return 0;
    }
    int board_width = parser.get<int>(0);
    int board_height = parser.get<int>(1);
    float square_size = parser.get<float>(2);
    int num_boards = parser.get<int>(2);
    if (board_width < 1 || board_height < 1 || square_size < 0) {
        cerr << "请输入正确的标定板宽高及尺寸 " << endl;
        printHelp();
        return -1;
    }
    //进行反转的标定
    bool flip = false;
    if (parser.has("f")) {
        flip = true;
    }
    float scale_factor = parser.get<float>("s");

    String board_type = parser.get<String>("sh");
    cout << "111" << endl;
    if (board_type == "chessboard") {
        //进行棋盘图片的标定
        chessBoard(board_width,board_height,square_size);
    } else if (board_type == "chessboardVideo") {
        //进行棋盘摄像头的标定
        chessBoardVideo(board_width,board_height,square_size,num_boards,scale_factor,flip);
    }else if (board_type == "circleVideo") {
        //进行圆网格摄像头的标定
        circleVideo(board_width,board_height,square_size,num_boards,scale_factor,flip);
    }else if (board_type == "AcircleVideo") {
        //进行非对称圆网格摄像头的标定
        acircleVideo(board_width,board_height,square_size,num_boards,scale_factor,flip);
    }
}