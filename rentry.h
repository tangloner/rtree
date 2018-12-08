/* ----------------------------------------------------------------------------
This header file includes class RtreeNodeEntry declaration.
It is a component in RtreeNode and it is a holder of IDs of child/objects
and corresponding MBBs.

The user may need to derive a class based on this class for r-tree
variants.
---------------------------------------------------------------------------- */


#ifndef RTREENODEENTRY_DEFINED
#define RTREENODEENTRY_DEFINED

#include "header.h"
#include "hypercube.h"

class RtreeNodeEntry
{
	//data member
public:
	const int m_id;  // page id;
	Hypercube m_hc;  // mbr
	int num_records; // the total number of records

	// methods
public:
	// constructor/destructor
	RtreeNodeEntry(const int a_id, const Hypercube& a_hc);
	RtreeNodeEntry(const int a_id, const Hypercube& a_hc, const int num_nodes); //deleted some!
	virtual ~RtreeNodeEntry();
	virtual RtreeNodeEntry* clone() const;


	//comparison
	virtual bool operator==(const RtreeNodeEntry& a_entry) const;
	virtual bool enclose(const RtreeNodeEntry& a_entry) const;

	//measures
	virtual float expansion(const RtreeNodeEntry& a_entry) const;

	//update
	// quadratic cost split SIGMOD84
	virtual int quadraticSplit(RtreeNodeEntry** a_entry, const int a_len, const int a_split, RtreeNodeEntry** a_gp0, int& a_gp0cnt, RtreeNodeEntry** a_gp1, int& a_gp1cnt);
	//// R*tree split algorithm VLDB90
	//virtual int goodnessSplit(RtreeNodeEntry** a_entry, const int a_len, const int a_split, RtreeNodeEntry** a_gp0, int& a_gp0cnt, RtreeNodeEntry** a_gp1, int& a_gp1cnt);
	
	// split algorithm for TGS
	virtual int packedSplit(RtreeNodeEntry** a_entry, const int a_len, const int a_bulk, RtreeNodeEntry** a_gp0, int& a_gp0cnt, RtreeNodeEntry** a_gp1, int& a_gp1cnt);

	//// pick out entries for R*tree reintsertion
	//virtual int pickWorst(RtreeNodeEntry** a_entry, const int a_len, const int a_reinsert, RtreeNodeEntry** a_gp0, int& a_gp0cnt, RtreeNodeEntry** a_gp1, int& a_gp1cnt);

	// merge a number of entries into one
	virtual int combine(RtreeNodeEntry** a_entry, const int a_len, const int a_id, RtreeNodeEntry** a_res);

	//sort a number of entries along dimen
	virtual int sortOnDimen(RtreeNodeEntry** a_entry, const int a_len, const int a_dimen);

	// memory operations
	virtual int toMem(char* a_content, int& a_len, const bool a_pt) const;

	// to convert a bytestring to a RtreeNodeEntry
	static RtreeNodeEntry* fromMem(const char* a_p, int& a_len, const int a_dimen, const bool a_pt);

	// info: storage size
	static int size(const int a_dimen, bool isPoint = false);
};

#endif // RTREENODEENTRY_DEFINED
