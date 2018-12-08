/* ----------------------------------------------------------------------------
This header file declares Rtree class.

The user may need to derive from this class for Rtree variants
---------------------------------------------------------------------------- */

#ifndef RTREE_DEFINED
#define RTREE_DEFINED

class Memory;
class RtreeNode;
class RtreeNodeEntry;
#include "header.h"

class Rtree
{
	//data member
public:
	Memory&			m_memory;			//	ref. to mem that store this rtree
	const int		m_dimen;			//	no. of dimensions
	const int		m_maxNodeChild;		//  max. number of entries in a node
	const int		m_minNodeChild;		//	min. number of entries in a node
	const int		m_maxLeafChild;		//  max. number of entries in a node
	const int		m_minLeafChild;		//	min. number of entries in a node
	bool			m_pointOnly;		//  indicate if the Rtree stores only points

	// methods

public:
	// constructor / destructor
	Rtree(Memory& a_memory, const int a_dimen, const int a_maxNodefill, const int a_maxLeafFill, const int a_minNodeFill, const int a_minLeafFill, const bool a_pointOnly = false);
	virtual ~Rtree();

	// update function
	int insert(const RtreeNodeEntry& a_entry, const int a_level = 0);	// add an entry at Rtree at a specified level (default:0)
	int remove(const RtreeNodeEntry& a_entry);							// del an entry from Rtree

	// test function
	bool integrityTest() const;								// test rtree integrity
	Hash* loadObjects() const;				     			// test if all objects are stored

	float nodeVolume(const int a_level) const;				//	find the vol. of nodes at level "a_level"
	float nodePerimeter(const int a_level) const;			//	find the peri. of nodes at level "a_level"
	int nodeCount(const int a_level) const;					//  find the no. of nodes at level "a_level"


protected:
	RtreeNode* chooseLeaf(const RtreeNodeEntry& a_entry, const int a_level);    // find leaf node of Rtree to insert an entry
	RtreeNode* locateLeaf(const RtreeNodeEntry& a_entry);						// find a leaf that contains a_entry
};

#endif // defined RTREE