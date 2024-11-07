#pragma once
#include "Intersection.h"

class Map;
class NODE;

class PathFinder : public Intersection
{
public:
	PathFinder(Map& m);
	~PathFinder();
	bool PathFind( int nStart, int nGoal, vector<int>& path);
	bool IsPlaceAllowed(CVector mousePos);

	std::vector <CVector>  PathSmoothing(std::vector <CVector> currentWaypoints, CVector entityPos, CSprite* entity);
	std::vector <CVector>  NodeCleaner(std::vector <CVector> currentWaypoints);

	void canSeeNextNode(CVector initVectorPos, std::vector <CVector> *currentWaypoints, int deleteStartIndex);
	std::vector <CVector> Move(Uint16 x, Uint16 y, CVector entityPos, bool mapOfscroll);

	//AI
	std::vector <CVector> GenerateAiPatrolPoints(CVector currentAiPos);

	CSpriteList testNodes;//to delete just for testing
	bool EnableNodesVisual;
private:
	vector<NODE> m_graph;
	std::vector <CVector> m_waypoints;
	Map& map;
};

//CONNECTIONS
struct CONNECTION
{
	int nEnd; // index of the destination node
	float cost; // cost of the transition
};

//NODES
struct NODE
{
	CVector pos; // 2D position of the node
	list<CONNECTION> conlist; // list of connections
	float costSoFar;
	int nConnection;
	bool open;

};
 
