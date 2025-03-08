#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "read_map.h"
#include <pthread.h>
#include <semaphore.h>

extern pthread_mutex_t map_mutex;

void printMap(DGIST dgist) {
    printf("Map:\n");
    for (int i = 0; i < MAP_ROW; i++) {
        for (int j = 0; j < MAP_COL; j++) {
            switch (dgist.map[i][j].item.status) {
                case nothing:
                    printf("- ");
                    break;
                case item:
                    printf("%d ", dgist.map[i][j].item.score);
                    break;
                case trap:
                    printf("T ");
                    break;
            }
        }
        printf("\n");
    }
}

void* read_map(void* arg) {
    map_thread_data_t* data = (map_thread_data_t*) arg;
    int sock = data->sock;
    DGIST* raw_map_ptr = data->raw_map_ptr;

    char buffer[sizeof(DGIST)] = {0};
    int valread;
    while(1){
        // Get map info from server
        if ((valread = read(sock, buffer, sizeof(DGIST))) == 0) {
            printf("Server disconnected\n");
        }
        // printf("get map info from server\n"); // TODO: erase

        // Synchronize with mutex to secure the shared resource.
        pthread_mutex_lock(&map_mutex);
        memcpy(raw_map_ptr, buffer, sizeof(DGIST));
        pthread_mutex_unlock(&map_mutex);
    }
    return NULL;
}
