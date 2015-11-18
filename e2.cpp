#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "iostream"
#include "C:\Users\jeffrey\Desktop\school\333\New folder\rt.h"
//#include "..\rt.h"

using namespace std;

CRendezvous r_init1("Init_Rend", 4);
CRendezvous r1("Rend", 4);
CRendezvous r_term("Termin_Rend", 4);

struct 	mydatapooldata {	// start of structure template
	int door_status = 0;	// 0 = closed, 1 = open
	int floor = 0;			// floor corresponding to lifts current position
	int direction = 0;		// direction of travel of lift, 1=up, 0=down
	int onNOMOMOMOM;
	int general_status = 1;	// 0 = out of service, 1 equals ub service
	int floors[10];	// an array representing the floors and whether requests are set 
};

// go = 1 is go, 0 is stop
// direction = 1 is up, 0 is down
// status = 0 is OOS, status = 1 is good, status = -1 is exit condition
struct elevpip {
	int go;
	int direction;
	int status;
};

struct elevpip elevator2_pipe;

CSemaphore ps3("Prod3", 0);	// e2 datapool semaphore producer
CSemaphore cs3("Cons3", 1);	// e2 datapool semaphore consumer

CSemaphore ps4("Prod4", 0);
CSemaphore cs4("Cons4", 1);

CPipe   Elevator2_Pipe("elevator2_PIPE", 1024);
//printf("e2 attempting to create/use the datapool.....\n");
CDataPool 		dp("Elevator2", sizeof(struct mydatapooldata));

struct mydatapooldata 	 *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool());


int main(int argc, char *argv[]) {

	// Waiting for init rendezvous
	cout << "Waiting for init data rendezvous" << endl;
	r_init1.Wait();

	while (1) {
		Elevator2_Pipe.Read(&elevator2_pipe, sizeof(elevator2_pipe));
		printf("Go: %d and Direction: %d\n", elevator2_pipe.go, elevator2_pipe.direction);


		//cs2.Wait();
		cs3.Wait();
		cs4.Wait();
		printf("adsgadsgag\n");

		if (elevator2_pipe.status == 1) {	// in service
			MyDataPool->general_status = 1;
			if (elevator2_pipe.go == 1) {
				printf("Elevator 2 moving\n");
				MyDataPool->door_status = 0;	// redundant safetly
				Sleep(1000);					// travel time

				if (elevator2_pipe.direction == 0) {
					MyDataPool->floor--;
					MyDataPool->direction = 0;
				}
				else {
					MyDataPool->floor++;
					MyDataPool->direction = 1;
				}

			}
			else {
				ps3.Signal();
				ps4.Signal();
				cs3.Wait();
				cs4.Wait();
				printf("Elevator 2 opening and closing door for person\n");
				MyDataPool->door_status = 1;
				Sleep(3000);
				MyDataPool->door_status = 0;
			}
		}
		else if (elevator2_pipe.status == 0) { // out of service
			printf("Elevator 2 OOS\n");
			MyDataPool->general_status = 0;
		}
		else if (elevator2_pipe.status == -1) {
			printf("Shutting down elevator 2\n");

			ps3.Signal();
			ps4.Signal();

			while (MyDataPool->floor > 0) {
				cs3.Wait();
				cs4.Wait();
				MyDataPool->floor--;
				Sleep(1000);
				ps3.Signal();
				ps4.Signal();
			}


			MyDataPool->general_status = 0;
			MyDataPool->door_status = 0;	// redundant safetly
			Sleep(4333);
			return 0;
		}

		ps3.Signal();
		ps4.Signal();
		//// Waiting for rendezvous
		//cout << "Waiting for data rendezvous" << endl;
		//r1.Wait();
	}


	// Waiting for terminate rendezvous
	cout << "Waiting for terminate data rendezvous" << endl;
	r_term.Wait();

	//	system( "Pause" );
	return 0;
}