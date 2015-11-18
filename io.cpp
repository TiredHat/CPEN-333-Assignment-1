#include <stdio.h>
#include <stdlib.h>
#include "iostream"
#include <conio.h>
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


struct mypipeline {
	int request;
	int priority;
};

struct mypipeline mypip1 = { -1, -1 };


CSemaphore ps1("Prod1", 0);	// e1 datapool semaphore producer
CSemaphore ps3("Prod3", 0);	// e2 datapool semaphore producer
							//CSemaphore ps3("Prod3",0,1);	// IO pipeline semaphore producer

CSemaphore cs1("Cons1", 1);	// e1 datapool semaphore consumer
CSemaphore cs3("Cons3", 1);	// e2 datapool semaphore consumer
							//CSemaphore cs3("Cons3",1,1);	// IO pipeline semaphore consumer

							//printf("IO Process Creating the Pipeline.....\n");
CPipe	pipe("MyPipe", 1024);							// Create a pipe 'p1' with the name "MyPipe"
														//cs3.Wait();
														//printf("Writing struct to pipe from IO to dispatcher with values mystruct.request = %d, mystruct.priority = %d.\n", mypip1.request, mypip1.priority);

														//pipe.Write(&mypip1, sizeof(mypip1));			// write the structure to the pipeline
														//ps3.Signal();

														// IO initializing e1 and e2 datapools
														//printf("IO attempting to create/use the datapool.....\n");
CDataPool 		dp1("Elevator1", sizeof(struct mydatapooldata));
struct mydatapooldata 	 *MyDataPool1 = (struct mydatapooldata *)(dp1.LinkDataPool());
//printf("IO linked to e1 datapool at address %p.....\n", MyDataPool1);

CDataPool 		dp2("Elevator2", sizeof(struct mydatapooldata));
struct mydatapooldata 	 *MyDataPool2 = (struct mydatapooldata *)(dp2.LinkDataPool());
//printf("IO linked to e2 datapool at address %p.....\n", MyDataPool2);

CSemaphore sema_io_console("sema_io_console", 1);

#define ELEV1_DISPLAYX 1
#define ELEV2_DISPLAYX 45

UINT __stdcall elevator_console(void *args)
{
	char str[3];
	int val[2]; // save str as int
	int priority = 0;

	Sleep(2000);

	while (1) {

		// gets request entry
		sema_io_console.Wait();
		MOVE_CURSOR(0, 2);
		printf("Please enter command:\n");
		sema_io_console.Signal();
		str[0] = _getch();
		// Clears out the enter message
		sema_io_console.Wait();
		MOVE_CURSOR(0, 3);
		printf("\t\t\t\t\t");
		MOVE_CURSOR(0, 3);
		printf("Servicing: %c", str[0]);
		sema_io_console.Signal();
		str[1] = _getch();
		str[2] = '\0';
		sema_io_console.Wait();
		MOVE_CURSOR(0, 3);
		printf("\t\t\t\t\t");
		MOVE_CURSOR(0, 3);
		printf("Entered: %s\n", str);


		// convert input to integers if valid
		if (atoi(&str[1]) && str[1] != '0') {
			val[1] = atoi(&str[1]);
		}
		else if (str[1] == '0') {
			val[1] = 0;
		}

		MOVE_CURSOR(0, 4);
		printf("\t\t\t\t\t\t\t\t\t\t");
		MOVE_CURSOR(0, 4);

		if (!strcmp(str, "ee")) { // exit case
			printf("Shutting down elevators\n");

			// Write request and priority type to pipe
			mypip1 = { 99, 99 }; // exit code
			pipe.Write(&mypip1, sizeof(mypip1));

			// redezvous type here before the return 0
			return 0;
		}
		else if ((str[0] == 'u' && (0 <= val[1] && val[1] < 9)) ||
			(str[0] == 'd' && (0 < val[1] && val[1] < 10))) { // calling the elevator case
			printf("Please wait, going %c from floor %c\n", str[0], str[1]);

			// Write request and priority type to pipe
			// "3-" is an up request and "4-" is a down request
			if (str[0] == 'u') {
				mypip1 = { 30 + val[1],priority }; // exit code
			}
			else if (str[0] == 'd') {
				mypip1 = { 40 + val[1],priority }; // exit code
			}
			pipe.Write(&mypip1, sizeof(mypip1));

			priority++;

		}
		else if ((str[0] == '1' || str[0] == '2') && (0 <= val[1] && val[1] < 10)) { // inside the elevator case
			printf("Closing door, moving elevator %c to floor %c \n", str[0], str[1]);
			if (str[0] == '1') {
				mypip1 = { 10 + val[1],priority }; // exit code
			}
			else if (str[0] == '2') {
				mypip1 = { 20 + val[1],priority }; // exit code
			}
			pipe.Write(&mypip1, sizeof(mypip1));

			priority++;
		}
		else if ((str[0] == '+' || str[0] == '-') && (val[1] == 1 || val[1] == 2)) // servicing elevators and faults
		{
			if (!strcmp(str, "+1"))
			{
				printf("elevator 1 is in service \n");
			}
			else if (!strcmp(str, "-1"))
			{
				printf("elevator 1 is out of service \n");
			}
			else if (!strcmp(str, "+2"))
			{
				printf("elevator 2 is in service \n");
			}
			else if (!strcmp(str, "-2"))
			{
				printf("elevator 2 is out of service \n");
			}

			// Write request and priority type to pipe
			// "5-" is an up request and "6-" is a down request
			if (str[0] == '+') {
				mypip1 = { 50 + val[1],0 }; // exit code
			}
			else if (str[0] == '-') {
				mypip1 = { 60 + val[1],0 }; // exit code
			}
			pipe.Write(&mypip1, sizeof(mypip1));

		}
		else {
			printf("INVALID ENTRY!\n");
		}

		sema_io_console.Signal();

		val[0] = -1; //if val[0] isn't what we want then set it to -1
		val[1] = -1; //if val[1] isn't what we want then set it to -1

	}
}

