// scheduler.cc
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would
//	end up calling SelectNextReadyThread(), and that would put us in an
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "scheduler.h"
#include "system.h"

//----------------------------------------------------------------------
// ProcessScheduler::ProcessScheduler
// 	Initialize the list of ready but not running threads to empty.
//----------------------------------------------------------------------

ProcessScheduler::ProcessScheduler()
{
  listOfReadyThreads = new List;

#if def USER_PROGRAM
    SchedulingAlgorithm=0;
#endif
}


//----------------------------------------------------------------------
// ProcessScheduler::~ProcessScheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

ProcessScheduler::~ProcessScheduler()
{
    delete listOfReadyThreads;
}

//----------------------------------------------------------------------
// ProcessScheduler::MoveThreadToReadyQueue
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
ProcessScheduler::MoveThreadToReadyQueue (NachOSThread *thread)
{
  thread->threadStatistics->stoppedRunning();

    DEBUG('t', "Putting thread %s with PID %d on ready list.\n", thread->getName(), thread->GetPID());

    thread->setStatus(READY);
#ifndef USER_PROGRAM
    listOfReadyThreads->Append((void *)thread);
#else
    listOfReadyThreads->SortedInsertInWaitQueue((void*)thread,thread->priorityValue);
#endif
}

//----------------------------------------------------------------------
// ProcessScheduler::SelectNextReadyThread
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

NachOSThread *
ProcessScheduler::SelectNextReadyThread ()
{
#ifndef USER_PROGRAM
  return (NachOSThread *)listOfReadyThreads->Remove();
#else
  switch (scheduler->SchedulingAlgorithm) {
    case 7:
    case 8:
    case 9:
    case 10:
    default:
      RearrangeThreadsOfReadyQueue();
  }
  return (NachOSThread *)listOfReadyThreads->Remove();
#endif
}

//----------------------------------------------------------------------
// ProcessScheduler::ScheduleThread
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//----------------------------------------------------------------------

void
ProcessScheduler::ScheduleThread (NachOSThread *nextThread)
{
    //int CPUUsageTime=currentThread->threadStatistics->stoppedRunning();
    //stats->updateBurst(CPUUsageTime);
  	nextThread->threadStatistics->startRunning();

    NachOSThread *oldThread = currentThread;

#ifdef USER_PROGRAM			// ignore until running user programs
    changePriorityCarefully(CPUUsageTime);
    if (currentThread->space != NULL) {	// if this thread is a user program,
        currentThread->SaveUserState(); // save the user's CPU registers
	       currentThread->space->SaveContextOnSwitch();
    }

    // THIS IS ONLY FOR UNIX SCHEDULING AGLO
    switch (scheduler->SchedulingAlgorithm) {
      case 7:
      case 8:
      case 9:
      case 10:
          break;
          //not needed, redundant
          changePriorityCarefully(CPUUsageTime);
      default:
        break;
    }


#endif

    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    currentThread = nextThread;		    // switch to the next thread
    currentThread->setStatus(RUNNING);      // nextThread is now running


#ifndef
  //hey
#else
    printf("[%d][Time: %d] Scheduling with priority: %d\n", currentThread->GetPID(), stats->totalTicks, currentThread->priorityValue + currentThread->CPUUsageTime/2);
#endif

    DEBUG('t', "Switching from thread \"%s\" with pid %d to thread \"%s\" with pid %d\n",
	  oldThread->getName(), oldThread->GetPID(), nextThread->getName(), nextThread->GetPID());

    // This is a machine-dependent assembly language routine defined
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    _SWITCH(oldThread, nextThread);

    DEBUG('t', "Now in thread \"%s\" with pid %d\n", currentThread->getName(), currentThread->GetPID());

    // If the old thread gave up the processor because it was finishing,
    // we need to delete its carcass.  Note we cannot delete the thread
    // before now (for example, in NachOSThread::FinishThread()), because up to this
    // point, we were still running on the old thread's stack!
    if (threadToBeDestroyed != NULL) {
        delete threadToBeDestroyed;
	threadToBeDestroyed = NULL;
    }

#ifdef USER_PROGRAM
    if (currentThread->space != NULL) {		// if there is an address space
        currentThread->RestoreUserState();     // to restore, do it.
	currentThread->space->RestoreContextOnSwitch();
    }
#endif
}

//----------------------------------------------------------------------
// ProcessScheduler::Tail
//      This is the portion of ProcessScheduler::ScheduleThread after _SWITCH(). This needs
//      to be executed in the startup function used in fork().
//----------------------------------------------------------------------

void
ProcessScheduler::Tail ()
{
    // If the old thread gave up the processor because it was finishing,
    // we need to delete its carcass.  Note we cannot delete the thread
    // before now (for example, in NachOSThread::FinishThread()), because up to this
    // point, we were still running on the old thread's stack!
    if (threadToBeDestroyed != NULL) {
        delete threadToBeDestroyed;
        threadToBeDestroyed = NULL;
    }

#ifdef USER_PROGRAM
    if (currentThread->space != NULL) {         // if there is an address space
        currentThread->RestoreUserState();     // to restore, do it.
        currentThread->space->RestoreContextOnSwitch();
    }
#endif
}

void
ProcessScheduler::RearrangeThreadsOfReadyQueue(){

  int k=0;
  while (k<thread_index) {
    if(exitThreadArray[k]){
      k++;
      continue;
    }
    threadArray[i]->CPUUsage/=2;
  }

  NachOSThread* t;
  for(List*l=new List;!listOfReadyThreads->IsEmpty();l->SortedInsert(t,t->priorityValue+t->CPUUsage/2))
    l=(NachOSThread* )listOfReadyThreads->Remove();
  listOfReadyThreads=l;

REMOVE THIS AFTER debugging
         printf("Priorities:\n");
        for(unsigned int i = 1; i < thread_index; i++) {
            if (!exitThreadArray[i]) {
                printf("%d ", threadArray[i]->priority + threadArray[i]->cpuCount/2);
            }
        }
        printf("\n");


}

//----------------------------------------------------------------------
// ProcessScheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
ProcessScheduler::Print()
{
    printf("Ready list contents:\n");
    listOfReadyThreads->Mapcar((VoidFunctionPtr) ThreadPrint);
}
