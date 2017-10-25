// main.cc
//	Bootstrap code to initialize the operating system kernel.
//
//	Allows direct calls into internal operating system functions,
//	to simplify debugging and testing.  In practice, the
//	bootstrap code would just initialize data structures,
//	and start a user program to print the login prompt.
//
// 	Most of this file is not needed until later assignments.
//
// Usage: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file> -c <consoleIn> <consoleOut>
//		-f -cp <unix file> <nachos file>
//		-p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d causes certain debugging messages to be printed (cf. utility.h)
//    -rs causes Yield to occur at random (but repeatable) spots
//    -z prints the copyright message
//
//  USER_PROGRAM
//    -s causes user programs to be executed in single-step mode
//    -x runs a user program
//    -c tests the console
//
//  FILESYS
//    -f causes the physical disk to be formatted
//    -cp copies a file from UNIX to Nachos
//    -p prints a Nachos file to stdout
//    -r removes a Nachos file from the file system
//    -l lists the contents of the Nachos directory
//    -D prints the contents of the entire file system
//    -t tests the performance of the Nachos file system
//
//  NETWORK
//    -n sets the network reliability
//    -m sets this machine's host id (needed for the network)
//    -o runs a simple test of the Nachos network software
//
//  NOTE -- flags are ignored until the relevant assignment.
//  Some of the flags are interpreted here; some in system.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.


#define q4 100
#define a 100


#define MAIN
#include "copyright.h"
#undef MAIN

#include "utility.h"
#include <fstream>
#include "system.h"
#include "scheduler.h"
using namespace std;
// External functions used by this file

extern void ThreadTest(void), Copy(char *unixFile, char *nachosFile);
extern void BeginBatch(char files[][300], int *priorities, int batchSize);
extern void Print(char *file), PerformanceTest(void);
extern void StartUserProcess(char *file),
  ConsoleTest(char *in, char *out);
extern void LaunchUserProcess(char *file), ConsoleTest(char *in, char *out);
extern void MailTest(int networkID);

//----------------------------------------------------------------------
// main
// 	Bootstrap the operating system kernel.
//
//	Check command line arguments
//	Initialize data structures
//	(optionally) Call test procedure
//
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------

int
main(int argc, char **argv)
{
    int argCount;			// the number of arguments
					// for a particular command

    DEBUG('t', "Entering main");
    (void) Initialize(argc, argv);

#ifdef THREADS
    ThreadTest();
#endif

    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	argCount = 1;
        if (!strcmp(*argv, "-z"))               // print copyright
            printf (copyright);
#ifdef USER_PROGRAM
        if(!strcmp(*argv,"-F")){
        
     //            ASSERT(FALSE);
          if(argc < 2){
            printf("file not specified\n");
            return 1;
          }

          //printf("Processes to run:\n");

////Batchmode use it or not?????
          char files[100][300];
          int priorities[100];
          int cnt = 0;
          std::ifstream input(argv[1]);


/*
          std::string firstLine
          getline(input,firstLine);
*/
/*
is this required???
int val, result = readInteger(firstLine.c_str(), val);
        if (result != 0 || val < 1 || val > 10) {
            printf("Bad scheduling algorithm number: %s\n", firstLine.c_str());
            input.close();
            return 1;
}
*/
// scheduler->schedAlgo = static_cast<SchedulingAlgo>(val);

          string line;
          getline(input,line);
          const char *cstr=line.c_str();

          // As described in the notes, 1 - corresponds to Nachos unpre-emptive algorithm
          // 2- etc..etc...
          int SchedulingAlgorithm=atoi(cstr);

          switch (SchedulingAlgorithm) {
            case 3:
            case 7:
              TimerQuantum=a/4.0;
              break;
            case 4:
            case 8:
              TimerQuantum=a/2.0;
              break;
            case 5:
            case 9:
              TimerQuantum=a*3.0/4;
              break;
            case 6:
            case 10:
              TimerQuantum=q4;
              break;
            default:
              TimerQuantum=100L;
          }
          
          while(getline(input,line)){
            int pos = line.find(' ');
            if(pos < line.length()){
              if(line[pos+1] >= '0' && line[pos+1] <= '9'){
                priorities[cnt]=atoi(line.c_str()+pos+1);
/*
result = readInteger(line.substr(pos+1).c_str(),
                                         priorities[cnt]);
                    if (result != 0) {
                        printf("Bad priority value in %s\n", line.c_str());
                        input.close();
                        return 1;
}

*/
                line[pos]=0;
                strcpy(files[cnt], line.c_str());
              }
              else{
                printf("bad string\n");
                input.close();
                return 1;
               }
            }
            else{
              priorities[cnt]=100;
              strcpy(files[cnt], line.c_str());
            }
            cnt++;
          }
          input.close();
          printf("Processes to run:\n");
          for (int i=0; i<cnt; i++) {
            printf("%s: %d\n", files[i], priorities[i]);
          }
          DEBUG('t', "Starting running of processes\n");
         //return;
         //ASSERT(FALSE);
         BeginBatch(files, priorities, cnt);
         
         //	break;
          argCount = 2;
        }

        if (!strcmp(*argv, "-x")) {        	// run a user program
	    ASSERT(argc > 1);
            LaunchUserProcess(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-c")) {      // test the console
	    if (argc == 1)
	        ConsoleTest(NULL, NULL);
	    else {
		ASSERT(argc > 2);
	        ConsoleTest(*(argv + 1), *(argv + 2));
	        argCount = 3;
	    }
	    interrupt->Halt();		// once we start the console, then
					// Nachos will loop forever waiting
					// for console input
	}
#endif // USER_PROGRAM
#ifdef FILESYS
	if (!strcmp(*argv, "-cp")) { 		// copy from UNIX to Nachos
	    ASSERT(argc > 2);
	    Copy(*(argv + 1), *(argv + 2));
	    argCount = 3;
	} else if (!strcmp(*argv, "-p")) {	// print a Nachos file
	    ASSERT(argc > 1);
	    Print(*(argv + 1));
	    argCount = 2;rpm
	} else if (!strcmp(*argv, "-r")) {	// remove Nachos file
	    ASSERT(argc > 1);
	    fileSystem->Remove(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-l")) {	// list Nachos directory
            fileSystem->List();
	} else if (!strcmp(*argv, "-D")) {	// print entire filesystem
            fileSystem->Print();
	} else if (!strcmp(*argv, "-t")) {	// performance test
            PerformanceTest();
	}
#endif // FILESYS
#ifdef NETWORK
        if (!strcmp(*argv, "-o")) {
	    ASSERT(argc > 1);
            Delay(2); 				// delay for 2 seconds
						// to give the user time to
						// start up another nachos
            MailTest(atoi(*(argv + 1)));
            argCount = 2;
        }
#endif // NETWORK
    }

    currentThread->FinishThread();	// NOTE: if the procedure "main"
				// returns, then the program "nachos"
				// will exit (as any other normal program
				// would).  But there may be other
				// threads on the ready list.  We switch
				// to those threads by saying that the
				// "main" thread is finished, preventing
				// it from returning.
    return(0);			// Not reached...
}
