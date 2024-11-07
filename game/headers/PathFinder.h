#pragma once
#include "Intersection.h"

//Forward Declaration
class Map;
class NODE;

class PathFinder : public Intersection
{
public:
	PathFinder(Map& m);
	~PathFinder();
	//Goes Through Graph to find best Rote
	bool PathFind( int nStart, int nGoal, vector<int>& path);

	//Every Frame checks is next node visible if so deletes first one
	std::vector <CVector>  PathSmoothing(std::vector <CVector> currentWaypoints, CVector entityPos, CSprite* entity);

	//Receives initial entity coordinates and destination coordinates, generates path and returns Vector of generated CVectors(Path)
	std::vector <CVector> PathGenerator(float x, float y, CVector entityPos, bool mapOfscroll);

	//Generates and returns
	std::vector <CVector> GenerateAiPatrolPoints(CVector currentAiPos);

	//Getters fo testin only
	CSpriteList GetTestNodes();
private:
	//Functions
	//if click on obstacles return false
	bool IsPlaceAllowed(CVector mousePos);

	//checks and cleans vectors what PathFind() generated, to delete unnecessary vectors
	std::vector <CVector>  NodeCleaner(std::vector <CVector> currentWaypoints);
	void canSeeNextNode(CVector initVectorPos, std::vector <CVector>* currentWaypoints, int deleteStartIndex);

	//All nodes holder
	vector<NODE> m_graph;

	//local Path holder
	std::vector <CVector> m_waypoints;

	//Map Pointer
	Map& map;

	//just for testing
	bool EnableNodesVisual;
	CSpriteList testNodes;
};


//___________ STRUCTS___________

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
 
