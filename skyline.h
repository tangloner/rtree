#ifndef _SKYLINE_H_
#define _SKYLINE_H_

#include "header.h"
#include "rtree.h"
#include "rnode.h"
#include "rentry.h"
#include "filemem.h"
#include "global.h"

#define MAXPAGEID 49999999

void rtreeRAM(Rtree& rt, unordered_map<long, RtreeNode*>& ramTree);


#endif
