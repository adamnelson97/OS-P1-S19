#include "classes.h"
#include "functions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
using namespace std;

// Don't Change!
// Default Constructor
Event::Event() {

}

// Don't Change!
// Parameterized Constructor
Event::Event(int t, string name, Thread thread, int num_threads) {
	time = t;
	event_name = name;
	thread_id = thread.thread_id;
	process_id = thread.process_id;
	thread_type = getPType(thread.type);
	if (name == "THREAD_ARRIVED") { msg = "Transistioned from NEW to READY"; }
	else if (name == "DISPATCHER_INVOKED") { msg = "Selected from " + to_string(num_threads) + "; will run to completion of burst";}
	else if (name == "PROCESS_DISPATCH_COMPLETED") { msg = "Transistioned from READY to RUNNING";}
	else if (name == "THREAD_DISPATCH_COMPLETED") { msg = "Transistioned from READY to RUNNING";}
	else if (name == "CPU_BURST_COMPLETED") { msg = "Transistioned from RUNNING TO BLOCKED";}
	else if (name == "IO_BURST_COMPLETED") { msg = "Transistioned from BLOCKED TO READY";}
	else if (name == "THREAD_COMPLETED") { msg = "Transistioned from READY to EXIT";}
	else if (name == "THREAD_PREEMPTED") { msg = "Transistioned from RUNNING to READY";}
}

// Don't Change!
Simulation::Simulation(ifstream& input) {
	input >> this->num_processes;
	input >> this->thread_overhead;
	input >> this->process_overhead;

	for (int i = 0; i < this->num_processes; i++) {
		Process process; // Create process object
		input >> process.id;
		input >> process.type;
		input >> process.num_threads;
		this->thread_types[process.type] += process.num_threads;

		for (int j = 0; j < process.num_threads; j++) {
			Thread thread; // Create thread object
			input >> thread.arrival_time;
			input >> thread.num_bursts;
			thread.thread_id = j;
			thread.process_id = i;
			thread.type = process.type;
			thread.bursts_processed = 0;
			thread.total_CPU_time = 0; // Initialize total processing times for the thread
			thread.total_IO_time = 0;
			thread.start_time = -1;
			thread.currBurst = 0;

			for (int k = 0; k < thread.num_bursts; k++) {
				Burst burst; // Create burst object
				input >> burst.CPU_time;
				thread.total_CPU_time += burst.CPU_time; // Add processing time to total
				if (k != thread.num_bursts - 1) {
					input >> burst.IO_time;
					thread.total_IO_time += burst.IO_time; // Add processing time to total
				}
				thread.bursts.push_back(burst); // Add burst to thread object
			}

			process.threads.push_back(thread); // Add thread to process object
			this->threadQueue.push(thread); // Adds thread to simulation queue
			this->threadVec.push_back(thread); // Adds thread to simulation vector
			if (thread.type == 0) { sysQueue.push(thread); }
			if (thread.type == 1) { intQueue.push(thread); }
			if (thread.type == 2) { normQueue.push(thread); }
			if (thread.type == 3) { batQueue.push(thread); }
		}
		this->processes.push_back(process); // Add process to sim object
	}
	this->time = 0;
	this->dispatch_time = 0;
	this->service_time = 0;
	this->IO_time = 0;
	this->idle_time = 0;
	this->curr_process_id = -1;
	this->thread_processing = false;

}

