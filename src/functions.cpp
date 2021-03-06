#include "functions.h"
#include "classes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <queue>
using namespace std;

//Don't Change!
void printhelp() {
	cout << endl << "Format: ./simulator [flags] simulation_file.txt" << endl
		<< "-t, --per_thread" << endl
		<< "\tOutput additional per-thread statistics for arrival time, service time, etc." << endl
		<< "-v, --verbose" << endl
		<< "\tOutput information about every state-changing event and scheduling decision." << endl
		<< "-a, --algorithm" << endl
		<< "\tThe scheduling algorithm to use. One of FCFS, RR, PRIORITY, or CUSTOM." << endl
		<< "-h --help" << endl
		<< "\tDisplay a help message about these flags and exit" << endl;
}

//Don't Change!
bool isTextFile (string const &fileName, string const &ending) {
    if (fileName.length() >= ending.length()) {
        return (0 == fileName.compare(fileName.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

// Don't Change!
int writeOutput(Simulation sim, bool verbose, bool per_thread) {
	cout << endl;
	if (per_thread) {
		cout << endl;
		for (int i = 0; i < sim.num_processes; i++) {
			cout << "Process " << i << " [" << getPType(sim.processes[i].type) << "]:" << endl;
			for (int j = 0; j < sim.processes[i].num_threads; j++) {
				cout << "\tThread " << j
					<< ":\tARR: " << sim.processes[i].threads[j].arrival_time
					<< "\tCPU: " << sim.processes[i].threads[j].total_CPU_time
					<< "\tI/O: " << sim.processes[i].threads[j].total_IO_time
					<< "\tTRT: " << sim.processes[i].threads[j].turn_around_time
					<< "\tEND: " << sim.processes[i].threads[j].end_time << endl;
			}
			cout << endl;
		}
	}

	cout << "SIMULATION COMPLETED!\n" << endl;
	for (int i = 0; i < 4; i++) {
		cout << getPType(i) << " THREADS:" << endl;
		cout << setw(27) << left << "\tTotal count:" << right << sim.thread_types[i] << endl;
		if (sim.thread_types[i] == 0) {
			cout << setw(27) << left << "\tAvg response time:" << right << 0 << endl;
			cout << setw(27) << left << "\tAvg turnaround time:" << right << 0 << endl;
		}
		else {
			cout << setw(27) << left << "\tAvg response time:" << right << (double) sim.avg_res_times[i] / (double) sim.thread_types[i] << endl;
			cout << setw(27) << left << "\tAvg turnaround time:" << right << (double) sim.avg_trt_times[i] / (double) sim.thread_types[i] << endl;
		}
	}
	cout << endl;
	cout << setw(34) << left << "Total elapsed time:" << right << sim.time << endl;
	cout << setw(34) << left << "Total service time:" << right << sim.service_time << endl;
	cout << setw(34) << left << "Total I/O time:" << right << sim.IO_time << endl;
	cout << setw(34) << left << "Total dispatch time:" << right << sim.IO_time << endl;
	cout << setw(34) << left << "Total idle time:" << right << sim.idle_time << endl;
	cout << endl;
	cout << setw(34) << left << "CPU utilization:" << right << sim.utilization << "%" << endl;
	cout << setw(34) << left << "CPU efficiency:" << right << sim.efficiency << "%" << endl;
	cout << endl;
	
	// Verbose output
	if (verbose) {
		//cout << endl;
		for (int x = 0; x < sim.time; x++) {
			for (int i = 0; i < sim.events.size(); i++) {
				if (sim.events[i].time == x) {
					cout << "At time " << sim.events[i].time << ":" << endl
						<< "\t" << sim.events[i].event_name << endl
						<< "\tThread " << sim.events[i].thread_id
						<< " in process " << sim.events[i].process_id << " ["
						<< sim.events[i].thread_type << "]" << endl
						<< "\t" << sim.events[i].msg << "\n" << endl;
				}
			}
		}
	}

	cout << "\nSIMULATION COMPLETE\n\n" << endl;

	return 0; // Successful output
}

// Don't Change!
string getPType(int type) {
	if (type == 0) { return "SYSTEM";}
	else if (type == 1) { return "INTERACTIVE";}
	else if (type == 2) { return "NORMAL";}
	else if (type == 3) { return "BATCH";}
	else return "ERROR: TYPE NOT FOUND";
}