/* ----------------------------------------------------------------------------
This header file includes class Hypercube declaration.
It is presented by two diagonal d-dimensioanl points.
---------------------------------------------------------------------------- */

#ifndef HYPERCUBE_DEFINED
#define HYPERCUBE_DEFINED

#include "header.h"
#include "point.h"

class Hypercube
{
	// data members
protected:
	Point   m_lower;                // lower corner
	Point   m_upper;                // upper corner

	// methods
public:
	// constructor/destructor
	Hypercube(const int a_dimen, const float* a_l, const float* a_u);
	Hypercube(const Point& a_lower, const Point& a_upper);
	Hypercube(const Hypercube& a_hc);
	virtual ~Hypercube();

	// assignment
	Hypercube& operator=(const Hypercube& a_hc);

	// search
	const Point& getLower() const;  // return the lower point
	const Point& getUpper() const;  // return the upper point
	Point getCenter() const;        // return the center of the hypercube
	int dimen() const;              // return the dimensionality
	float volume() const;           // return the volume of the hypercube
	float perimeter() const;        // return the perimeter of the hypercube

	// measures
	float mindist(const Hypercube& a_hc) const; // min. distance to a hypercube
	float maxdist(const Hypercube& a_hc) const; // max. distance to a hypercube
	float mindist(const Point& a_pt) const;     // min. distance to a point
	float maxdist(const Point& a_pt) const;     // max. distance to a point
	float mindist(const Point& a_pt, const int a_dimen) const; // min distance to a point on dimen d
	float diagdist() const;                     // diagonal distance


	// comparison
	bool operator==(const Hypercube& a_hc) const;   // if it equals a hypercube
	bool enclose(const Hypercube& a_hc) const;      // if it encloses a hypercube
	bool enclose(const Point& a_pt) const;          // if it encloses a point

	bool isIntersected(const Hypercube& a_hc0, const Hypercube& a_hc1) const;   //if the two hypercude intersects
	bool atRight(const Hypercube& a_hc0, const Hypercube& a_hc1) const;   //if hc0 interects hc1 at right
	bool atAbove(const Hypercube& a_hc0, const Hypercube& a_hc1) const;   //if hc0 interects hc1 at above
	bool atLeft(const Hypercube& a_hc0, const Hypercube& a_hc1) const;   //if hc0 interects hc1 at left
	bool atBelow(const Hypercube& a_hc0, const Hypercube& a_hc1) const;   //if hc0 interects hc1 at below
	int  EncloseWhichQuadrant(const Hypercube& a_hc0, const Hypercube& a_hc1, int qd_ID[]) const;//which quadrant of hc1 is enclosed by hc0

	int inWhichStripe(const Hypercube& a_hc0, const Hypercube& a_hc1, int stripe[]) const;//a_hc0 falls in which stripe of a_hc1
	int inWhichQuadrant(const Hypercube& a_hc0, const Hypercube& a_hc1, int quadrant[]) const;//a_hc0 falls in which quadrant of a_hc1

	// operations
	static Hypercube combine(const Hypercube& a_hc0, const Hypercube& a_hc1);
	static Hypercube combine(const Hypercube** a_hcs, const int a_count);
	static Hypercube intersect(const Hypercube& a_hc0, const Hypercube& a_hc1);
	static Hypercube intersect(const Hypercube** a_hcs, const int a_count);
	static Hypercube extend(const Hypercube& a_hc, const float* a_sidelen); // extend hc on every side

	// info
	static int size(const int a_dimen);
};

#endif  // HYPERCUBE_DEFINED
