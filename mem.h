/* ----------------------------------------------------------------------------
This header file includes abstract class Memory declaration.
It is only an interface to store R-tree data.

The user needs to derive a class based on this class for real storage.
---------------------------------------------------------------------------- */

#ifndef MEM_DEFINED
#define MEM_DEFINED

class Rtree;
class RtreeNode;

class Memory
{
	// data members
public:
	Rtree*			m_rtree;		//	rtree;
	int				m_rootPageID;	//	root page;
	const int		m_pagesize;		//	page size;

public:
	Memory(const int a_pagesize) :m_rootPageID(-1), m_pagesize(a_pagesize){};
	virtual ~Memory() {};

	virtual RtreeNode* loadPage(const int a_pageID) = 0;

	virtual int allocatePage() = 0;
	virtual void writePage(const int a_pageID, const RtreeNode* m_p) = 0;
	virtual void removePage(const int a_pageID) = 0;
	virtual void flush() = 0;
};

#endif // defined abstarct memory class