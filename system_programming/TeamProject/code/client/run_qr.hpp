// Struct for QR thread
typedef struct {
    int sock;
    int* cur_x_ptr;
    int* cur_y_ptr;
    int* set_bomb_ptr;
} qr_thread_data_t;

void* run_qr(void* arg);