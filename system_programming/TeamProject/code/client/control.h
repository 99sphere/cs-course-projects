#define PIN_L1 2 
#define PIN_L2 3
#define PIN_R1 0
#define PIN_R2 7
#define DEVICE_ADDR 0x16

int write_i2c_block_data(int reg, unsigned char* data, int length);
int ctrl_car(int l_dir, int l_speed, int r_dir, int r_speed);
int control_servo(int id, int angle);
int car_run(int speed1, int speed2);
int car_stop();
int turn_left();
int turn_right();
int go_straight();