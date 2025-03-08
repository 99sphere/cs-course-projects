#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "read_map.h"
#include "run_qr.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

using namespace std;
using namespace cv;

extern pthread_mutex_t qr_mutex;

bool isNumber(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

void* run_qr(void* arg){
    qr_thread_data_t* data = (qr_thread_data_t*) arg;
    int sock = data->sock;
    int* cur_x_ptr = data->cur_x_ptr;
    int* cur_y_ptr = data->cur_y_ptr;
    
    VideoCapture cap(0);
    if (!cap.isOpened()) {
	    std::cerr << "Error: Unable to open the camera" << std::endl;
    }
    
    cv::QRCodeDetector detector;
    cv::Mat frame, gray;    
    int set_bomb;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Unable to capture frame" << std::endl;
            break;
        }
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        vector<Point> points;

        String info;

        // If QR detcted, decode and update cur_x, cur_y value. 
        // And send to server (cur_x, cur_y, action) info. 
        if(detector.detect(gray, points)){
            if (contourArea(points) > 0.0){
                info = detector.decode(gray, points); 
                if (isNumber(info)){
                    ClientAction action;
                    int xy=stoi(info);
                    int x = xy / 10;
                    int y = xy % 10;

                    // Synchronize with mutex to secure the shared resource.
                    pthread_mutex_lock(&qr_mutex);
                    *cur_x_ptr = x;
                    *cur_y_ptr = y;
                    pthread_mutex_unlock(&qr_mutex);
    
                    action.row = x;
                    action.col = y;
                    if ((x==1 && y==1) || (x==1 && y==3) || (x==3 && y==1) || (x==3 && y==3)){
                        set_bomb = 1;
                    }
                    else{
                        set_bomb = 0;
                    }
                    action.action = set_bomb;
                    send(sock, &action, sizeof(ClientAction), 0);
                    printf("[QR Thread] x: %d, y: %d\n", x, y);
                }
            }
            else{
                printf("Contour Area Error Occured\n");
            }
        }

    }
    cap.release();
    return NULL;
}
