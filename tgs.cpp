#include "tgs.h"
#include "rtree.h"
#include "rnode.h"
#include "rentry.h"
#include "mem.h"
#include "collection.h"

class Rtree;
class RtreeNode;

int TGS::partition(Rtree& a_rtree, RtreeNodeEntry** a_p, const int a_numEntry, const int a_level, Array& a)
{
	int r = 0;
	const int max = a_level == 0 ? a_rtree.m_maxLeafChild : a_rtree.m_maxNodeChild;
	if (a_numEntry <= max)
	{   // create a node for entries
		int pageid = a_rtree.m_memory.allocatePage();
		RtreeNode* n = new RtreeNode(&a_rtree, pageid, a_level, -1);
		for (int i = 0; i<a_numEntry; i++)
			n->insert(*a_p[i]);
		a_rtree.m_memory.writePage(pageid, n);
		if (a_level > 0)
		{
			for (int i = 0; i < a_numEntry; i++)
			{
				RtreeNode* m = a_rtree.m_memory.loadPage(a_p[i]->m_id);
				m->m_parent = n->m_pageid;
				a_rtree.m_memory.writePage(m->m_pageid, m);
				delete m;
			}
		}
		a.append(n->genNodeEntry());
		delete n;
		a_rtree.m_memory.m_rootPageID = pageid;
		r = 1;
	}
	else
	{   // partition a set of entries into two
		RtreeNodeEntry** gp0 = new RtreeNodeEntry*[a_numEntry];
		RtreeNodeEntry** gp1 = new RtreeNodeEntry*[a_numEntry];
		int cnt0 = 0, cnt1 = 0;

		a_p[0]->packedSplit(a_p, a_numEntry, max, gp0, cnt0, gp1, cnt1);

		int r0 = partition(a_rtree, gp0, cnt0, a_level, a);
		int r1 = partition(a_rtree, gp1, cnt1, a_level, a);
		delete[] gp0;
		delete[] gp1;
		r = r0 + r1;
	}
	return r;
}

Rtree* TGS::bulkload(Memory& a_memory, const int a_dimen, const int a_maxNodeFill, const int a_maxLeafFill, const int a_minNodeFill, const int a_minLeafFill,
	RtreeNodeEntry** a_p, int a_numEntry, const bool a_pointOnly)
{
	Rtree* rtree = new Rtree(a_memory, a_dimen, a_maxNodeFill, a_maxLeafFill, a_minNodeFill, a_minLeafFill, a_pointOnly);

	int level = 0;
	while (true)
	{
		Array a;
		int r = partition(*rtree, a_p, a_numEntry, level, a);
		for (int i = 0; i < a_numEntry; i++)
			delete a_p[i];
		if (r == 1)
		{
			for (int i = 0; i < a.size(); i++)
				delete (RtreeNodeEntry*)a[i];
			a.clean();
			break;
		}
		else
		{
			for (int i = 0; i < a.size(); i++)
				a_p[i] = (RtreeNodeEntry*)a[i];
			a_numEntry = a.size();
			level++;
		}
	}
	return rtree;
}
