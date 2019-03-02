#include "skyline.h"


void rtreeRAM(Rtree& rt, unordered_map<long, RtreeNode*>& ramTree)
{
	ramTree.clear();
	queue<long> H;
	RtreeNode* node;
	H.push(rt.m_memory.m_rootPageID);
	long pageID;
	while (!H.empty())
	{
		pageID = H.front();
		H.pop();
		node = rt.m_memory.loadPage(pageID);
		ramTree[pageID] = node;
		if (node->isLeaf() == false)
		{
			for (int i = 0; i < node->m_usedspace; i++)
			{
				H.push(node->m_entry[i]->m_id);
			}
		}
	}
}

