#include "rtree.h"
#include "rnode.h"
#include "rentry.h"
#include "mem.h"
#include "header.h"

Rtree::Rtree(Memory& a_memory, const int a_dimen, const int a_maxNodefill, const int a_maxLeafFill, const int a_minNodeFill, const int a_minLeafFill, const bool a_pointOnly) :
m_memory(a_memory), m_dimen(a_dimen), m_maxNodeChild(a_maxNodefill), m_minNodeChild(a_minNodeFill), m_maxLeafChild(a_maxLeafFill), m_minLeafChild(a_minLeafFill), m_pointOnly(a_pointOnly)
{
	m_memory.m_rtree = this;
}

Rtree::~Rtree(){}

int Rtree::insert(const RtreeNodeEntry& a_entry, const int a_level)
{
	if (m_memory.m_rootPageID == -1)
	{   // no tree exists
		m_memory.m_rootPageID = m_memory.allocatePage();
		RtreeNode n(this, m_memory.m_rootPageID);
		n.insert(a_entry);
		m_memory.writePage(m_memory.m_rootPageID, &n);
	}
	else
	{
		RtreeNode* n = chooseLeaf(a_entry, a_level);
		int status = n->insert(a_entry);

		if (a_level > 0)
		{
			RtreeNode* m = m_memory.loadPage(a_entry.m_id);
			m->m_parent = n->m_pageid;
			m_memory.writePage(m->m_pageid, m);
			delete m;
		}

		while (status != NODE_UNCHANGED)
		{
			RtreeNode* parent = 0;
			switch (status)
			{

#pragma region NODE_OVERFLOW
			case NODE_OVERFLOW:
			{
								  RtreeNode *n0, *n1;
								  n->quadraticSplit(&n0, &n1);
								  n0->m_pageid = n->m_pageid;
								  n1->m_pageid = m_memory.allocatePage();
								  n->m_usedspace = 0; // reset the usedspace since all entries are moved to split nodes
								  if (!n->isLeaf())
								  {
									  for (int i = 0; i < n0->m_usedspace; i++)
									  {
										  int id = n0->m_entry[i]->m_id;
										  RtreeNode* child = m_memory.loadPage(id);
										  child->m_parent = n0->m_pageid;
										  m_memory.writePage(id, child);
										  delete child;
									  }

									  for (int i = 0; i < n1->m_usedspace; i++)
									  {
										  int id = n1->m_entry[i]->m_id;
										  RtreeNode* child = m_memory.loadPage(id);
										  child->m_parent = n1->m_pageid;
										  m_memory.writePage(id, child);
										  delete child;
									  }
								  }

								  if (n->isRoot())
								  {
									  int parentid = m_memory.allocatePage();
									  m_memory.m_rootPageID = parentid;
									  parent = new RtreeNode(this, parentid, n->m_level + 1, -1);

									  RtreeNodeEntry* e0 = n0->genNodeEntry();
									  n0->m_parent = parentid;
									  parent->insert(*e0);

									  RtreeNodeEntry* e1 = n1->genNodeEntry();
									  n1->m_parent = parentid;
									  parent->insert(*e1);

									  m_memory.writePage(n0->m_pageid, n0);
									  m_memory.writePage(n1->m_pageid, n1);

									  status = NODE_UNCHANGED;
									  delete e0;
									  delete e1;
								  }
								  else
								  {
									  parent = m_memory.loadPage(n->m_parent);
									  RtreeNodeEntry* e0 = n0->genNodeEntry();
									  n0->m_parent = parent->m_pageid;
									  int status1 = parent->replace(*e0);

									  RtreeNodeEntry* e1 = n1->genNodeEntry();
									  n1->m_parent = parent->m_pageid;
									  int status2 = parent->replace(*e1);

									  m_memory.writePage(n0->m_pageid, n0);
									  m_memory.writePage(n1->m_pageid, n1);
									  status = status2;
									  if (status != NODE_OVERFLOW&&status1 == NODE_CHANGED)
									  {
										  status = NODE_CHANGED;
									  }
									  delete e0;
									  delete e1;
								  }
								  delete n0;
								  delete n1;
								  break;
			}
#pragma endregion

#pragma region NODE_CHANGED
			case NODE_CHANGED:
			{
								 if (n->isRoot())
								 {
									 status = NODE_UNCHANGED;
									 parent = n->clone();
									 break;  //no parent to update; quit!
								 }
								 else
								 {
									 parent = m_memory.loadPage(n->m_parent);
									 RtreeNodeEntry* e = n->genNodeEntry();
									 status = parent->replace(*e);
									 m_memory.writePage(n->m_pageid, n);
									 delete e;
								 }
			}
#pragma endregion

			}
			delete n;
			n = parent;
		}
		m_memory.writePage(n->m_pageid, n);
		delete n;
	}
	return 0;
}

