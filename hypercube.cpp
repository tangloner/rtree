#include "header.h"
#include "hypercube.h"

//constructor/destructor
Hypercube::Hypercube(const int a_dimen, const float* a_l, const float* a_u) : m_lower(a_dimen, a_l), m_upper(a_dimen, a_u)
{}

Hypercube::Hypercube(const Point& a_lower, const Point& a_upper) : m_lower(a_lower), m_upper(a_upper)
{}

Hypercube::Hypercube(const Hypercube& a_hc) : m_lower(a_hc.m_lower), m_upper(a_hc.m_upper)
{}

Hypercube::~Hypercube()
{}

// operation
Hypercube& Hypercube::operator=(const Hypercube& a_hc)
{
	m_lower = a_hc.getLower();
	m_upper = a_hc.getUpper();
	return *this;
}

// search
const Point& Hypercube::getLower() const
{
	return m_lower;
}

const Point& Hypercube::getUpper() const
{
	return m_upper;
}

Point Hypercube::getCenter() const
{
	int dim = dimen();
	float c[MAXDIMEN];
	for (int i = 0; i < dim; i++)
		c[i] = (m_lower[i] + m_upper[i]) / 2;
	Point pt(dim, c);
	return pt;
}

int Hypercube::dimen() const
{
	return m_lower.m_dimen;
}

float Hypercube::volume() const
{
	float size = 1;
	int dimen = m_lower.m_dimen;
	for (int d = 0; d < dimen; d++)
		size *= (m_upper[d] - m_lower[d]);
	return size;
}

float Hypercube::perimeter() const
{
	float peri = 0;
	int dimen = m_lower.m_dimen;
	for (int d = 0; d < dimen; d++)
		peri += float((m_upper[d] - m_lower[d]) * pow(2.0, dimen - 1));
	return peri;
}

// measures
float Hypercube::mindist(const Hypercube& a_hc) const
{
	float mindist = 0;
	int dimen = m_lower.m_dimen;
	for (int d = 0; d<dimen; d++)
	{
		float dist = 0;
		if (m_lower[d] > a_hc.m_upper[d])
			dist = m_lower[d] - a_hc.m_upper[d];
		else if (m_upper[d] < a_hc.m_lower[d])
			dist = a_hc.m_lower[d] - m_upper[d];
		mindist += dist * dist;
	}
	return (float)sqrt(mindist);
}

float Hypercube::maxdist(const Hypercube& a_hc) const
{
	float maxdist = 0;
	int dimen = m_lower.m_dimen;
	for (int d = 0; d<dimen; d++)
	{
		float ldist = a_hc.m_upper[d] - m_lower[d];
		ldist = ldist * ldist;
		float rdist = m_upper[d] - a_hc.m_lower[d];
		rdist = rdist * rdist;
		maxdist += ldist > rdist ? ldist : rdist;
	}
	return (float)sqrt(maxdist);
}

float Hypercube::mindist(const Point& a_pt) const
{
	float mindist = 0;
	int dimen = m_lower.m_dimen;
	for (int d = 0; d<dimen; d++)
	{
		float dist = 0;
		if (m_lower[d] > a_pt[d])
			dist = m_lower[d] - a_pt[d];
		else if (m_upper[d] < a_pt[d])
			dist = a_pt[d] - m_upper[d];
		mindist += dist * dist;
	}
	return (float)sqrt(mindist);
}

float Hypercube::maxdist(const Point& a_pt) const
{
	float maxdist = 0;
	int dimen = m_lower.m_dimen;
	for (int d = 0; d<dimen; d++)
	{
		float ldist = a_pt[d] - m_lower[d];
		ldist = ldist * ldist;
		float rdist = m_upper[d] - a_pt[d];
		rdist = rdist * rdist;
		maxdist += ldist > rdist ? ldist : rdist;
	}
	return (float)sqrt(maxdist);
}

float Hypercube::mindist(const Point& a_pt, const int a_dimen) const
{
	if (m_lower[a_dimen] > a_pt[a_dimen])
		return m_lower[a_dimen] - a_pt[a_dimen];
	else if (m_upper[a_dimen] < a_pt[a_dimen])
		return a_pt[a_dimen] - m_upper[a_dimen];
	return 0;
}

float Hypercube::diagdist() const
{
	float ddist = 0;
	int dimen = m_lower.m_dimen;
	for (int d = 0; d < dimen; d++)
	{
		float dist = m_upper[d] - m_lower[d];
		dist = dist * dist;
		ddist += dist;
	}
	return (float)sqrt(ddist);
}

// comparison
bool Hypercube::operator==(const Hypercube& a_hc) const
{
	return (m_lower == a_hc.m_lower && m_upper == a_hc.m_upper);
}

