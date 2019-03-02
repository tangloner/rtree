/*this header file includes serveral general function declartion
it helps the debuger process and program usages
*/
#ifndef _GLOBAL_H
#define _GLOBAL_H

#define _CRT_SECURE_NO_DEPRECATE

#include "header.h"

// help messages for usage
void helpmsg(const char* pgm);

double dataSize(int objCnt, int dim);

int PointvsHS(const int& Dimen, const double hs[], vector<double>& tmpPT);

int MBRvsHS(const int& Dimen, const double hs[], const double MBR[], vector<string>& Comb);

void myitoa(unsigned long val, char* buf, unsigned radix);

bool MBRisValid(const int &Dimen, const double hsp[], const double mbr[], vector<string> &Comb);

void genLenNBinStrs(long int len1, long int HammingDist, multimap<int, vector<char>>& binString);

void GenString(long int stringLen, long int HammingDist, long int curLen, long int start, vector<char>& hammstr, multimap<int, vector<char>>& binString);

void GenerateQuery(int D, int queries);

void Generateallqueries(int D);

void removeSkylines(unordered_set<long int>& skylines, unordered_set<long int>& removeSL, unordered_set<long int>& ignoreRecords);

bool dominateRecords(double* a, double* b, const int dimen);

void parseRegions(string &strreg, vector<double>& regions);

#endif