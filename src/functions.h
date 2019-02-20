#pragma once
#include "classes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
using namespace std;

void printhelp();
bool isTextFile(string const &fileName, string const &ending);
int writeOutput(Simulation sim, bool verbose, bool per_thread);
string getPType(int type);
double getResponseTime(Simulation sim, int i);
double getTRT(Simulation sim, int i);