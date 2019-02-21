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

}

void Simulation::iterateFCFS() {
	while (!threadQueue.empty()) {
		for (int i = 0; i < threadQueue.size(); i++) {
			Thread thread = threadQueue.front(); // Gets the next thread from the queue
			threadQueue.pop(); // Removes thread from the queue
			if (time == thread.arrival_time) {
				Event event(thread.arrival_time, "THREAD_ARRIVED", thread, threadQueue.size());
				events.push_back(event);
			}
			else {
				threadQueue.push(thread);
				// Adds the thread back to the queue if it's not the arrival time
			}
		}
		time++;
	}

	idle_time = time - dispatch_time - service_time;
	utilization = 100 * ((float) (time - idle_time)) / (float)time;
	efficiency = 100 * (float) service_time / (float) time; 
}

void Simulation::iterateRR() {}
void Simulation::iteratePriority() {}
void Simulation::iterateCustom() {}