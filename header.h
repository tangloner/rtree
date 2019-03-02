// header files and namespace
#ifndef _HEADER_H
#define _HEADER_H

// standard libary
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <string>
#include <algorithm>
#include <inttypes.h>
#include <memory.h>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <unordered_map>
#include <random>
#include <queue>
#include <unordered_set>

// overload data structures
#include "collection.h"
 
// define platform
#define WINDOWS
//#define UBUNTU
//#define MAC

//#define DEBUG

using namespace std;

#define MAXPTS 10000001  // macro define the maximum number of points
#define MB 1048576      // macro define the size of MB

#ifdef DEBUG
	#define PAGESIZE 144 // for debugging
#else
	#define PAGESIZE 4096   // macro define the pagesize (KB)
#endif


#define LEAFNODE -1
#define NONLEAFNODE 1
#define ABOVE	1
#define BELOW	2
#define ON 3
#define HP_OVERLAPPED	4


#define MAXNOBINSTRINGTOCHECK 60000
#define MAXLOOP 10000
#define ZEROEXTENT 1e-15

#define SIDELEN 0.0001
#define CELLING 1.000

#endif
