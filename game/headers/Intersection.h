#pragma once

class Intersection
{
public:
	bool FindIntersection(CVector a, CVector b, CVector c, CVector d, float& k1, float& k2);
	bool FindIntersection(CVector a, CVector b, CVector c, CVector d);
};

 