# 2024 Spring System Programming

# Team
- 201811118 이 구 
- 202211061 남민영
- 202211211 최한슬
- 202211222 황지영

# Demo
![demo](https://github.com/99sphere/2024-Spring-System-Programming/assets/59161083/beb5565a-b684-4464-973a-12047003a68c)


# Compile & Execution
## Client
```
git clone {repo}
cd {repo_name}/client
g++ -Wall main.c read_map.c control.c run_qr.cpp -o runfile -lpthread -lwiringPi $(pkg-config opencv4 --libs --cflags)
./runfile <Server IP> <Port Number> <Local Direction>
```

## Server
> Server code comes from "https://github.com/CELL-DGIST/2024_SystemProgramming_Server.git"

```
git clone {repo}
cd {repo_name}/server
make
./Server <Port Number>
```

# Function
- main.c      
  Connect to server, setup wiring Pi, global path planning with greedy algorithm and car control. 
  
- run_qr.cpp     
  Running thread that detect and decode QR Code and send decoded info to server.

- read_map.c     
  Running thread that recieve map info from server.
    
- control.c     
  Consist with go_straight, turn_left, turn_right function.

# Path Planning
Since "control.c" only provides the functions of moving forward one step and rotating left or right in place, it maintains both the global direction based on the map and the local direction based on the current vehicle's orientation. 0, 1, 2, 3 represent up, right, down, and left, respectively. Based on the current vehicle's position, it checks the information of the next node in the up, down, left, and right directions. If an item does not exist, it is considered an item with a score of 0, and the vehicle moves to the node with the highest score among the surrounding nodes. If all directions except down (up, left, and right) are surrounded by traps, it rotates 180 degrees to escape.

# Set Bomb Strategy
The locations for placing the bombs were set manually. Bombs were placed at (1, 1), (1, 3), (3, 1), and (3, 3), where frequent movement is expected.

