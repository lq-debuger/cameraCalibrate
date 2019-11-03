#include "board.h"
//发现并且绘制角点
bool findCorners(const cv::Mat &image, const cv::Size &patternSize, vector<cv::Point2f> &corners, BOARD board) {
    if (board == CHEESE || board == CHEESEVIDEO) {
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        //查找当前图片的所有角点
        bool patterWasFound = cv::findChessboardCorners(gray, patternSize, corners);
        if (patterWasFound) {//找到角点
            //提高角点的精确度
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

        }
        //将所有的角点在云图中绘制出来
        cv::drawChessboardCorners(image, patternSize, corners, patterWasFound);
        //绘制完成后显示原图
        cv::imshow("src", image);
        return patterWasFound;
    }
    if (board == ACIRCLEVIDEO || board == CIRCLEVIDEO) {
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        //查找当前图片的所有圆网格的中心
        bool patterWasFound = cv::findCirclesGrid(gray, patternSize, corners,cv::CALIB_CB_ASYMMETRIC_GRID);

        //将所有的角点在云图中绘制出来
        cv::drawChessboardCorners(image, patternSize, corners, patterWasFound);
        //绘制完成后显示原图
        cv::imshow("src", image);
        return patterWasFound;
    }
}
//计算坐标
void calcObjPoints(const cv::Size &patternSize, float squareSize, vector<cv::Point3f> &objPoints,BOARD board) {
    if (board == CHEESE || board == CHEESEVIDEO || board == CIRCLEVIDEO) {
        for (int i = 0; i < patternSize.height; ++i) {
            for (int j = 0; j < patternSize.width; ++j) {
                objPoints.emplace_back(j * squareSize, i * squareSize, 0);
            }
        }
    }
    if (board == ACIRCLEVIDEO) {
        for (int i = 0; i < patternSize.height; ++i) {
            for (int j = 0; j < patternSize.width; ++j) {
                objPoints.emplace_back(float(j*2+i%2)*squareSize, float(i * squareSize), 0);
            }
        }
    }
}
//保存标定文件
void save2yml(cv::String saveFileName,
               const cv::Size &imageSize, const cv::Size &patternSize, float square_size, int num_boards,
               const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs) {
    //    cv::FileStorage fs("./calib_chessboard.xml", cv::FileStorage::WRITE);
    cv::FileStorage fs(saveFileName, cv::FileStorage::WRITE);
    // 获取当前时间
    time_t tm;
    time(&tm);
    struct tm *t2 = localtime(&tm);
    char buf[1024];
    strftime(buf, sizeof(buf), "%c", t2);

    // 图片宽高，标定板宽高，标定板尺寸，图片个数
    // 相机内参，畸变系数，RMS
    fs << "calib_time" << buf;
    fs << "imageSize" << imageSize;
    fs << "patternSize" << patternSize;
    fs << "square_size" << square_size;
    fs << "num_boards" << num_boards;
    fs << "cameraMatrix" << cameraMatrix;
    fs << "distCoeffs" << distCoeffs;

    fs.release();
}

