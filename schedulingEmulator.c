#include "inc.h"

// FileRead_thread
// Read the inputted file, create (emulated) process, place in Ready_Q

// CPU_scheduler_thread
// Emulate by sleeping for given CPU burst time, then release, and place in IO_Q

// IO_scheduler_thread
// Emulate by sleeping for given IO burst time, then release, and place in Ready_Q