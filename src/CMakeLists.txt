

add_executable(undistort undistort.cpp)
target_link_libraries(undistort ${OpenCV_LIBS})

add_executable(undistortMap undistortMap.cpp)
target_link_libraries(undistortMap ${OpenCV_LIBS})

add_executable(undistortMap1 undistortMap1.cpp)
target_link_libraries(undistortMap1 ${OpenCV_LIBS})

add_executable(camerCalibrate camerCalibrate.cpp board.cpp)
target_link_libraries(camerCalibrate ${OpenCV_LIBS})