//棋盘格图片的标定
int chessBoard(int board_width,int board_height,float squareSize){
    //保存多张图片对象列表
    vector<vector<cv::Point3f>> objectPoints;
    //保存多张图片角点列表
    vector<vector<cv::Point2f>> imagesPoints;
    //棋盘的尺寸（宽：6，高：9）
    const cv::Size patternSize(board_width, board_height);
    int num_boards = 1;
    //图片的像素尺寸
    cv::Size imageSize;
    //1.加载所有的照片
    //图片路径
    cv::String pic_path = "./data/image_*.jpg";
    vector<cv::String> fileNames;
    //获取路径下所有的文件名
    cv::glob(pic_path, fileNames);
    cout << "fileNames size:" << fileNames.size() << endl;
    for (cv::String fille: fileNames) {
        //读取图片
        cv::Mat src = cv::imread(fille);
        cv::Size srcSize = src.size();
        cv::Mat image;
        src.copyTo(image);
//        cout << srcSize << endl;
        imageSize = image.size();
        //2.遍历查找每个照片的角点
        vector<cv::Point2f> corners;
//        cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1);
        bool patternFound = findCorners(image, patternSize, corners,CHEESE);

        //如果找到了角点，把角点和对应的3d坐标加到列表中
        if (patternFound) {
            // 4. 创建对应的物体3d坐标
            /**
             *  [0,0,0] [1,0,0] [2,0,0]
             *  [0,1,0] [1,1,0] [2,1,0]
             *  [0,2,0] [1,2,0] [2,2,0]
             */
            vector<cv::Point3f> objPoints;
            calcObjPoints(patternSize, squareSize, objPoints,CHEESE);

            imagesPoints.push_back(corners);
            objectPoints.push_back(objPoints);
        }


    }
    // 相机内参
    cv::Mat cameraMatrix;
    //畸变参数 distortion coefficients
    cv::Mat distCoeffs;
    //每张图片的旋转矩阵
    vector<cv::Mat> recvs;
    //每张图片的平移矩阵
    vector<cv::Mat> tvecs;

    //3.执行相机标定操作
    double rms = cv::calibrateCamera(objectPoints, imagesPoints, imageSize, cameraMatrix, distCoeffs, recvs, tvecs);

    cout << "rms == " << rms << endl;
    //4.打印并保存标定结果到文件
    cout << "cameraMatrix:\n" << cameraMatrix << endl;
    cout << "distCoeffs:\n" << distCoeffs << endl;

    save2yml("./chessboard.yml",imageSize,patternSize,squareSize,num_boards,cameraMatrix, distCoeffs);
    while (cv::waitKey(30) != 27){

    }
    cv::destroyAllWindows();

    return 0;
}

//棋盘格视频的标定
int chessBoardVideo(int board_width, int board_height, float squareSize, int num_boards, float scale_factor, bool f){
    //保存多张图片对象列表
    vector<vector<cv::Point3f>> objectPoints;
    //保存多张图片角点列表
    vector<vector<cv::Point2f>> imagesPoints;
    //棋盘的尺寸（宽：6，高：9）
    const cv::Size patternSize(board_width, board_height);


    //图片的像素尺寸
    cv::Size imageSize;

    //1.打开摄像头
    cv::VideoCapture capture(0);
    //判断相机是否打开
    if (!capture.isOpened()) {
        cerr << "相机无法打开，检查后重试" << endl;
        return -1;
    }

    while(true){
        cv::Mat image0,image;
        bool rst = capture.read(image0);
        if(!rst){
            cout << "未读到图片" << endl;
            continue;
        }
        image0.copyTo(image);

        //水平翻转图片
        if (f) {
            cv::flip(image, image, 1);

        }

        //执行缩放
        cv::resize(image, image, cv::Size(), scale_factor, scale_factor, cv::INTER_LINEAR);

        imageSize = image.size();

        //2.遍历查找每个照片的角点
        vector<cv::Point2f> corners;
//        cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1);
        bool patternFound = findCorners(image, patternSize, corners,CHEESEVIDEO);
        int action = cv::waitKey(30) ;
        if (action == 27 || action == 'q') {
            cout <<  "标定程序已经终止" << endl;
            return -1;
        } else if (action == 32) {

            if (patternFound) {
                // 4. 创建对应的物体3d坐标
                /**
                 *  [0,0,0] [1,0,0] [2,0,0]
                 *  [0,1,0] [1,1,0] [2,1,0]
                 *  [0,2,0] [1,2,0] [2,2,0]
                 */
                //创建对应物体的3d坐标
                vector<cv::Point3f> objPoints;
                calcObjPoints(patternSize, squareSize, objPoints,CHEESEVIDEO);

                imagesPoints.push_back(corners);
                objectPoints.push_back(objPoints);

                printf("保存照片及参数 %d/%d\n", (int) imagesPoints.size(), num_boards);
                if ((int) imagesPoints.size() == num_boards) {
                    break;
                }
            } else{
                cout << "未发现角点" << endl;
            }
        }
    }
    cout << "照片即角点参数已采集完毕，开始执行标定" << endl;
    // 相机内参
    cv::Mat cameraMatrix;
    //畸变参数 distortion coefficients
    cv::Mat distCoeffs;
    //每张图片的旋转矩阵
    vector<cv::Mat> recvs;
    //每张图片的平移矩阵
    vector<cv::Mat> tvecs;

    //3.执行相机标定操作
    double rms = cv::calibrateCamera(objectPoints, imagesPoints, imageSize, cameraMatrix, distCoeffs, recvs, tvecs);

    cout << rms << endl;
    //4.打印并保存标定结果到文件
    cout << "cameraMatrix:\n" << cameraMatrix << endl;
    cout << "distCoeffs:\n" << distCoeffs << endl;
    save2yml("./ChessBoardVideo.yml",imageSize, patternSize, squareSize, num_boards,cameraMatrix, distCoeffs);

//    while (cv::waitKey(30) != 27){
//
//    }
    cv::destroyAllWindows();

    return 0;
}

