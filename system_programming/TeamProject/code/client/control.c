#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include "control.h"

extern int fd;

int write_i2c_block_data(int reg, unsigned char* data, int length) {
    unsigned char buf[length + 1];
    buf[0] = reg;
    for (int i = 0; i < length; i++) {
        buf[i + 1] = data[i];
    }
    if (write(fd, buf, length + 1) != length + 1) {
        printf("Failed to write to the i2c bus\n");
        return -1;
    }
    return 0;
}

int ctrl_car(int l_dir, int l_speed, int r_dir, int r_speed) {
    int reg = 0x01;
    unsigned char data[4] = {l_dir, l_speed, r_dir, r_speed};
    return write_i2c_block_data(reg, data, 4);
}

int control_servo(int id, int angle) {
    int reg = 0x03;
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    unsigned char data[2] = {id, (unsigned char)angle};
    return write_i2c_block_data(reg, data, 2);
}

int car_run(int speed1, int speed2) {
    int dir1 = (speed1 < 0) ? 0 : 1;
    int dir2 = (speed2 < 0) ? 0 : 1;

    return ctrl_car(dir1, abs(speed1), dir2, abs(speed2));
}

int car_stop() {
    int reg = 0x02;
    unsigned char stop_data[1] = {0x00};
    return write_i2c_block_data(reg, stop_data, 1);
}

int turn_left() {
    printf("Turn left");
    ctrl_car(0, 30, 1, 50);
    delay(300);
    while(1){
    	int L1 = digitalRead(PIN_L1);
	int L2 = digitalRead(PIN_L2);
	int R1 = digitalRead(PIN_R1);
	int R2 = digitalRead(PIN_R2);

	ctrl_car(0, 0, 1, 70);
	delay(100);

	if (L2==LOW && R1 ==LOW){
	    ctrl_car(0,0,0,0);
	    delay(50);
	    return 1;
	}
    }
    
    return 0;
}

int turn_right() {
    printf("Turn Right\n");
    ctrl_car(1, 50, 0, 30);
    delay(300);
    while(1){
        int L1 = digitalRead(PIN_L1);
        int L2 = digitalRead(PIN_L2);
        int R1 = digitalRead(PIN_R1);
        int R2 = digitalRead(PIN_R2);
            
        ctrl_car(1, 70, 0, 0);
        delay(100);

        if (L2==LOW && R1==LOW){
            ctrl_car(0,0,0,0);
            delay(50);
            return 1;
        }
    }
    
    return 0;
}

int go_straight() {
    printf("go straight\n");
    
    while(1){
        int L1 = digitalRead(PIN_L1);
        int L2 = digitalRead(PIN_L2);
        int R1 = digitalRead(PIN_R1);
        int R2 = digitalRead(PIN_R2);
        
        if ((L1 == LOW || L2 == LOW) && R2 == LOW) {
            car_run(35, -15) ;
            delay(200);
        }
        else if (L1 == LOW && (R1 == LOW || R2 == LOW)) {
            car_run(-15, 35);
            delay(200);
        }
        else if (L1 == LOW) {
            car_run(-35, 35);
            delay(50);
        }
        else if (R2 == LOW) {
            car_run(35, -35);
            delay(50);
        }
        else if (L2 == LOW && R1 == HIGH) {
            car_run(-30, 30);
            delay(20);
        }
        else if (L2 == HIGH && R1 == LOW) {
            car_run(30, -30);
            delay(20);          
        }
        else if (L2 == LOW && R1 == LOW) {
            car_run(70, 70);
	    delay(20);
        }
        
        if (L2 == LOW && R1 == LOW && (L1 ==LOW || R2 == LOW)){
            ctrl_car(0, 0, 0, 0);
	    delay(20);
            return 1 ;
        }

        if (L1 == LOW && L2 == LOW && R1 == LOW && R2 == LOW){
            ctrl_car(0, 0, 0, 0);
	    delay(20);
            return 1;
        }
        
        if (L1 == HIGH && L2 == HIGH && R1 == HIGH && R2 == HIGH){
	    car_run(-30, -30) ;
            delay(100) ;
        }
    }
}
