//
// Created by lq on 2019/11/3.
//

#ifndef CAMERA_BOARD_H
#define CAMERA_BOARD_H

#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;
const int ACTION_ESC = 27;
const int ACTION_SPACE = 32;
enum BOARD{CHEESE,CHEESEVIDEO,CIRCLEVIDEO,ACIRCLEVIDEO};
int chessBoard(int board_width,int board_height,float squareSize);

int chessBoardVideo(int board_width, int board_height, float squareSize, int num_boards, float scale_factor, bool f);
int circleVideo(int board_width, int board_height, float squareSize, int num_boards, float scale_factor, bool f);

int acircleVideo(int board_width, int board_height, float square_size, int numboards, float scalefactor, bool f);

#endif //CAMERA_BOARD_H
