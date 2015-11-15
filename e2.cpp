#include <stdio.h>
#include <stdlib.h>
#include "iostream"
#include "C:\Users\jeffrey\Desktop\school\333\New folder\rt.h"
//#include "..\rt.h"

using namespace std;

CRendezvous r_init1("Init_Rend",4);
CRendezvous r1("Rend",4);
CRendezvous r_term("Termin_Rend",4);

struct 	mydatapooldata 	{	// start of structure template
		int door_status;
		int floor ;			// floor corresponding to lifts current position
		int direction ;		// direction of travel of lift
		int onNOMOMOMOM;
		int floors[10] ;	// an array representing the floors and whether requests are set 
} ;		

int main( int argc, char *argv[] ) {

	// Waiting for init rendezvous
	cout << "Waiting for init data rendezvous" << endl;
	r_init1.Wait();

	CSemaphore ps2("Prod2",1,1);	// e2 datapool semaphore producer
	CSemaphore cs2("Cons2",1,1);	// e2 datapool semaphore consumer

	printf("e2 attempting to create/use the datapool.....\n") ;
	CDataPool 		dp("Elevator2", sizeof(struct mydatapooldata)) ;

	struct mydatapooldata 	 *MyDataPool = (struct mydatapooldata *)(dp.LinkDataPool()) ;

	//cs2.Wait();
	ps2.Wait();
	printf("e2 linked to elevator2 dp at address %p.....\n", MyDataPool) ;
	printf("e2 Writing value '3' to floor variable.....\n") ;
	MyDataPool->floor = 3 ;
	printf("e2 Writing value '0' to direction variable.....\n") ;
	MyDataPool->direction = 0 ;		// store 1 into the variable 'direction' in the datapool
	printf("e2 Writing value '8888' to e2 onNOMOMOMOM.....\n") ;
	MyDataPool->onNOMOMOMOM = 8888;
	printf("e2 Writing value '0 1 2 3 4 5 6 7 8 9' to floors array.....\n") ;
	for(int i = 0; i < 10; i ++) {
		MyDataPool->floors[ i ] = i ;	
	}
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