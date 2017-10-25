	
// stats.h
//	Routines for managing statistics about Nachos performance.
//
// DO NOT CHANGE -- these stats are maintained by the machine emulation.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"
#include "stats.h"
using namespace std;

long long TimerQuantum=100;

//----------------------------------------------------------------------
// Statistics::Statistics
// 	Initialize performance metrics to zero, at system startup.
//----------------------------------------------------------------------

Statistics::Statistics()
{


	// maybe min= infinity and max=0 but i think its taken care of

	totalCPUBusyTime=totalExecutionTime=CPUUtilization=0;
	maxCPUBurstLength=minCPUBurstLength=averageCPUBurstLength=numNonZeroBursts=averageWaitingTime=0;
	maxThreadCompletionTime=minThreadCompletionTime=averageThreadCompletionTime=varianceThreadCompletionTime=0;


    totalTicks = idleTicks = systemTicks = userTicks = 0;
    numDiskReads = numDiskWrites = 0;
    numConsoleCharsRead = numConsoleCharsWritten = 0;
    numPageFaults = numPacketsSent = numPacketsRecvd = 0;
}

//----------------------------------------------------------------------
// Statistics::Print
// 	Print performance metrics, when we've finished everything
//	at system shutdown.
//----------------------------------------------------------------------

void
Statistics::Print()
{

	totalCPUBusyTime = 	systemTicks+userTicks;
	totalExecutionTime = totalTicks;
	CPUUtilization = (totalCPUBusyTime*100.0)/totalExecutionTime;

	bool flag=false;
	double totalCompletionTime=0.0;
	int numcount=0;
	for(list<int>::iterator i=allCompletionTimes.begin();i!=allCompletionTimes.end();i++){
		numcount++;
		totalCompletionTime+=*i;
		if(!flag){
			minThreadCompletionTime=*i;
			maxThreadCompletionTime=*i;
		}
		if(*i<minThreadCompletionTime) minThreadCompletionTime=*i;
		if(*i>maxThreadCompletionTime) maxThreadCompletionTime=*i;
	}
	averageWaitingTime=totalCompletionTime/numcount;

	for(list<int>::iterator i=allCompletionTimes.begin();i!=allCompletionTimes.end();i++)
		varianceThreadCompletionTime=varianceThreadCompletionTime+ (averageWaitingTime-*i)*(averageWaitingTime-*i);

	varianceThreadCompletionTime/=numcount;

    printf("totalCPUBusyTime %d\n", totalCPUBusyTime); //systemTicks+userTicks
    printf("totalExecutionTime %d\n", totalExecutionTime);
    printf("CPUUtilization %f%%\n",CPUUtilization);
    printf("minCPUBurstLength %d\n", minCPUBurstLength);
    printf("maxCPUBurstLength %d\n", maxCPUBurstLength);
    printf("averageCPUBurstLength %f\n", averageCPUBurstLength);
    printf("numNonZeroBursts %d\n", numNonZeroBursts);


    printf("--minThreadCompletionTime %d\n", minThreadCompletionTime);
    printf("maxThreadCompletionTime %d\n", maxThreadCompletionTime);
    printf("averageWaitingTime %f\n", averageWaitingTime);
    printf("varianceThreadCompletionTime %f\n\n\n", varianceThreadCompletionTime);

    printf("Ticks: total %d, idle %d, system %d, user %d\n", totalTicks,
	idleTicks, systemTicks, userTicks);
    printf("Disk I/O: reads %d, writes %d\n", numDiskReads, numDiskWrites);
    printf("Console I/O: reads %d, writes %d\n", numConsoleCharsRead,
	numConsoleCharsWritten);
    printf("Paging: faults %d\n", numPageFaults);
    printf("Network I/O: packets received %d, sent %d\n", numPacketsRecvd,
	numPacketsSent);
}

 /*
+void Statistics::newBurst(int burstTime) {
+    if (burstTime > 0) {
+        int totalBurstTime = averageBurst * totalNonZeroBursts + burstTime;
+        totalNonZeroBursts++;
+        averageBurst = totalBurstTime / totalNonZeroBursts;
+
+        minBurst = min(burstTime, minBurst);
+        maxBurst = max(burstTime, maxBurst);
+    }
+}
+
+void Statistics::newWait(int waitTime) {
+    int totalWaitTime = averageWait * totalWaits + waitTime;
+    averageWait = totalWaitTime / (totalWaits+1);
+    totalWaits++;
+}
+
+void Statistics::newCompletion(int startToEnd) {
+    compTimes.push_back(startToEnd);
+}F
+*/


void Statistics::updateBurst(int timek){
	if(timek==0) return;

	bool eq=timek!=0 && timek<=2*TimerQuantum;
	ASSERT(eq);
	//HERE IT SHOULD BE QUANTUM LENGTH INSTEAD OF 200

	float time=timek;
	if(minCPUBurstLength>time) minCPUBurstLength=time;
	if(maxCPUBurstLength<time) maxCPUBurstLength=time;
	averageCPUBurstLength=(averageCPUBurstLength*(numNonZeroBursts)+ time)/(numNonZeroBursts+1);
	numNonZeroBursts++;
}
/*
void Statistics::updateThreadCompletion(){
----------------- WE NEED TO COMPLETE THREADCOMPLETIONTIME FOR EACH PRCOESS, THEN TAKE AVERA,MI,MA,ETC
*/
void Statistics::updateCompletionTimes(int duration){
	allCompletionTimes.push_front(duration);
}