UINT __stdcall elevator1(void *args) {
	while (1) {
		if (ps1.Read() > 0) {
			ps1.Wait();
			sema_io_console.Wait();
			MOVE_CURSOR(ELEV1_DISPLAYX, 10);
			printf("Elevator 1\n");
			MOVE_CURSOR(ELEV1_DISPLAYX, 12);
			printf("\t\t\t\t\t");
			MOVE_CURSOR(ELEV1_DISPLAYX, 12);
			if (MyDataPool1->general_status == 0) {
				printf("- Offline\n");
			}
			else {
				printf("- Online\n");
			}

			// clears elevator
			for (int i = 0; i < 10;i++) {
				MOVE_CURSOR(ELEV1_DISPLAYX, 10 + (13 - i));
				printf("\t\t\t\t\t");
			}

			// clears elevator
			for (int i = 0; i < 10;i++) {
				MOVE_CURSOR(ELEV1_DISPLAYX, 10 + (13 - i));
				printf("__ __ F%d", i);
			}

			// displays elevator location
			MOVE_CURSOR(ELEV1_DISPLAYX, 10 + (13 - MyDataPool1->floor));
			printf("[|_|] F%d", MyDataPool1->floor);

			sema_io_console.Signal();
			cs1.Signal();
		}
	}
}

UINT __stdcall elevator2(void *args) {
	while (1) {
		if (ps3.Read() > 0) {
			ps3.Wait();
			sema_io_console.Wait();
			MOVE_CURSOR(ELEV2_DISPLAYX, 10);
			printf("Elevator 2\n");
			MOVE_CURSOR(ELEV2_DISPLAYX, 12);
			printf("\t\t\t\t\t");
			MOVE_CURSOR(ELEV2_DISPLAYX, 12);
			if (MyDataPool2->general_status == 0) {
				printf("- Offline\n");
			}
			else {
				printf("- Online\n");
			}

			// clears elevator
			for (int i = 0; i < 10;i++) {
				MOVE_CURSOR(ELEV2_DISPLAYX, 10 + (13 - i));
				printf("\t\t\t\t\t");
			}

			// clears elevator
			for (int i = 0; i < 10;i++) {
				MOVE_CURSOR(ELEV2_DISPLAYX, 10 + (13 - i));
				printf("__ __ F%d", i);
			}

			// displays elevator location
			MOVE_CURSOR(ELEV2_DISPLAYX, 10 + (13 - MyDataPool2->floor));
			printf("[|_|] F%d", MyDataPool2->floor);

			sema_io_console.Signal();
			cs3.Signal();
		}
	}
}