bool Hypercube::enclose(const Hypercube& a_hc) const
{
	int dimen = m_lower.m_dimen;
	for (int d = 0; d < dimen; d++)
	{
		if (a_hc.m_lower[d] < m_lower[d]) return false;
		if (a_hc.m_upper[d] > m_upper[d]) return false;
	}
	return true;
}

bool Hypercube::enclose(const Point& a_pt) const
{
	int dimen = m_lower.m_dimen;
	for (int d = 0; d < dimen; d++)
	{
		if (a_pt[d] <= m_lower[d]) return false;
		if (a_pt[d] >= m_upper[d]) return false;
	}
	return true;
}

bool Hypercube::isIntersected(const Hypercube& a_hc0, const Hypercube& a_hc1) const    //if the two hypercude intersects
{
	int dimen = a_hc0.m_lower.m_dimen;
	Hypercube hc = intersect(a_hc0, a_hc1);

	if (hc == a_hc1)       //current query rectangle a_hc1 is completely enclosed by a_hc0
		return false;

	for (int d = 0; d < dimen; d++)
	{
		if (hc.m_lower[d] == 0 && hc.m_upper[d] == 0)
			return false;
	}

	return true;
}

bool Hypercube::atRight(const Hypercube& a_hc0, const Hypercube& a_hc1) const   //if hc0 interects hc1 at right
{
	int dimen = a_hc0.m_lower.m_dimen;
	if ((a_hc0.m_lower[0] <= a_hc1.m_upper[0]) && (a_hc0.m_upper[0] > a_hc1.m_upper[0]))
	{
		if ((a_hc0.m_lower[1] >= a_hc1.m_upper[1]) || (a_hc0.m_upper[1] <= a_hc1.m_lower[1]))
			return false;
		else
			return true;
	}
	else
		return false;
}

bool Hypercube::atAbove(const Hypercube& a_hc0, const Hypercube& a_hc1) const   //if hc0 interects hc1 at above
{
	if ((a_hc0.m_lower[1] <= a_hc1.m_upper[1]) && (a_hc0.m_upper[1] > a_hc1.m_upper[1]))
	{
		if ((a_hc0.m_upper[0] <= a_hc1.m_lower[0]) || (a_hc0.m_lower[0] >= a_hc1.m_upper[0]))
			return false;
		else
			return true;
	}
	else
		return false;
}

bool Hypercube::atLeft(const Hypercube& a_hc0, const Hypercube& a_hc1) const   //if hc0 interects hc1 at left
{
	if ((a_hc0.m_lower[0] < a_hc1.m_lower[0]) && (a_hc0.m_upper[0] >= a_hc1.m_lower[0]))
	{
		if ((a_hc0.m_lower[1] >= a_hc1.m_upper[1]) || (a_hc0.m_upper[1] <= a_hc1.m_lower[1]))
			return false;
		else
			return true;
	}
	else
		return false;
}

bool Hypercube::atBelow(const Hypercube& a_hc0, const Hypercube& a_hc1) const   //if hc0 interects hc1 at below
{
	if ((a_hc0.m_lower[1] < a_hc1.m_lower[1]) && (a_hc0.m_upper[1] >= a_hc1.m_lower[1]))
	{
		if ((a_hc0.m_upper[0] <= a_hc1.m_lower[0]) || (a_hc0.m_lower[0] >= a_hc1.m_upper[0]))
			return false;
		else
			return true;
	}
	else
		return false;
}

int  Hypercube::EncloseWhichQuadrant(const Hypercube& a_hc0, const Hypercube& a_hc1, int qd_ID[]) const//which quadrant of hc1 is enclosed by hc0
{
	//intersect (or enclose) the NE quadrant
	if ((a_hc0.m_lower[0] <= a_hc1.m_upper[0]) && (a_hc0.m_upper[0] >= a_hc1.m_upper[0])
		&& (a_hc0.m_lower[1] <= a_hc1.m_upper[1]) && (a_hc0.m_upper[1] >= a_hc1.m_upper[1])
		&& (a_hc0.m_upper[1] > a_hc1.m_upper[1]) && (a_hc0.m_upper[0] > a_hc1.m_upper[0]))
		qd_ID[1] = 1;

	//intersect (or enclose) the Nw quadrant
	if ((a_hc0.m_lower[0] <= a_hc1.m_lower[0]) && (a_hc0.m_upper[0] >= a_hc1.m_lower[0])
		&& (a_hc0.m_lower[1] <= a_hc1.m_upper[1]) && (a_hc0.m_upper[1] >= a_hc1.m_upper[1])
		&& (a_hc0.m_upper[1] > a_hc1.m_upper[1]) && (a_hc0.m_lower[0] < a_hc1.m_lower[0]))
		qd_ID[2] = 1;

	//intersect (or enclose) the SW quadrant
	if ((a_hc0.m_lower[0] <= a_hc1.m_lower[0]) && (a_hc0.m_upper[0] >= a_hc1.m_lower[0])
		&& (a_hc0.m_lower[1] <= a_hc1.m_lower[1]) && (a_hc0.m_upper[1] >= a_hc1.m_lower[1])
		&& (a_hc0.m_lower[1] < a_hc1.m_lower[1]) && (a_hc0.m_lower[0] < a_hc1.m_lower[0]))
		qd_ID[3] = 1;

	//intersect (or enclose) the SE quadrant
	if ((a_hc0.m_lower[0] <= a_hc1.m_upper[0]) && (a_hc0.m_upper[0] >= a_hc1.m_upper[0])
		&& (a_hc0.m_lower[1] <= a_hc1.m_lower[1]) && (a_hc0.m_upper[1] >= a_hc1.m_lower[1])
		&& (a_hc0.m_lower[1]<a_hc1.m_lower[1]) && (a_hc0.m_upper[0]>a_hc1.m_upper[0]))
		qd_ID[4] = 1;

	return 1;
}

