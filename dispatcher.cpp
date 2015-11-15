#include <stdio.h>
#include <stdlib.h>
#include "iostream"
#include "..\rt.h"

using namespace std;

CRendezvous r_init1("Init_Rend",4);
CRendezvous r1("Rend",4);
CRendezvous r_term("Termin_Rend",4);
  
struct 	    mydatapooldata 	{		// start of structure template
		int floor ;				// floor corresponding to lifts current position
		int direction ;			// direction of travel of lift
		int onNOMOMOMOM;
		int floors[10] ;			// an array representing the floors and whether requests are set 
} ;							// end of structure template


struct mypipeline {
		int request ;
		int priority ;
} ;

struct mypipeline mypip1;

int main ( void ) {

	CSemaphore ps1("Prod1",1,1);	// e1 datapool semaphore producer
	CSemaphore ps2("Prod2",1,1);	// e2 datapool semaphore producer
	CSemaphore ps3("Prod3",0,1);	// IO pipeline semaphore producer

	CSemaphore cs1("Cons1",1,1);	// e1 datapool semaphore consumer
	CSemaphore cs2("Cons2",1,1);	// e2 datapool semaphore consumer
	CSemaphore cs3("Cons3",1,1);	// IO pipeline semaphore consumer

	printf("Dispatcher Process Creating the Pipeline.....\n") ;
	CPipe	pipe("MyPipe", 1024) ;							// Create a pipe 'p1' with the name "MyPipe"

	cout << "Dispatcher attempting to create/use the datapool....." << endl;
	CDataPool 		dp1("Elevator1", sizeof(struct mydatapooldata)) ;
	CDataPool 		dp2("Elevator2", sizeof(struct mydatapooldata)) ;
	
	struct mydatapooldata 	 *MyDataPool1 = (struct mydatapooldata *)(dp1.LinkDataPool()) ;
	struct mydatapooldata 	 *MyDataPool2 = (struct mydatapooldata *)(dp2.LinkDataPool()) ;

	printf("Dispatcher linked to elevator1 dp at address %p.....\n", MyDataPool1) ;
	printf("Dispatcher linked to elevator2 dp at address %p.....\n", MyDataPool2) ;


	system( "Pause" );
//	Create process objects p1,p2 and p3. Arguments are as follows
//	1)	Pathlist to the child process executable i.e. the runnable child program
//	2)	The process priority. Allowable values are
//		HIGH_PRIORITY_CLASS	
//		IDLE_PRIORITY_CLASS
//		NORMAL_PRIORITY_CLASS	(this is the default if you do not specify). 
//		REALTIME_PRIORITY_CLASS 	(use with extreme caution)
//	3)	Whether the child process uses its own window or uses that of the parent. Values are
//		OWN_WINDOW or PARENT_WINDOW. Default is PARENT_WINDOW	 if not specified
//	4)	The initial processing state of the process, Allowable values are
//		ACTIVE or SUSPENDED. Default is ACTIVE if not specifie

	cout << "Creating Child Processes....." << endl ;
	CProcess e1("C:\\Users\\Tim\\Downloads\\CPEN333ASSN1\\Dispatcher\\Debug\\ELEVATOR_1.exe",	// pathlist to child program executable				
			NORMAL_PRIORITY_CLASS,			// priority
			OWN_WINDOW,						// process has its own window					
			SUSPENDED							// process is active immediately
	) ;

	CProcess e2("C:\\Users\\Tim\\Downloads\\CPEN333ASSN1\\Dispatcher\\Debug\\ELEVATOR_2.exe",	// pathlist to child program executable				
			NORMAL_PRIORITY_CLASS,			// priority
			OWN_WINDOW,						// process has its own window					
			SUSPENDED							// process is active immediately
	) ;

	CProcess io("C:\\Users\\Tim\\Downloads\\CPEN333ASSN1\\Dispatcher\\Debug\\IO.exe",	// pathlist to child program executable				
			NORMAL_PRIORITY_CLASS,			// priority
			OWN_WINDOW,						// process has its own window					
			SUSPENDED							// process is active immediately
	) ;

	e1.Resume();
	e2.Resume();
	io.Resume();

	// Waiting for init rendezvous
	cout << "Waiting for init data rendezvous" << endl;
	r_init1.Wait();

	cout << "Child processes active . . ." << endl;

	//// Waiting for rendezvous
	//cout << "Waiting for data rendezvous" << endl;
	////Sleep(15000);
	//r1.Wait();

//	Sleep(3000);

	cs1.Wait();
	// Print off datapool values for e1
	printf("\nIO Read e1 value for Floor = %d\n", MyDataPool1->floor) ;
	printf("IO Read e1 value for Direction = %d\n", MyDataPool1->direction) ; 
	printf("My e1 onNOMOMOMOM  value is = %d\n", MyDataPool1->onNOMOMOMOM) ; 
	printf("IO Read e1 values for floor array = ") ;
	for(int i=0; i < 10; i ++)
		printf("%d ", MyDataPool1->floors[ i ]) ;
	cs1.Signal();
	cout << endl;

	cs2.Wait();
	// Print off datapool values for e2
	printf("\nIO Read e2 value for Floor = %d\n", MyDataPool2->floor) ;
	printf("IO Read e2 value for Direction = %d\n", MyDataPool2->direction) ; 
	printf("My e2 onNOMOMOMOM  value is = %d\n", MyDataPool2->onNOMOMOMOM) ; 
	printf("IO Read e2 values for floor array = ") ;
	for(int i=0; i < 10; i ++)
		printf("%d ", MyDataPool2->floors[ i ]) ;
	cs2.Signal();

	ps3.Wait();
	pipe.Read(&mypip1, sizeof(mypip1)) ;	// Read the structure from the pipeline
	printf("\n\nDispatcher read mystruct.request = %d, mystruct.priority = %d from Pipeline.....\n\n", mypip1.request, mypip1.priority) ;
	cs3.Signal();

	system("Pause");
	
	// Waiting for terminate rendezvous
	cout << "Waiting for terminate data rendezvous" << endl;
	r_term.Wait();

	cout << "Waiting for elevator 1 to finish . . ." << endl;
	e1.WaitForProcess();

	cout << "Waiting for elevator 2 to finish . . ." << endl;
	e2.WaitForProcess();

	cout << "Waiting for io to finish . . ." << endl;
	io.WaitForProcess();

	//system("Pause");
	return 0;

}