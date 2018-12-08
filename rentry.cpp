#include "header.h"
#include "rentry.h"

RtreeNodeEntry::RtreeNodeEntry(const int a_id, const Hypercube& a_hc) :m_id(a_id), m_hc(a_hc){ }

RtreeNodeEntry::RtreeNodeEntry(const int a_id, const Hypercube& a_hc, const int num_nodes) : m_id(a_id), m_hc(a_hc), num_records(num_nodes){ }

RtreeNodeEntry::~RtreeNodeEntry(){};

RtreeNodeEntry* RtreeNodeEntry::clone() const
{
	return new RtreeNodeEntry(m_id, m_hc);
}

bool RtreeNodeEntry::operator==(const RtreeNodeEntry& a_entry) const
{
	return m_id == a_entry.m_id && m_hc == a_entry.m_hc;
}

bool RtreeNodeEntry::enclose(const RtreeNodeEntry& a_entry) const
{
	return m_hc.enclose(a_entry.m_hc);
}

// merge a number of entries into one
int RtreeNodeEntry::combine(RtreeNodeEntry** a_entry, const int a_len, const int a_id, RtreeNodeEntry** a_res)
{
	int dimen = a_entry[0]->m_hc.dimen();
	float cl[MAXDIMEN], cu[MAXDIMEN];

	for (int d = 0; d < dimen; d++)
	{
		cl[d] = a_entry[0]->m_hc.getLower()[d];
		cu[d] = a_entry[0]->m_hc.getUpper()[d];
	}

	for (int i = 0; i < a_len; i++)
	{
		for (int d = 0; d < dimen; d++)
		{
			cl[d] = cl[d] < a_entry[i]->m_hc.getLower()[d] ? cl[d] : a_entry[i]->m_hc.getLower()[d];
			cu[d] = cu[d] > a_entry[i]->m_hc.getUpper()[d] ? cu[d] : a_entry[i]->m_hc.getUpper()[d];
		}
	}

	Hypercube hc(dimen, cl, cu);
	(*a_res) = new RtreeNodeEntry(a_id, hc);
	return a_len;
}

int RtreeNodeEntry::quadraticSplit(RtreeNodeEntry** a_entry, const int a_len, const int a_split, RtreeNodeEntry** a_gp0, int& a_gp0cnt, RtreeNodeEntry** a_gp1, int& a_gp1cnt)
{
	class carrier
	{
	public:
		const int			m_dimen;
		RtreeNodeEntry*		m_p;
	public:
		carrier(const int a_dimen, RtreeNodeEntry* a_p) :m_dimen(a_dimen), m_p(a_p){};
		~carrier(){};
		static int compareL(const void* a_p0, const void* a_p1)
		{
			carrier* c0 = *(carrier**)a_p0;
			carrier* c1 = *(carrier**)a_p1;
			float val0 = c0->m_p->m_hc.getLower()[c0->m_dimen];
			float val1 = c1->m_p->m_hc.getLower()[c1->m_dimen];
			if (val0 < val1) return -1;
			if (val0 > val1) return +1;
			return 0;
		}

		static int compareU(const void* a_p0, const void* a_p1)
		{
			carrier* c0 = *(carrier**)a_p0;
			carrier* c1 = *(carrier**)a_p1;
			float val0 = c0->m_p->m_hc.getUpper()[c0->m_dimen];
			float val1 = c1->m_p->m_hc.getUpper()[c1->m_dimen];
			if (val0 < val1) return -1;
			if (val0 > val1) return +1;
			return 0;
		}
	};

	int dimen = m_hc.dimen();
	float totalarea = -1;
	for (int d = 0; d < dimen; d++)
	{
		Collection::Array listL;
		Collection::Array listU;

		for (int i = 0; i < a_len; i++)
		{
			carrier* c = new carrier(d, a_entry[i]);
			listL.append(c);
			listU.append(c);
		}

		listL.sort(carrier::compareL);
		listU.sort(carrier::compareU);

		Hypercube seed0 = ((carrier*)listL[a_len - 1])->m_p->m_hc;
		Hypercube seed1 = ((carrier*)listU[0])->m_p->m_hc;

		int cnt0 = 0, cnt1 = 0;
		RtreeNodeEntry** tmp0 = new RtreeNodeEntry*[a_len];
		RtreeNodeEntry** tmp1 = new RtreeNodeEntry*[a_len];

		for (int i = 0; i < a_len; i++)
		{
			Hypercube test0 = Hypercube::combine(seed0, a_entry[i]->m_hc);
			Hypercube test1 = Hypercube::combine(seed1, a_entry[i]->m_hc);

			if (cnt0 > a_len - a_split)
				tmp1[cnt1++] = a_entry[i];
			else if (cnt1 > a_len - a_split)
				tmp0[cnt0++] = a_entry[i];
			else
			{
				if ((test0.volume() - seed0.volume()) < (test1.volume() - seed1.volume()))
				{
					seed0 = test0;
					tmp0[cnt0++] = a_entry[i];
				}
				else
				{
					seed1 = test1;
					tmp1[cnt1++] = a_entry[i];
				}
			}
		}
		float area = seed0.volume() + seed1.volume();
		if (area < totalarea || totalarea == -1)
		{
			a_gp0cnt = a_gp1cnt = 0;
			for (int i = 0; i < cnt0; i++)
			{
				a_gp0[a_gp0cnt++] = tmp0[i];
			}
			for (int i = 0; i < cnt1; i++)
			{
				a_gp1[a_gp1cnt++] = tmp1[i];
			}
			totalarea = area;
		}

		delete[] tmp0;
		delete[] tmp1;

		for (int i = 0; i < a_len; i++)
		{
			delete (carrier*)listL[i];
		}

		listL.clean();
		listU.clean();
	}
	return 0;
}