RtreeNode* Rtree::chooseLeaf(const RtreeNodeEntry& a_entry, const int a_level)
{
	RtreeNode* root = m_memory.loadPage(m_memory.m_rootPageID);
	while (root->m_level > a_level)
	{
		int candidate = root->m_entry[0]->m_id;
		float enlargement = root->m_entry[0]->expansion(a_entry);
		for (int i = 1; i < root->m_usedspace; i++)
		{
			float exp = root->m_entry[i]->expansion(a_entry);
			if (exp < enlargement)
			{
				enlargement = exp;
				candidate = root->m_entry[i]->m_id;
			}
		}
		delete root;
		root = m_memory.loadPage(candidate);
	}
	return root;
}

RtreeNode* Rtree::locateLeaf(const RtreeNodeEntry& a_entry)
{
	int result = -1;
	Stack s;
	s.push((void*)m_memory.m_rootPageID);
	while (!s.isEmpty())
	{
		int pageid = (intptr_t)s.pop();
		if (result == -1)
		{
			RtreeNode* n = m_memory.loadPage(pageid);
			if (n->isLeaf())
			{
				for (int i = 0; i < n->m_usedspace; i++)
				{
					if (*n->m_entry[i] == a_entry)
						result = pageid;
				}
			}
			else
			{
				for (int i = 0; i < n->m_usedspace; i++)
				{
					if (n->m_entry[i]->enclose(a_entry))
						s.push((void*)n->m_entry[i]->m_id);
				}
			}
			delete n;
		}
	}
	if (result == -1)
		return 0;
	return (RtreeNode*)m_memory.loadPage(result);
}

int Rtree::remove(const RtreeNodeEntry& a_entry)
{
	class carrier
	{
	public:
		const int       m_level;
		RtreeNodeEntry* m_entry;
	public:
		carrier(const int a_level, RtreeNodeEntry* a_entry) : m_level(a_level), m_entry(a_entry) {};
		virtual ~carrier(){};
	};

	RtreeNode* n = locateLeaf(a_entry);
	if (n == 0) return 0;
	Array a;
	int status = n->remove(a_entry);

	while (!n->isRoot())
	{
		if (status == NODE_UNCHANGED)
		{
			m_memory.writePage(n->m_pageid, n);
			break;
		}

		if (status == NODE_CHANGED)
		{
			RtreeNode* parent = m_memory.loadPage(n->m_parent);
			RtreeNodeEntry* e = n->genNodeEntry();
			status = parent->replace(*e);
			m_memory.writePage(n->m_pageid, n);
			delete e;
			delete n;
			n = parent;
		}
		else if (status == NODE_UNDERFULL)
		{
			RtreeNode* parent = m_memory.loadPage(n->m_parent);
			for (int i = 0; i < n->m_usedspace; i++)
			{
				a.append((void*)new carrier(n->m_level, n->m_entry[i]->clone()));
			}
			RtreeNodeEntry* e = n->genNodeEntry();
			status = parent->remove(*e);
			m_memory.removePage(n->m_pageid);
			delete e;
			delete n;
			n = parent;
		}
	}

	if (n->isRoot())
	{
		if (n->m_usedspace == 1 && !n->isLeaf())
		{
			m_memory.removePage(n->m_pageid);
			m_memory.m_rootPageID = n->m_entry[0]->m_id;
			RtreeNode* child = m_memory.loadPage(n->m_entry[0]->m_id);
			child->m_parent = -1;
			m_memory.writePage(child->m_pageid, child);
			delete child;
		}
		else
		{
			m_memory.writePage(n->m_pageid, n);
		}
	}
	delete n;

	for (int i = 0; i < a.size(); i++)
	{
		carrier* c = (carrier*)a[i];
		insert(*c->m_entry, c->m_level);
		delete c->m_entry;
		delete c;
	}

	return 0;
}

