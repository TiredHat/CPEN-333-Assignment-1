#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "iostream"
#include "C:\Users\jeffrey\Desktop\school\333\New folder\rt.h"
//#include "..\rt.h"

using namespace std;

CRendezvous r_init1("Init_Rend",4);
CRendezvous r1("Rend",4);
CRendezvous r_term("Termin_Rend",4);

struct 	mydatapooldata {	// start of structure template
	int door_status = 0;	// 0 = closed, 1 = open
	int floor = 0;			// floor corresponding to lifts current position
	int direction = 0;		// direction of travel of lift, 1=up, 0=down
	int onNOMOMOMOM;
	int general_status = 1;	// 0 = out of service, 1 equals ub service
	int floors[10];	// an array representing the floors and whether requests are set 
};


struct elevpip {
	int go;
	int direction;
	int status;
};
struct elevpip elevator1_pipe;

CSemaphore ps1("Prod1", 0);	// e1 datapool semaphore producer
CSemaphore cs1("Cons1", 1);	// e1 datapool semaphore consumer

CSemaphore ps2("Prod2", 0);
CSemaphore cs2("Cons2", 1);

//printf("e1 attempting to create/use the datapool.....\n");
CDataPool 		dp("Elevator1", sizeof(struct mydatapooldata));
CPipe	Elevator_Pipe("elevator1_PIPE", 1024);

struct mydatapooldata 	 *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool());

int main( int argc, char *argv[] ) {

	// Waiting for init rendezvous
	cout << "Waiting for init data rendezvous" << endl;
	r_init1.Wait();

	//cs1.Wait();
		cs1.Wait();
		cs2.Wait();
		printf("e1 linked to elevator1 dp at address %p.....\n", MyDataPool);
		printf("e1 Writing value '1' to floor variable.....\n");
		MyDataPool->floor = 1;
		printf("e1 Writing value '1' to direction variable.....\n");
		MyDataPool->direction = 1;		// store 1 into the variable 'direction' in the datapool
		printf("Parent Writing value '9999' to e1 onNOMOMOMOM.....\n");
		MyDataPool->onNOMOMOMOM = 9999;
		printf("Parent Writing value '0 1 2 3 4 5 6 7 8 9' to floors array.....\n");
		for (int i = 0; i < 10; i++) {
			MyDataPool->floors[i] = i;
		}
		ps1.Signal();
		ps2.Signal();
	//// Waiting for rendezvous
	//cout << "Waiting for data rendezvous" << endl;
	//r1.Wait();

	cout << endl;

	// Waiting for terminate rendezvous
	cout << "Waiting for terminate data rendezvous" << endl;
	r_term.Wait();

//	system( "Pause" );
	return 0;
}