//圆网格视频的标定
int circleVideo(int board_width, int board_height, float squareSize, int num_boards, float scale_factor, bool f){
    //保存多张图片对象列表
    vector<vector<cv::Point3f>> objectPoints;
    //保存多张图片角点列表
    vector<vector<cv::Point2f>> imagesPoints;
    //棋盘的尺寸（宽：6，高：9）
    const cv::Size patternSize(board_width, board_height);


    //图片的像素尺寸
    cv::Size imageSize;

    //1.打开摄像头
    cv::VideoCapture capture(0);
    //判断相机是否打开
    if (!capture.isOpened()) {
        cerr << "相机无法打开，检查后重试" << endl;
        return -1;
    }

    while(true){
        cv::Mat image0,image;
        bool rst = capture.read(image0);
        if(!rst){
            cout << "未读到图片" << endl;
            continue;
        }
        image0.copyTo(image);

        //水平翻转图片
        if (f) {
            cv::flip(image, image, 1);

        }

        //执行缩放
        cv::resize(image, image, cv::Size(), scale_factor, scale_factor, cv::INTER_LINEAR);

        imageSize = image.size();

        //2.遍历查找每个照片的角点
        vector<cv::Point2f> corners;
//        cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1);
        bool patternFound = findCorners(image, patternSize, corners,CIRCLEVIDEO);
        int action = cv::waitKey(30) ;
        if (action == 27 || action == 'q') {
            cout <<  "标定程序已经终止" << endl;
            return -1;
        } else if (action == 32) {

            if (patternFound) {
                // 4. 创建对应的物体3d坐标
                /**
                 *  [0,0,0] [1,0,0] [2,0,0]
                 *  [0,1,0] [1,1,0] [2,1,0]
                 *  [0,2,0] [1,2,0] [2,2,0]
                 */
                //创建对应物体的3d坐标
                vector<cv::Point3f> objPoints;
                calcObjPoints(patternSize, squareSize, objPoints,CIRCLEVIDEO);

                imagesPoints.push_back(corners);
                objectPoints.push_back(objPoints);

                printf("保存照片及参数 %d/%d\n", (int) imagesPoints.size(), num_boards);
                if ((int) imagesPoints.size() == num_boards) {
                    break;
                }
            } else{
                cout << "未发现角点" << endl;
            }
        }
    }
    cout << "照片即角点参数已采集完毕，开始执行标定" << endl;
    // 相机内参
    cv::Mat cameraMatrix;
    //畸变参数 distortion coefficients
    cv::Mat distCoeffs;
    //每张图片的旋转矩阵
    vector<cv::Mat> recvs;
    //每张图片的平移矩阵
    vector<cv::Mat> tvecs;

    //3.执行相机标定操作
    cv::calibrateCamera(objectPoints, imagesPoints, imageSize, cameraMatrix, distCoeffs, recvs, tvecs);

    //4.打印并保存标定结果到文件
    cout << "cameraMatrix:\n" << cameraMatrix << endl;
    cout << "distCoeffs:\n" << distCoeffs << endl;
    save2yml("./circleVideo.yml",imageSize,patternSize,squareSize,num_boards,cameraMatrix, distCoeffs);

//    while (cv::waitKey(30) != 27){
//
//    }
    cv::destroyAllWindows();

    return 0;
}