int Hypercube::inWhichStripe(const Hypercube& a_hc0, const Hypercube& a_hc1, int stripe[]) const //a_hc0 falls in which stripe of a_hc1
{
	//*
	if (a_hc0.m_lower[0] == a_hc0.m_upper[0])  //current MBR is a vertical line
	{
		if (a_hc0.m_lower[0] > a_hc1.m_upper[0] && (!(a_hc0.m_lower[1] >= a_hc1.m_upper[1]))    //the right-side 
			&& (!(a_hc0.m_upper[1] <= a_hc1.m_lower[1])))
		{
			stripe[0] = 1;
		}

		if (a_hc0.m_lower[1] >= a_hc1.m_upper[1] && a_hc0.m_lower[0] >= a_hc1.m_lower[0] && a_hc0.m_upper[0] <= a_hc1.m_upper[0])    //the up-side 
		{
			stripe[1] = 1;
		}

		if (a_hc0.m_upper[0] < a_hc1.m_lower[0] && (!(a_hc0.m_lower[1] >= a_hc1.m_upper[1]))    //the left-side 
			&& (!(a_hc0.m_upper[1] <= a_hc1.m_lower[1])))
		{
			stripe[2] = 1;
		}

		if (a_hc0.m_upper[1] <= a_hc1.m_lower[1] && a_hc0.m_lower[0] >= a_hc1.m_lower[0] && a_hc0.m_upper[0] <= a_hc1.m_upper[0])    //the down-side 
		{
			stripe[3] = 1;
		}
	}
	else if (a_hc0.m_lower[1] == a_hc0.m_upper[1])   //current MBR is a horizontal line
	{
		if (a_hc0.m_lower[0] >= a_hc1.m_upper[0] && a_hc0.m_lower[1] >= a_hc1.m_lower[1] && a_hc0.m_upper[1] <= a_hc1.m_upper[1])    //the right-side 
		{
			stripe[0] = 1;
		}

		if (a_hc0.m_lower[1] > a_hc1.m_upper[1] && (!(a_hc0.m_lower[0] >= a_hc1.m_upper[0]))    //the up-side 
			&& (!(a_hc0.m_upper[0] <= a_hc1.m_lower[0])))
		{
			stripe[1] = 1;
		}

		if (a_hc0.m_upper[0] <= a_hc1.m_lower[0] && a_hc0.m_lower[1] >= a_hc1.m_lower[1] && a_hc0.m_upper[1] <= a_hc1.m_upper[1])    //the left-side 
		{
			stripe[2] = 1;
		}

		if (a_hc0.m_upper[1]<a_hc1.m_lower[1] && (!(a_hc0.m_lower[0] >= a_hc1.m_upper[0]))    //the down-side 
			&& (!(a_hc0.m_upper[0] <= a_hc1.m_lower[0])))
		{
			stripe[3] = 1;
		}
	}
	else
	{
		if (a_hc0.m_lower[0]>a_hc1.m_upper[0] && (!(a_hc0.m_lower[1] >= a_hc1.m_upper[1]))    //the right-side 
			&& (!(a_hc0.m_upper[1] <= a_hc1.m_lower[1])))
		{
			stripe[0] = 1;
		}

		if (a_hc0.m_lower[1] > a_hc1.m_upper[1] && (!(a_hc0.m_lower[0] >= a_hc1.m_upper[0]))    //the up-side 
			&& (!(a_hc0.m_upper[0] <= a_hc1.m_lower[0])))
		{
			stripe[1] = 1;
		}

		if (a_hc0.m_upper[0] < a_hc1.m_lower[0] && (!(a_hc0.m_lower[1] >= a_hc1.m_upper[1]))    //the left-side 
			&& (!(a_hc0.m_upper[1] <= a_hc1.m_lower[1])))
		{
			stripe[2] = 1;
		}

		if (a_hc0.m_upper[1] < a_hc1.m_lower[1] && (!(a_hc0.m_lower[0] >= a_hc1.m_upper[0]))    //the down-side 
			&& (!(a_hc0.m_upper[0] <= a_hc1.m_lower[0])))
		{
			stripe[3] = 1;
		}

		if ((a_hc0.m_lower[0] >= a_hc1.m_lower[0]) && (a_hc0.m_upper[0] <= a_hc1.m_upper[0])    //a_hc0 is enclosed by a_hc1
			&& (a_hc0.m_lower[1] >= a_hc1.m_lower[1]) && (a_hc0.m_upper[1] <= a_hc1.m_upper[1]))
		{
			stripe[4] = 1;
		}
	}
	return 0;
}

