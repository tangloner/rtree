/* ----------------------------------------------------------------------------
This header file includes class FileMemory declaration.
It stores R-tree data in a file.

Description of file structures.
The file is block based.
The first block is a header consisting of
1. the id of the root page ("rootPageID")
2. the head of the free page list ("freelist")
3. the largest page number used ("largest").
After the first block, the subsequent blocks are either
1. data block, (containing one RtreeNode) or
2. free block (available to reuse, containing a pointer to the "next"
free block)
the head of free block chain is pointed by "freelist"
In case that no free block available, largest indicate the largest
page id used. If a new page is allocated, the largest is incremented.
---------------------------------------------------------------------------- */


#ifndef FILEMEMORY_DEFINED
#define FILEMEMORY_DEFINED

#include "header.h"
#include "mem.h"

class RtreeNodeEntry;

using namespace std;

class FileMemory :public Memory
{
public:
	fstream		m_memfile;
	int			m_freelist;
	int			m_largest;
	char*		m_buffer;

	RtreeNodeEntry* (*m_fromMem)(const char* a_p, int& a_len, const int a_dimen, const bool a_pt); // to convert a bytestring to a RtreeNodeEntry

public:
	FileMemory(const int a_pagesize, const char* filename, RtreeNodeEntry* (*a_fromMem)(const char* a_mem, int& a_len, const int a_dimen, const bool a_pt), const bool a_new = false);
	virtual ~FileMemory();

	//search
	virtual RtreeNode* loadPage(const int a_pageID); // read a node (page) from a file
	//update
	virtual int allocatePage(); // obtain ID of an unused page
	virtual void writePage(const int a_pageID, const RtreeNode* m_p); // write a page to a file
	virtual void removePage(const int a_pageID); // remove a page from a file
	virtual void flush(); // clean the memory
};

#endif //FILEMEMORY_DEFINED