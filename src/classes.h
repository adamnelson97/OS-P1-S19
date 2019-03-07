#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
#include <queue>
using namespace std;

// Structs/Classes
struct Burst {
	int CPU_time;
	int IO_time;
	bool IO_processed;
};

struct Thread {
	int arrival_time;
	int start_time;
	int response_time;
	int total_CPU_time;
	int total_IO_time;
	int turn_around_time;
	int end_time;
	int bursts_processed;
	bool IO_processed;
	bool CPU_processed;

	int num_bursts;
	int currBurst;
	int thread_id;
	int process_id;
	int type;
	string state;
	vector<Burst> bursts;
};

struct Process {
	int id;
	int type;
	int num_threads;
	vector<Thread> threads;
};

class Event {
public:
	Event();
	Event(int t, string name, Thread thread, int num_threads);
	int time;
	string event_name;
	int thread_id;
	int process_id;
	string thread_type;
	string start_thread_state;
	string end_thread_state;
	string msg;
};

class Simulation {
public:
	Simulation();
	Simulation(ifstream& input);
	int num_processes;
	int thread_overhead;
	int process_overhead;
	vector<Process> processes;
	int thread_types[4] = {0,0,0,0};
	int avg_res_times[4] = {0,0,0,0};
	int avg_trt_times[4] = {0,0,0,0};
	vector<Event> events;
	queue<Thread> threadQueue;
	// Priority Queues
	queue<Thread> sysQueue;
	queue<Thread> intQueue;
	queue<Thread> normQueue;
	queue<Thread> batQueue;
	vector<Thread> blockThreads;
	Thread currThread;
	Thread prevThread;
	int curr_process_id;
	bool diff_process;
	bool all_threads_complete;
	int time;
	int overhead_counter;
	bool thread_processing;
	int dispatch_time;
	int service_time;
	int IO_time;
	int idle_time;
	float utilization;
	float efficiency;
	void iterateFCFS();
	void iterateRR();
	void iteratePriority();
	void iterateCustom();
};