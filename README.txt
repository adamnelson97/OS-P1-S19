Name: Adam Nelson

Files:
	classes.cpp		File for functions of custom classes and structs
	classes.h		Header file for custom classes and structs
	functions.cpp	File for functions used within simulation.cpp
	functions.h 	Header file for simulation.cpp functions
	simulation.cpp	Main file that runs the simulator

Unusual/Interesting Features:
	My program does not have anything that is out of the ordinary.

Approxmiate Number of Hours: 25

My Algorithm:

My algorithm tries to reduce the overall simulator time as much as possible by reducing thread
and process switching. The dispatch overhead is wasted time, so by eliminating dispatches more
time is spent on actual processing. 

It prioritizes processes based on the SYSTEM/INTERACTIVE/NORMAL/BATCH order, and then processes
threads within the process based on their arrival time. I met the required number
of queues by using the same queues used for the PRIORITY algorithm.

Starvation is not possible because the outer-most while loop constantly checks that at least
one of the queues has a thread in it. The simulation is only complete when all queues are
simultaneously empty. Otherwise it will keep running, so starvation won't occur.

My algorithm is not fair, because of the strict priority schedule. For instance, if there is
only one Normal process that needs to run but there are 100 System processes, the Normal will
run last.