// 进行非对称圆网格的视频标定
int acircleVideo(int board_width, int board_height, float square_size, int numboards, float scalefactor, bool f){


    //保存多张图片对象列表
    vector<vector<cv::Point3f>> objectPoints;
    //保存多张图片角点列表
    vector<vector<cv::Point2f>> imagesPoints;


    const cv::Size patternSize(board_width, board_height);



    //棋盘的尺寸（宽：6，高：9）
    //黑方格的大小（20mm）
    const int squareSize = square_size;
    //缩放因子
    double scale_factor = scalefactor;
    //图片的像素尺寸
    cv::Size imageSize;
    //采集15张图片
    int num_boards = numboards;
    //1.打开摄像头
    cv::VideoCapture capture(0);
    //判断相机是否打开
    if (!capture.isOpened()) {
        cerr << "相机无法打开，检查后重试" << endl;
        return -1;
    }

    while (true) {
        cv::Mat image0, image;
        bool rst = capture.read(image0);
        if (!rst) {
            cout << "未读到图片" << endl;
            continue;
        }
        image0.copyTo(image);

        if (f) {
            //水平翻转图片
            cv::flip(image, image, 1);
        }

        if (scale_factor != 1.0) {
            //执行缩放
            cv::resize(image, image, cv::Size(), scale_factor, scale_factor, cv::INTER_LINEAR);
        }

        imageSize = image.size();

        //2.遍历查找每个照片的角点
        vector<cv::Point2f> corners;
//        cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1);
        bool patternFound = findCorners(image, patternSize, corners,ACIRCLEVIDEO);
        int action = cv::waitKey(30);
        if (action == 27 || action == 'q') {
            cout << "标定程序已经终止" << endl;
            return -1;
        } else if (action == 32) {

            if (patternFound) {
                // 4. 创建对应的物体3d坐标
                /**
                 *  [0,0,0] [1,0,0] [2,0,0]
                 *  [0,1,0] [1,1,0] [2,1,0]
                 *  [0,2,0] [1,2,0] [2,2,0]
                 */
                //创建对应物体的3d坐标
                vector<cv::Point3f> objPoints;
                calcObjPoints(patternSize, squareSize, objPoints,ACIRCLEVIDEO);

                imagesPoints.push_back(corners);
                objectPoints.push_back(objPoints);

                printf("保存照片及参数 %d/%d\n", (int) imagesPoints.size(), num_boards);
                //闪屏
//                cv::bitwise_not(image, image, cv::noArray());
                cout << "11" << endl;

                stringstream ss;
                ss << "./camera_" << (int)imagesPoints.size() << ".jpg";
                cout << ss.str() << endl;
                cv::imwrite(ss.str(), image, vector<int>{CV_IMWRITE_JPEG_QUALITY, 100});
                if ((int) imagesPoints.size() == num_boards) {
                    break;
                }
            } else {
                cout << "未发现角点" << endl;
            }
        }
    }
    cout << "照片即角点参数已采集完毕，开始执行标定" << endl;
    // 相机内参
    cv::Mat cameraMatrix;
    //畸变参数 distortion coefficients
    cv::Mat distCoeffs;
    //每张图片的旋转矩阵
    vector<cv::Mat> recvs;
    //每张图片的平移矩阵
    vector<cv::Mat> tvecs;

    //3.执行相机标定操作
    double rms = cv::calibrateCamera(objectPoints, imagesPoints, imageSize, cameraMatrix, distCoeffs, recvs, tvecs);
    cout << "rms == " << rms << endl;
    //检查标定结果误差
    bool ok = cv::checkRange(cameraMatrix) && cv::checkRange(distCoeffs);

    if (ok) {
        printf("RMS error report by calibrateCamera: %f\n", rms);
        //4.打印并保存标定结果到文件
        cout << "cameraMatrix:\n" << cameraMatrix << endl;
        cout << "distCoeffs:\n" << distCoeffs << endl;
        save2yml("./AcircleVideo.yml",imageSize, patternSize, squareSize, num_boards,
                    cameraMatrix, distCoeffs);
        cout << "标定成功，文件已经保存";
    } else {
        cout << "标定失败" << endl;
    }

//    while (cv::waitKey(30) != 27){
//
//    }
    cv::destroyAllWindows();

    return 0;

}