int Hypercube::inWhichQuadrant(const Hypercube& a_hc0, const Hypercube& a_hc1, int quadrant[]) const//a_hc0 falls in which quadrant of a_hc1
{
	if ((a_hc0.m_upper[0] > a_hc1.m_upper[0]) && (a_hc0.m_upper[1] > a_hc1.m_upper[1]))
		quadrant[0] = 1;         //hc0 falls in (or intersects) the northeast quadrant

	if ((a_hc0.m_lower[0]<a_hc1.m_lower[0]) && (a_hc0.m_upper[1]>a_hc1.m_upper[1]))
		quadrant[1] = 1;         //hc0 falls in (or intersects) the northwest quadrant

	if ((a_hc0.m_lower[0] < a_hc1.m_lower[0]) && (a_hc0.m_lower[1]<a_hc1.m_lower[1]))
		quadrant[2] = 1;         //hc0 falls in (or intersects) the southwest quadrant

	if ((a_hc0.m_upper[0]>a_hc1.m_upper[0]) && (a_hc0.m_lower[1] < a_hc1.m_lower[1]))
		quadrant[3] = 1;         //hc0 falls in (or intersects) the southeast quadrant

	return 0;
}

// operations
Hypercube Hypercube::combine(const Hypercube& a_hc0, const Hypercube& a_hc1)
{
	float cl[MAXDIMEN];
	float cu[MAXDIMEN];
	int dimen = a_hc0.m_lower.m_dimen;
	for (int d = 0; d < dimen; d++)
	{
		cl[d] =
			a_hc0.m_lower[d] < a_hc1.m_lower[d] ?
			a_hc0.m_lower[d] : a_hc1.m_lower[d];
		cu[d] =
			a_hc0.m_upper[d] > a_hc1.m_upper[d] ?
			a_hc0.m_upper[d] : a_hc1.m_upper[d];
	}
	Hypercube hc(dimen, cl, cu);
	return hc;
}

Hypercube Hypercube::combine(const Hypercube** a_hcs, const int a_count)
{
	Hypercube hc(*a_hcs[0]);
	for (int i = 1; i < a_count; i++)
		hc = combine(hc, *a_hcs[i]);
	return hc;
}

Hypercube Hypercube::intersect(const Hypercube& a_hc0, const Hypercube& a_hc1)
{
	float cl[MAXDIMEN];
	float cu[MAXDIMEN];
	int dimen = a_hc0.m_lower.m_dimen;
	for (int d = 0; d<dimen; d++)
	{
		cl[d] =
			a_hc0.m_lower[d] > a_hc1.m_lower[d] ?
			a_hc0.m_lower[d] : a_hc1.m_lower[d];
		cu[d] =
			a_hc0.m_upper[d] < a_hc1.m_upper[d] ?
			a_hc0.m_upper[d] : a_hc1.m_upper[d];
		if (cu[d] < cl[d])
			cu[d] = cl[d] = 0;
	}
	Hypercube hc(dimen, cl, cu);
	return hc;
}

Hypercube Hypercube::intersect(const Hypercube** a_hcs, const int a_count)
{
	Hypercube hc(*a_hcs[0]);
	for (int i = 1; i < a_count; i++)
		hc = intersect(hc, *a_hcs[i]);
	return hc;
}

Hypercube Hypercube::extend(const Hypercube& a_hc, const float* a_sidelen)
{
	Hypercube hc(a_hc);
	for (int d = 0; d < hc.dimen(); d++)
	{
		hc.m_lower.set(d, hc.getLower()[d] - a_sidelen[d]);
		hc.m_upper.set(d, hc.getUpper()[d] + a_sidelen[d]);
	}
	return hc;
}

int Hypercube::size(const int a_dimen)
{
	return 2 * Point::size(a_dimen);
}
