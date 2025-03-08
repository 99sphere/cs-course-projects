#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "run_qr.hpp"
#include "read_map.h"
#include "control.h"

pthread_mutex_t map_mutex;
pthread_mutex_t qr_mutex;
pthread_mutex_t main_mutex;

int fd;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <Server IP> <port number> <cur dir>\n", argv[0]);
        return 1;
    }

    // init start direction
    int cur_dir = atoi(argv[3]);

    // if (!((cur_dir == 1) || (cur_dir == 3))){
    //     printf("Current direction must be 1 or 3.\n");
    //     return 2;
    // }

    int cur_x = -1;
    int cur_y = -1;
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

	const int PORT = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Init thread assets (QR)
    pthread_mutex_init(&qr_mutex, NULL);
    pthread_t thread_qr;
    qr_thread_data_t qr_thread_data;

    // Init data struct for qr thread
    qr_thread_data.sock = sock;
    qr_thread_data.cur_x_ptr = &cur_x;
    qr_thread_data.cur_y_ptr = &cur_y;

    // Create and run QR thread
    int qr_thread_ret;
    qr_thread_ret = pthread_create(&thread_qr, NULL, run_qr, (void*)&qr_thread_data);
    if (qr_thread_ret) {
        printf("Error: unable to create thread, %d\n", qr_thread_ret);
        exit(-1);
    }

    // Init thread assets (map)
    pthread_mutex_init(&map_mutex, NULL);
    pthread_t thread_map;
    map_thread_data_t map_thread_data;
    

    // Init data struct for map thread
    DGIST raw_map={0,};
    map_thread_data.sock = sock;
    map_thread_data.raw_map_ptr = &raw_map;

    // Create and run map thread
    int map_thread_ret;
    map_thread_ret = pthread_create(&thread_map, NULL, read_map, (void*)&map_thread_data);
    if (map_thread_ret) {
        printf("Error: unable to create thread, %d\n", map_thread_ret);
        exit(-1);
    }

    // Init wiringPi
    if (wiringPiSetup() == -1) {
        printf("WiringPi Setup Failure\n");
        return 1;
    }
    else printf("WiringPi Setup Sucessed\n");

    fd = wiringPiI2CSetup(DEVICE_ADDR);
    if (fd == -1) {
        printf("I2C Setup Failure\n");
        return 1;
    }
    else printf("I2C Setup Sucessed\n");

    pinMode(PIN_L1, INPUT);
    pinMode(PIN_L2, INPUT);
    pinMode(PIN_R1, INPUT);
    pinMode(PIN_R2, INPUT);


    // Init for main algorithm (Greedy)    
    int dir[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
    int ctrl_ret;
    ctrl_ret = go_straight();

    pthread_mutex_init(&main_mutex, NULL);

    while(1){
        int next_dir= -1;
        int score_min = -1;
        int next_x=0;
        int next_y=0;

        pthread_mutex_lock(&main_mutex);
        for(int d = 0;d<4;d++){
            int score;
            int diff = d - cur_dir;
            if (diff < 0){
                diff += 4;
            }
            if (diff != 2){
                int nx = cur_x+dir[d][0];
                int ny = cur_y+dir[d][1];
    
                if((ny >= 0 && nx >= 0) && (ny < 5 && nx < 5)){
                    if (raw_map.map[nx][ny].item.status==1 || raw_map.map[nx][ny].item.status==0){
                        if (raw_map.map[nx][ny].item.status==1){
                            score = raw_map.map[nx][ny].item.score;
                        }
                        else{
                            score = 0;
                        }
                        if (score > score_min){
                            next_dir = d;
                            next_x = nx;
                            next_y = ny;
                        }
                    }
                }
            }
        }
        pthread_mutex_unlock(&main_mutex);

        
        printf("next x: %d, next y: %d\n", next_x, next_y);
        if (next_dir==-1){ // Surrounded by trap
            printf("BACK\n");
            ctrl_ret = turn_left();
            delay(50);
            ctrl_ret = turn_left();
            delay(50);
            ctrl_ret = go_straight();
            cur_dir -= 2;
            if (cur_dir < 0){
                cur_dir += 4;
            }        
        }

        else{ // Go to next node
            int calc_rot = cur_dir - next_dir;
            if (calc_rot < 0){
                calc_rot += 4;
            }

            if (calc_rot == 0){
                printf("FRONT\n");
                ctrl_ret = go_straight();
            } 

            else if (calc_rot == 1){
                printf("LEFT\n");
                ctrl_ret = turn_left();
                delay(50);
                ctrl_ret = go_straight();
                cur_dir -= 1;
                if (cur_dir < 0){
                    cur_dir += 4;
                }
            }

            else if (calc_rot == 3){
                printf("RIGHT\n");
                ctrl_ret = turn_right();
                delay(50);
                ctrl_ret = go_straight();
                cur_dir += 1;
                if (cur_dir > 3){
                    cur_dir -= 4;
                }
            }
        }
    }

    // Wait until thread terminate
    pthread_join(thread_qr, NULL);
    pthread_join(thread_map, NULL);

    pthread_mutex_destroy(&qr_mutex);
    pthread_mutex_destroy(&map_mutex);
    pthread_mutex_destroy(&main_mutex);

    close(sock);
    return 0;
}
