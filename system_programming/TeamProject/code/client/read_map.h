#define MAX_CLIENTS 2
#define _MAP_ROW 4
#define _MAP_COL 4
#define MAP_ROW _MAP_ROW + 1
#define MAP_COL _MAP_COL + 1
#define MAP_SIZE = MAP_COL*MAP_ROW

const int MAX_SCORE = 4; // Item max score
const int SETTING_PERIOD = 20; //Boradcast & Item generation period
const int INITIAL_ITEM = 10; //Initial number of item
const int INITIAL_BOMB = 4; //The number of bomb for each user
const int SCORE_DEDUCTION = 2; //The amount of score deduction due to bomb

typedef struct{
    int socket;
    struct sockaddr_in address;
	int row;
	int col;
	int score;
	int bomb;
} client_info;

enum Status{
	nothing, //0
	item, //1
	trap //2
};

typedef struct{
	enum Status status;
	int score;
} Item;

typedef struct {
	int row;
	int col;
	Item item; 
} Node;

typedef struct{
	client_info players[MAX_CLIENTS];
	Node map[MAP_ROW][MAP_COL];
} DGIST;

enum Action{
	move, //0
	setBomb, //1
};

typedef struct{
	int row;
	int col;
	int action;
} ClientAction;

// Added
void* read_map(void* arg);
void printMap(DGIST dgist);

typedef struct {
    int sock;
    DGIST* raw_map_ptr;
} map_thread_data_t;