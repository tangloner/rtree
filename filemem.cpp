#include "filemem.h"
#include "rnode.h"
#include "rentry.h"

typedef struct
{
	int m_rootpage;
	int m_freelist;
	int m_largest;
}Header;



FileMemory::FileMemory(const int a_pagesize, const char* filename, RtreeNodeEntry* (*a_fromMem)(const char* a_mem, int& a_len, const int a_dimen, const bool a_pt), const bool a_new) :
Memory(a_pagesize), m_fromMem(a_fromMem), m_freelist(-1), m_largest(0)
{
	if (a_new)
	{
		m_memfile.open(filename, fstream::in | fstream::out | fstream::binary | fstream::trunc);
		flush();
	}
	else
	{
		m_memfile.open(filename, fstream::in | fstream::out | fstream::binary);
		m_memfile.seekg(0);
		Header hdr;
		m_memfile.read((char*)&hdr, sizeof(hdr));
		m_rootPageID = hdr.m_rootpage;
		m_freelist = hdr.m_freelist;
		m_largest = hdr.m_largest;
	}
	m_buffer = new char[a_pagesize];
}

FileMemory::~FileMemory()
{
	m_memfile.seekg(0);
	Header hdr;
	hdr.m_rootpage = m_rootPageID;
	hdr.m_freelist = m_freelist;
	hdr.m_largest = m_largest;
	m_memfile.write((char*)&hdr, sizeof(hdr));
	m_memfile.close();
	delete m_buffer;
}

RtreeNode* FileMemory::loadPage(const int a_pageID)
{
	int len = 0;
	m_memfile.seekg(a_pageID * m_pagesize + sizeof(Header));
	m_memfile.read(m_buffer, m_pagesize);
	RtreeNode* n = RtreeNode::fromMem(m_buffer, len, m_rtree, m_fromMem);
	return n;
}

int FileMemory::allocatePage()
{
	if (m_freelist != -1)
	{
		int res = m_freelist;
		int next = 0;
		m_memfile.seekg(m_freelist * m_pagesize + sizeof(Header));
		m_memfile.read((char*)&m_freelist, sizeof(m_freelist));
		return res;
	}
	return m_largest++;
}

void FileMemory::writePage(const int a_pageID, const RtreeNode* m_p)
{
	int len = 0;
	m_p->toMem(m_buffer, len);
	m_memfile.seekg(a_pageID * m_pagesize + sizeof(Header));
	m_memfile.write(m_buffer, m_pagesize);
}

void FileMemory::removePage(const int a_pageID)
{
	m_memfile.seekg(a_pageID * m_pagesize + sizeof(Header));
	m_memfile.write((char*)&m_freelist, sizeof(m_freelist));
	m_freelist = a_pageID;
}

void FileMemory::flush()
{
	m_memfile.seekg(0);
	Header hdr;
	hdr.m_rootpage = m_rootPageID = -1;
	hdr.m_freelist = m_freelist = -1;
	hdr.m_largest = m_largest = 0;
	m_memfile.write((char*)&hdr, sizeof(hdr));
}
