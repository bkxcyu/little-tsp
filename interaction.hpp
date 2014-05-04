#ifndef INTERACTION_H
#define INTERACTION_H

// INCLUDES
#include "common.hpp"

enum programmode_t { NOT_SET, OPTTSP, NAIVETSP };

programmode_t parseArgs(int argc, char* argv[]);
programmode_t checkMode(const char* optarg);
	
#endif // INTERACTION_H
