#include "classes.h"
#include "functions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
using namespace std;

int main(int argc, char *argv[]) {
	// Debugging
	// for (int i = 0; i < argc; i++) {
	// 	cout << argv[i] << endl;
	// }

	// Parse command-line arguments
	bool per_thread = false;
	bool verbose = false;
	bool algorithm = false;
	string alg_type;

	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--per_thread") == 0) { per_thread = true; }
		if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) { verbose = true; }
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { printhelp(); return 0; }
		if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0) {
			alg_type = argv[i+1];
			cout << "Algorithm Type: " << alg_type << endl;
			algorithm = true;
		}
	}

	// Create input file stream
	string fileName;
	for (int i = 0; i < argc; i++) {
		if (isTextFile(argv[i], ".txt")) { fileName = argv[i]; }
	}
	cout << "Input File: " << fileName << endl;

	ifstream fileIn(fileName);
	if (fileIn.fail()) {
		cerr << "File " << fileName << " not found." << endl;
		return -1;
	}

	// Create simulation object from input file
	Simulation sim(fileIn);
	if (algorithm) {
		if (alg_type == "FCFS") { sim.iterateFCFS(); }
		if (alg_type == "RR") { sim.iterateRR(); }
		if (alg_type == "PRIORITY") { sim.iteratePriority(); }
		if (alg_type == "CUSTOM") { sim.iterateCustom(); }
	}
	else sim.iterateFCFS(); // Default algorithm if none is specified

	return writeOutput(sim, verbose, per_thread); //Normal return statement
}