bool Rtree::integrityTest() const
{
	// this function tests the Rtree structure that
	// 1) the entries kept in the parent node matches exactly the child nodes and
	// 2) the level of the node is 1 less than that of its parent
	class carrier
	{
	public:
		const int       m_pageid;   // page id
		const int       m_level;    // node level
		RtreeNodeEntry* m_e;        // entry at the parent
	public:
		carrier(const int a_pageid, const int a_level = 0, RtreeNodeEntry* a_e = 0) : m_pageid(a_pageid), m_level(a_level), m_e(a_e){};
		virtual ~carrier() {};
	};

	bool result = true;
	Stack s;
	s.push(new carrier(m_memory.m_rootPageID));
	while (!s.isEmpty())
	{
		carrier* c = (carrier*)s.pop();
		RtreeNode* n = m_memory.loadPage(c->m_pageid);
		if (c->m_e != 0)
		{
			RtreeNodeEntry* e = n->genNodeEntry();
			if (!(*e == *c->m_e))
				result = false;
			if (n->m_level != c->m_level)
				result = false;
			delete e;
			delete c->m_e;
		}
		for (int i = 0; i < n->m_usedspace && !n->isLeaf(); i++)
			s.push(new carrier(n->m_entry[i]->m_id, n->m_level - 1, n->m_entry[i]->clone()));
		delete n;
		delete c;
	}
	return true;
}

Hash* Rtree::loadObjects() const
{
	Hash* h = new Hash(10000);
	Stack s;
	s.push((void*)m_memory.m_rootPageID);
	while (!s.isEmpty())
	{
		int pageid = (intptr_t)s.pop();
		RtreeNode* n = m_memory.loadPage(pageid);
		if (n->isLeaf())
		for (int i = 0; i < n->m_usedspace; i++)
		{
			RtreeNodeEntry* e = (RtreeNodeEntry*)h->get(n->m_entry[i]->m_id);
			if (e != 0)
			{
				// duplicate
				std::cout << "duplicate insertion of object " << n->m_entry[i]->m_id << std::endl;
			}
			h->put(n->m_entry[i]->m_id, n->m_entry[i]->clone());
		}
		else
		for (int i = 0; i < n->m_usedspace; i++)
		{
			s.push((void*)n->m_entry[i]->m_id);
		}
		delete n;
	}
	return h;
}

float Rtree::nodeVolume(const int a_level) const
{
	float vol = 0;
	Stack s;
	s.push((void*)m_memory.m_rootPageID);
	while (!s.isEmpty())
	{
		int pageid = (intptr_t)s.pop();
		RtreeNode* n = m_memory.loadPage(pageid);
		if (n->m_level > a_level)
		{
			if (n->m_level == a_level + 1)
			for (int i = 0; i < n->m_usedspace; i++)
				vol += n->m_entry[i]->m_hc.volume();
			else
			for (int i = 0; i < n->m_usedspace; i++)
				s.push((void*)n->m_entry[i]->m_id);
		}
		delete n;
	}
	return vol;
}

float Rtree::nodePerimeter(const int a_level) const
{
	float peri = 0;
	Stack s;
	s.push((void*)m_memory.m_rootPageID);
	while (!s.isEmpty())
	{
		int pageid = (intptr_t)s.pop();
		RtreeNode* n = m_memory.loadPage(pageid);
		if (n->m_level > a_level)
		{
			if (n->m_level == a_level + 1)
			for (int i = 0; i<n->m_usedspace; i++)
				peri += n->m_entry[i]->m_hc.perimeter();
			else
			for (int i = 0; i<n->m_usedspace; i++)
				s.push((void*)n->m_entry[i]->m_id);
		}
		delete n;
	}
	return peri;
}

int Rtree::nodeCount(const int a_level) const
{
	int cnt = 0;
	Stack s;
	s.push((void*)m_memory.m_rootPageID);
	while (!s.isEmpty())
	{
		int pageid = (intptr_t)s.pop();
		RtreeNode* n = m_memory.loadPage(pageid);
		if (n->m_level > a_level)
		{
			if (n->m_level == a_level + 1)
				cnt += n->m_usedspace;
			else
			for (int i = 0; i<n->m_usedspace; i++)
				s.push((void*)n->m_entry[i]->m_id);
		}
		delete n;
	}
	return cnt;
}