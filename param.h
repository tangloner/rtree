/*
This header file includes class Param decleration.
It reads and filters program arguments
*/

#ifndef PARAM_DEFINED
#define PARAM_DEFINED

#include <string>
#include <cstring>

class Param
{
public:
	static const char* read(const int a_argc, const char** a_argv, const char* a_param, const char* a_def);
};

#endif