// DON'T CHANGE
void Simulation::iterateFCFS() {
	for (int i = 0; i < threadVec.size(); i++) {
		Event event(threadVec[i].arrival_time, "THREAD_ARRIVED", threadVec[i], threadQueue.size());
		events.push_back(event);
	}

	while (!threadQueue.empty()) {
		prevThread = currThread;
		currThread = threadQueue.front();
		threadQueue.pop();
		
		Event event(time, "DISPATCHER_INVOKED", currThread, threadQueue.size());
		events.push_back(event);
		if (prevThread.process_id != currThread.process_id) {
			time += process_overhead;
			dispatch_time += process_overhead;
			Event event(time, "PROCESS_DISPATCH_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}
		else {
			time += thread_overhead;
			dispatch_time += thread_overhead;
			Event event(time, "THREAD_DISPATCH_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}
		if (currThread.start_time == -1) {
			currThread.start_time = time;
			currThread.response_time = currThread.start_time - currThread.arrival_time;
			avg_res_times[currThread.type] += currThread.response_time;
		}

		if (currThread.currBurst < currThread.num_bursts - 1) {
			time += currThread.bursts[currThread.currBurst].CPU_time;
			service_time += currThread.bursts[currThread.currBurst].CPU_time;
			currThread.total_CPU_time += currThread.bursts[currThread.currBurst].CPU_time;
			Event event(time, "CPU_BURST_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
			blockThreads.push_back(currThread);
		}
		else {
			time += currThread.bursts[currThread.currBurst].CPU_time;
			service_time += currThread.bursts[currThread.currBurst].CPU_time;
			currThread.total_CPU_time += currThread.bursts[currThread.currBurst].CPU_time;
			currThread.end_time = time;
			currThread.turn_around_time = currThread.end_time - currThread.arrival_time;
			avg_trt_times[currThread.type] += currThread.turn_around_time;
			processes[currThread.process_id].threads[currThread.thread_id] = currThread;
			Event event(time, "THREAD_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}

		for (int i = 0; i < blockThreads.size(); i++) {
			IO_time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			if (threadQueue.empty()) {
				time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			}
			blockThreads[i].total_IO_time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			Event event(time, "IO_BURST_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
			blockThreads[i].currBurst++;
			threadQueue.push(blockThreads[i]);
		}
		blockThreads.clear();
	}

	idle_time = time - dispatch_time - service_time;
	utilization = 100 * ((float) (time - idle_time)) / (float)time;
	efficiency = 100 * (float) service_time / (float) time; 
}

void Simulation::iterateRR() {
	while (!threadQueue.empty()) {
		prevThread = currThread;
		currThread = threadQueue.front();
		threadQueue.pop();
		
		Event event(time, "DISPATCHER_INVOKED", currThread, threadQueue.size());
		events.push_back(event);

		if (prevThread.process_id != currThread.process_id) {
			time += process_overhead;
			dispatch_time += process_overhead;
			Event event(time, "PROCESS_DISPATCH_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}
		else {
			time += thread_overhead;
			dispatch_time += thread_overhead;
			Event event(time, "THREAD_DISPATCH_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}
		if (currThread.start_time == -1) {
			currThread.start_time = time;
			currThread.response_time = currThread.start_time - currThread.arrival_time;
		}
		if (currThread.currBurst < currThread.num_bursts - 1) {
			if (currThread.bursts[currThread.currBurst].CPU_time > 3) {
				currThread.bursts[currThread.currBurst].CPU_time -= 3;
				time += 3;
				service_time += 3;
				currThread.total_CPU_time += 3;
				Event event(time, "THREAD_PREEMPTED", currThread, threadQueue.size());
				events.push_back(event);
				threadQueue.push(currThread);
			}
			else {
				time += currThread.bursts[currThread.currBurst].CPU_time;
				service_time += currThread.bursts[currThread.currBurst].CPU_time;
				currThread.total_CPU_time += currThread.bursts[currThread.currBurst].CPU_time;
				Event event(time, "CPU_BURST_COMPLETED", currThread, threadQueue.size());
				events.push_back(event);
				blockThreads.push_back(currThread);
			}
		}
		else {
			if (currThread.bursts[currThread.currBurst].CPU_time > 3) {
				currThread.bursts[currThread.currBurst].CPU_time -= 3;
				time += 3;
				service_time += 3;
				currThread.total_CPU_time += 3;
				Event event(time, "THREAD_PREEMPTED", currThread, threadQueue.size());
				events.push_back(event);
				threadQueue.push(currThread);
			}
			else {
				time += currThread.bursts[currThread.currBurst].CPU_time;
				service_time += currThread.bursts[currThread.currBurst].CPU_time;
				currThread.total_CPU_time += currThread.bursts[currThread.currBurst].CPU_time;
				currThread.end_time = time;
				currThread.turn_around_time = currThread.end_time - currThread.arrival_time;
				Event event(time, "THREAD_COMPLETED", currThread, threadQueue.size());
				events.push_back(event);
			}
		}
		for (int i = 0; i < blockThreads.size(); i++) {
			IO_time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			if (threadQueue.empty()) {
				time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			}
			blockThreads[i].total_IO_time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			Event event(time, "IO_BURST_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
			blockThreads[i].currBurst++;
			threadQueue.push(blockThreads[i]);
		}
		blockThreads.clear();
	}

	idle_time = time - dispatch_time - service_time;
	utilization = 100 * ((float) (time - idle_time)) / (float)time;
	efficiency = 100 * (float) service_time / (float) time; 
}


// DON'T CHANGE
void Simulation::iteratePriority() {
	for (int i = 0; i < threadVec.size(); i++) {
		Event event(threadVec[i].arrival_time, "THREAD_ARRIVED", threadVec[i], threadQueue.size());
		events.push_back(event);
	}

	// Re-order threads in queue based on priority
	while (!threadQueue.empty()) threadQueue.pop(); // Clear queue
	while (!sysQueue.empty()) {
		threadQueue.push(sysQueue.front());
		sysQueue.pop();
	}
	while (!intQueue.empty()) {
		threadQueue.push(intQueue.front());
		intQueue.pop();
	}
	while (!normQueue.empty()) {
		threadQueue.push(normQueue.front());
		normQueue.pop();
	}
	while (!batQueue.empty()) {
		threadQueue.push(batQueue.front());
		batQueue.pop();
	}


	for (int i = 0; i < threadVec.size(); i++) {
		Event event(threadVec[i].arrival_time, "THREAD_ARRIVED", threadVec[i], threadQueue.size());
		events.push_back(event);
	}

	while (!threadQueue.empty()) {
		prevThread = currThread;
		currThread = threadQueue.front();
		threadQueue.pop();
		
		Event event(time, "DISPATCHER_INVOKED", currThread, threadQueue.size());
		events.push_back(event);
		if (prevThread.process_id != currThread.process_id) {
			time += process_overhead;
			dispatch_time += process_overhead;
			Event event(time, "PROCESS_DISPATCH_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}
		else {
			time += thread_overhead;
			dispatch_time += thread_overhead;
			Event event(time, "THREAD_DISPATCH_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}
		if (currThread.start_time == -1) {
			currThread.start_time = time;
			currThread.response_time = currThread.start_time - currThread.arrival_time;
			avg_res_times[currThread.type] += currThread.response_time;
		}

		if (currThread.currBurst < currThread.num_bursts - 1) {
			time += currThread.bursts[currThread.currBurst].CPU_time;
			service_time += currThread.bursts[currThread.currBurst].CPU_time;
			currThread.total_CPU_time += currThread.bursts[currThread.currBurst].CPU_time;
			Event event(time, "CPU_BURST_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
			blockThreads.push_back(currThread);
		}
		else {
			time += currThread.bursts[currThread.currBurst].CPU_time;
			service_time += currThread.bursts[currThread.currBurst].CPU_time;
			currThread.total_CPU_time += currThread.bursts[currThread.currBurst].CPU_time;
			currThread.end_time = time;
			currThread.turn_around_time = currThread.end_time - currThread.arrival_time;
			avg_trt_times[currThread.type] += currThread.turn_around_time;
			processes[currThread.process_id].threads[currThread.thread_id] = currThread;
			Event event(time, "THREAD_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
		}

		for (int i = 0; i < blockThreads.size(); i++) {
			IO_time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			if (threadQueue.empty()) {
				time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			}
			blockThreads[i].total_IO_time += blockThreads[i].bursts[blockThreads[i].currBurst].IO_time;
			Event event(time, "IO_BURST_COMPLETED", currThread, threadQueue.size());
			events.push_back(event);
			blockThreads[i].currBurst++;
			threadQueue.push(blockThreads[i]);
		}
		blockThreads.clear();
	}

	idle_time = time - dispatch_time - service_time;
	utilization = 100 * ((float) (time - idle_time)) / (float)time;
	efficiency = 100 * (float) service_time / (float) time; 
}

void Simulation::iterateCustom() {}