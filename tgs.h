/* ----------------------------------------------------------------------------
Author: Ken C. K. Lee
Email:  cklee@cse.psu.edu

Copyright(c) Ken C. K. Lee 2007

This header file declares TGS BulkLoader class.

This is implemented based on
"A Greedy Algorithm for Bulk Loading R-trees"
Yvan J. Garcia, Mario A. Lopez and Scott T. Leutenegger
ACM GIS 1998
---------------------------------------------------------------------------- */


#include "mem.h"
#include "header.h"

#ifndef TGS_DEFINED
#define TGS_DEFINED

class Memory;
class Rtree;
class RtreeNode;
class RtreeNodeEntry;

class TGS
{
public:
	// bulk load a set of entires and form a Rtree
	static Rtree* bulkload(Memory& a_memory, const int a_dimen, const int a_maxNodeFill, const int a_maxLeafFill, const int a_minNodeFill, const int a_minLeafFill,
		RtreeNodeEntry** a_p, int a_numEntry, const bool a_pointOnly = false);

	static int partition(Rtree& a_rtree, RtreeNodeEntry** a_p, const int a_numEntry, const int a_level, Array& a);
};

#endif // TGS_DEFINED

