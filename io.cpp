#include <stdio.h>
#include <stdlib.h>
#include "iostream"
#include "..\rt.h"

using namespace std;

CRendezvous r_init1("Init_Rend",4);
CRendezvous r1("Rend",4);
CRendezvous r_term("Termin_Rend",4);

struct 	mydatapooldata 	{	// start of structure template
		int floor ;			// floor corresponding to lifts current position
		int direction ;		// direction of travel of lift
		int onNOMOMOMOM;
		int floors[10] ;	// an array representing the floors and whether requests are set 
} ;	

struct mypipeline {
		int request ;
		int priority ;
} ;

struct mypipeline mypip1 = { 5, 7};

int main( int argc, char *argv[] ) {

	// Waiting for init rendezvous
	cout << "Waiting for init data rendezvous" << endl;
	r_init1.Wait();

	CSemaphore ps1("Prod1",1,1);	// e1 datapool semaphore producer
	CSemaphore ps2("Prod2",1,1);	// e2 datapool semaphore producer
	CSemaphore ps3("Prod3",0,1);	// IO pipeline semaphore producer

	CSemaphore cs1("Cons1",1,1);	// e1 datapool semaphore consumer
	CSemaphore cs2("Cons2",1,1);	// e2 datapool semaphore consumer
	CSemaphore cs3("Cons3",1,1);	// IO pipeline semaphore consumer

	printf("IO Process Creating the Pipeline.....\n") ;
	CPipe	pipe("MyPipe", 1024) ;							// Create a pipe 'p1' with the name "MyPipe"

	cs3.Wait();
	printf("Writing struct to pipe from IO to dispatcher with values mystruct.request = %d, mystruct.priority = %d.\n", mypip1.request, mypip1.priority);
	pipe.Write(&mypip1, sizeof(mypip1)) ;			// write the structure to the pipeline
	ps3.Signal();

	// IO initializing e1 and e2 datapools
	printf("IO attempting to create/use the datapool.....\n") ;
	CDataPool 		dp1("Elevator1", sizeof(struct mydatapooldata)) ;
	struct mydatapooldata 	 *MyDataPool1 = (struct mydatapooldata *)(dp1.LinkDataPool()) ;
	printf("IO linked to e1 datapool at address %p.....\n", MyDataPool1) ;

	CDataPool 		dp2("Elevator2", sizeof(struct mydatapooldata)) ;
	struct mydatapooldata 	 *MyDataPool2 = (struct mydatapooldata *)(dp2.LinkDataPool()) ;
	printf("IO linked to e2 datapool at address %p.....\n", MyDataPool2) ;

//	Sleep(3000);

	//// Waiting for rendezvous
	//cout << "Waiting for data rendezvous" << endl;
	//r1.Wait();

	cs1.Wait();
	// Print off datapool values for e1
	printf("\nIO Read e1 value for Floor = %d\n", MyDataPool1->floor) ;
	printf("IO Read e1 value for Direction = %d\n", MyDataPool1->direction) ; 
	printf("My e1 onNOMOMOMOM  value is = %d\n", MyDataPool1->onNOMOMOMOM) ; 
	printf("IO Read e1 values for floor array = ") ;
	for(int i=0; i < 10; i ++)
		printf("%d ", MyDataPool1->floors[ i ]) ;
	cout << endl;
	cs1.Signal();

	cs2.Wait();
	// Print off datapool values for e2
	printf("\nIO Read e2 value for Floor = %d\n", MyDataPool2->floor) ;
	printf("IO Read e2 value for Direction = %d\n", MyDataPool2->direction) ; 
	printf("My e2 onNOMOMOMOM  value is = %d\n", MyDataPool2->onNOMOMOMOM) ; 
	printf("IO Read e2 values for floor array = ") ;
	for(int i=0; i < 10; i ++)
		printf("%d ", MyDataPool2->floors[ i ]) ;
	cs2.Signal();

	cout << endl<< endl;

	// Waiting for terminate rendezvous
	cout << "Waiting for terminate data rendezvous" << endl;
	r_term.Wait();

	//system( "Pause" );
	return 0;
}