int main(int argc, char *argv[]) {

	// Waiting for init rendezvous
	cout << "Waiting for init data rendezvous" << endl;
	r_init1.Wait();
	/*
	CSemaphore ps1("Prod1",0);	// e1 datapool semaphore producer
	CSemaphore ps3("Prod3",0);	// e2 datapool semaphore producer
	//CSemaphore ps3("Prod3",0,1);	// IO pipeline semaphore producer

	CSemaphore cs1("Cons1",1);	// e1 datapool semaphore consumer
	CSemaphore cs3("Cons3",1);	// e2 datapool semaphore consumer
	//CSemaphore cs3("Cons3",1,1);	// IO pipeline semaphore consumer

	printf("IO Process Creating the Pipeline.....\n") ;
	CPipe	pipe("MyPipe", 1024) ;							// Create a pipe 'p1' with the name "MyPipe"

	//cs3.Wait();
	printf("Writing struct to pipe from IO to dispatcher with values mystruct.request = %d, mystruct.priority = %d.\n", mypip1.request, mypip1.priority);
	pipe.Write(&mypip1, sizeof(mypip1)) ;			// write the structure to the pipeline
	//ps3.Signal();

	// IO initializing e1 and e2 datapools
	printf("IO attempting to create/use the datapool.....\n") ;
	CDataPool 		dp1("Elevator1", sizeof(struct mydatapooldata)) ;
	struct mydatapooldata 	 *MyDataPool1 = (struct mydatapooldata *)(dp1.LinkDataPool()) ;
	printf("IO linked to e1 datapool at address %p.....\n", MyDataPool1) ;

	CDataPool 		dp2("Elevator2", sizeof(struct mydatapooldata)) ;
	struct mydatapooldata 	 *MyDataPool2 = (struct mydatapooldata *)(dp2.LinkDataPool()) ;
	printf("IO linked to e2 datapool at address %p.....\n", MyDataPool2) ;
	*/
	//	Sleep(3000);

	//// Waiting for rendezvous
	//cout << "Waiting for data rendezvous" << endl;
	//r1.Wait();
	//detect for keyboard hit
	/*
	while (!_kbhit()) {
	if (ps1.Read() > 0) {
	ps1.Wait();
	// Print off datapool values for e1
	printf("\nIO Read e1 value for Floor = %d\n", MyDataPool1->floor);
	printf("IO Read e1 value for Direction = %d\n", MyDataPool1->direction);
	printf("My e1 onNOMOMOMOM  value is = %d\n", MyDataPool1->onNOMOMOMOM);
	printf("IO Read e1 values for floor array = ");
	for (int i = 0; i < 10; i++)
	printf("%d ", MyDataPool1->floors[i]);
	cout << endl;
	cs1.Signal();
	}
	if (ps3.Read() > 0) {
	ps3.Wait();
	// Print off datapool values for e2
	printf("\nIO Read e2 value for Floor = %d\n", MyDataPool2->floor);
	printf("IO Read e2 value for Direction = %d\n", MyDataPool2->direction);
	printf("My e2 onNOMOMOMOM  value is = %d\n", MyDataPool2->onNOMOMOMOM);
	printf("IO Read e2 values for floor array = ");
	for (int i = 0; i < 10; i++)
	printf("%d ", MyDataPool2->floors[i]);
	cs3.Signal();
	}
	}
	*/


	CThread c1(elevator_console, ACTIVE, NULL);
	CThread c2(elevator1, ACTIVE, NULL);
	CThread c3(elevator2, ACTIVE, NULL);
	c1.WaitForThread();
	c2.WaitForThread();
	c3.WaitForThread();
	cout << endl << endl;
	// Waiting for terminate rendezvous
	cout << "Waiting for terminate data rendezvous" << endl;
	r_term.Wait();

	//system( "Pause" );
	return 0;
}