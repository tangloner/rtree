/* ----------------------------------------------------------------------------
This header file includes class RtreeNode declaration.
It is a component in Rtree and it is a holder of RtreeNodeEntry objects

The user may need to derive a class based on this class for r-tree
variants.
---------------------------------------------------------------------------- */

#ifndef RTREENODE_DEFINED
#define RTREENODE_DEFINED

class Rtree;
class RtreeNodeEntry;

#define NODE_UNCHANGED 10000  // value for node whose content unchanged
#define NODE_CHANGED   10001  // value for node content change
#define NODE_OVERFLOW  10002  // value for node overflow
#define NODE_UNDERFULL 10003  // value for node underfull

class RtreeNode
{
	// data members
public:
	const Rtree*		m_rtree;		// reference to Rtree
	int					m_pageid;		// page ID of the node
	int					m_level;		// level in the tree (0:leaf)
	int					m_parent;		// page ID of its parent node
	int					m_usedspace;	// number of entries
	RtreeNodeEntry**	m_entry;		// an array of entries

	// methods
public:
	// constructor/destructor
	RtreeNode(const Rtree* a_tree, const int a_pageid, const int a_level = 0, const int a_parent = -1);
	RtreeNode(const int a_pageid, const int a_level = 0, const int a_parent = -1);

	virtual ~RtreeNode();
	virtual RtreeNode* clone() const;

	//search function
	virtual bool isRoot() const;
	virtual bool isLeaf() const;
	virtual RtreeNodeEntry* genNodeEntry() const;

	//update
	virtual int insert(const RtreeNodeEntry& a_entry);

	virtual void quadraticSplit(RtreeNode** a_p0, RtreeNode** a_p1);
	virtual int replace(const RtreeNodeEntry& a_entry);
	virtual int remove(const RtreeNodeEntry& a_entry);

	//memory operations
	virtual int toMem(char* a_content, int& a_len) const;
	static RtreeNode* fromMem(const char* a_p, int& a_len, const Rtree* a_rtree, RtreeNodeEntry* (*fromMem)(const char* a_mem, int& a_len, const int a_dimen, const bool a_pt));

	//info
	static int size();
};

#endif // defined RTREENODE_DEFINED