float RtreeNodeEntry::expansion(const RtreeNodeEntry& a_entry) const
{
	Hypercube hc = Hypercube::combine(m_hc, a_entry.m_hc);
	return hc.volume() - m_hc.volume();
}

//memory operation
int RtreeNodeEntry::toMem(char* a_content, int& a_len, const bool a_pt) const
{
	int init = a_len;
	memcpy(&a_content[a_len], &m_id, sizeof(m_id));
	a_len += sizeof(m_id);
	memcpy(&a_content[a_len], &num_records, sizeof(m_id));
	a_len += sizeof(num_records);

	for (int i = 0; i < m_hc.dimen(); i++)
	{
		float l = m_hc.getLower()[i];
		float u = m_hc.getUpper()[i];

		memcpy(&a_content[a_len], &l, sizeof(l));
		a_len += sizeof(l);
		if (!a_pt)
		{
			memcpy(&a_content[a_len], &u, sizeof(u));
			a_len += sizeof(u);
		}
	}
	return a_len - init;
}

RtreeNodeEntry* RtreeNodeEntry::fromMem(const char* a_content, int& a_len, const int dimen, const bool a_pt)
{
	int id;
	memcpy(&id, &a_content[a_len], sizeof(id));
	a_len += sizeof(id);
	int num_nodes;
	memcpy(&num_nodes, &a_content[a_len], sizeof(num_nodes));
	a_len += sizeof(id);

	float cl[MAXDIMEN];
	float cu[MAXDIMEN];

	for (int i = 0; i < dimen; i++)
	{
		memcpy(&cl[i], &a_content[a_len], sizeof(cl[i]));
		a_len += sizeof(cl[i]);

		cu[i] = cl[i];
		if (!a_pt)
		{
			memcpy(&cu[i], &a_content[a_len], sizeof(cu[i]));
			a_len += sizeof(cu[i]);
		}
	}
	Hypercube hc(dimen, cl, cu);
	return new RtreeNodeEntry(id, hc, num_nodes);
}

int RtreeNodeEntry::size(const int a_dimen, bool isPoint)
{
	if (isPoint)
		return sizeof(int)+Hypercube::size(a_dimen) / 2;
	//return sizeof(int)+Hypercube::size(a_dimen);

	return sizeof(int)+Hypercube::size(a_dimen) + sizeof(int);
}

int dimenToSort = 0;
int compareDimen(const void* a0, const void* a1)
{
	RtreeNodeEntry* r0 = *(RtreeNodeEntry**)a0;
	RtreeNodeEntry* r1 = *(RtreeNodeEntry**)a1;
	float v0 = r0->m_hc.getLower()[dimenToSort];
	float v1 = r1->m_hc.getLower()[dimenToSort];
	if (v0 < v1) return -1;
	if (v0 > v1) return +1;
	return 0;
}

int RtreeNodeEntry::sortOnDimen(RtreeNodeEntry** a_entry, const int a_len, const int a_dimen)
{
	dimenToSort = a_dimen;
	qsort(a_entry, a_len, sizeof(RtreeNodeEntry*), compareDimen);
	return a_len;
}

int RtreeNodeEntry::packedSplit(RtreeNodeEntry** a_entry, const int a_len, const int a_bulk, RtreeNodeEntry** a_gp0, int& a_gp0cnt, RtreeNodeEntry** a_gp1, int& a_gp1cnt)
{
	int dimen = m_hc.dimen();
	float bestvol = -1;
	int bestcut = -1;
	int bestdim = -1;
	for (int d = 0; d < dimen; d++)
	{
		sortOnDimen(a_entry, a_len, d);
		int piece = a_len % a_bulk == 0 ? a_len / a_bulk : a_len / a_bulk + 1;
		int splitpt = piece / 2 * a_bulk;

		RtreeNodeEntry* r0;
		RtreeNodeEntry* r1;
		combine(&a_entry[0], splitpt, 0, &r0);
		combine(&a_entry[splitpt], a_len - splitpt, 1, &r1);

		float vol = r0->m_hc.volume() + r1->m_hc.volume();
		if (vol < bestvol || bestcut == -1)
		{
			bestdim = d;
			bestvol = vol;
			bestcut = splitpt;
		}
		delete r0;
		delete r1;

	}

	if (bestdim != dimen - 1)
		sortOnDimen(a_entry, a_len, bestdim);
	a_gp0cnt = a_gp1cnt = 0;
	for (int i = 0; i < a_len; i++)
	{
		if (i < bestcut)
			a_gp0[a_gp0cnt++] = a_entry[i];
		else
			a_gp1[a_gp1cnt++] = a_entry[i];
	}
	return 0;
}
