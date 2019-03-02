/*
Created by tangloner Dec 9 1018
Copyright @ DBGroup, SUSTech

Bulkload Rtree construction.

The user may use it as a test interface for rtree functions
*/

#include <fstream>
#include "header.h"
#include "hypercube.h"
#include "rentry.h"
#include "rnode.h"
#include "rtree.h"
#include "filemem.h"
#include "tgs.h"
#include "global.h"
#include "param.h"
#include "skyline.h"


unordered_map<long, RtreeNode *> ramTree; // load Rtree to main-memory
int objCnt = 0; // object count, init with 0
double totalSpaceCost = 0.0; // space cost (MB)

int main(const int argc, const char** argv)
{
	int dim = atoi(Param::read(argc, argv, "-d", ""));   // data dimensions
	const char *indexfile = Param::read(argc, argv, "-i", ""); // r-tree file
	const char *datafile = Param::read(argc, argv, "-f", "");  // data file 
	int n = atoi(Param::read(argc, argv, "-n", ""));   // data size

	cout << "Load data points from file" << endl;
	float** PointSet = new float*[MAXPTS + 1];
	RtreeNodeEntry** p = new RtreeNodeEntry*[MAXPTS];
	fstream fpdata;
	fpdata.open(datafile, ios::in);
	while (true)
	{
		int id;
		float* cl = new float[dim];
		float* cu = new float[dim];
		fpdata >> id;
		if (fpdata.eof())
			break;

		PointSet[objCnt + 1] = new float[2 * dim];

		for (int d = 0; d < dim; d++)
		{
			fpdata >> cl[d];
			PointSet[objCnt + 1][d] = cl[d];
		}

		for (int d = 0; d < dim; d++)
		{
			fpdata >> cu[d];
			PointSet[objCnt + 1][d + dim] = cu[d];
		}

		Hypercube hc(dim, cl, cu);
		p[objCnt++] = new RtreeNodeEntry(id, hc);

		//log information
		if (objCnt % 1000 == 0)
			cout << ".";
		if (objCnt % 10000 == 0)
			cout << objCnt << " objects loaded" << endl;
	}

	double rawSize = dataSize(objCnt, dim);
	cout << "Total number of objects: " << objCnt << endl;
	totalSpaceCost += rawSize;

	// build rtree
	cout << "Bulkloading R-tree..." << endl;
	const int maxChild = (PAGESIZE - RtreeNode::size()) / RtreeNodeEntry::size(dim);
	FileMemory mem(PAGESIZE, indexfile, RtreeNodeEntry::fromMem, true);
	Rtree* rtree = TGS::bulkload(mem, dim, maxChild, maxChild, (int)maxChild*0.3, (int)maxChild*0.3, p, objCnt, false);

	cout << "[Rtree build done]" << endl;

	// load r-tree to memory, in-memory rtree
	cout << "loading R-tree for In-Memeory Processing..." << endl;
	rtreeRAM(*rtree, ramTree);   
	totalSpaceCost += ramTree.size() * 4096.00 / MB;


	return 0;

}