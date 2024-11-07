#include "stdafx.h"
#include "PathFinder.h"
#include "Map/Map.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!to see all the nodes on the map


//CONSTRUCTOR 
PathFinder::PathFinder(Map& m) : map(m)
{
	//for testing
	EnableNodesVisual = false;

	//NODES pivot points
	float Coords[][2] = {
		// Bathroom (5 points)
		{370, 65}, {370, 235}, {627, 115}, {627, 260}, {780, 285}, {445, 310},
		// First Room Bottom (4 points)
		{315, 570}, {235, 665}, {445, 665}, {700, 665},
		// First Room Top (3 points)
		{235, 1000}, {445, 1000}, {700, 1000},
		// Corridor Between Rooms (2 points)
		{820, 820}, {1100, 820},
		// Room Two - Top (4 points)
		{1290, 666}, {1450, 666}, {1740, 666}, {1900, 666},
		// Room Two - Mid (4 points)
		{1290, 410}, {1450, 410}, {1740, 410}, {1900, 410},
		// Room Two - Bottom (2 points)
		{1450, 210}, {1840, 210},
		// Kitchen (2 points)
		{1200, 1400}, {1826, 1400}
	};

	//NODES connections
	int Connections[][2] = {
		// Bathroom connections
		{0, 1}, {0, 2}, {1, 5}, {2, 3}, {3, 4}, {3, 5}, {5, 6},
		// First Room Bottom connections
		{6, 7}, {6, 8}, {6, 9}, {7, 10}, {8, 9}, {8, 11}, {9, 13}, {9, 12},
		// First Room Top connections
		{10, 11}, {11, 12}, {12, 13},
		// Corridor Between Rooms connections
		{13, 14}, {14, 15}, {14, 19}, {14, 25},
		// Room Two - Top connections
		{15, 16}, {15, 19}, {16, 20}, {16, 17}, {17, 18}, {17, 21}, {18, 22},
		// Room Two - Mid connections
		{19, 20}, {19, 23}, {20, 21}, {20, 23}, {21, 22}, {21, 24},
		// Room Two - Bottom connections
		{23, 24}, {24, 22},
		// Kitchen connections
		{25, 26}
	};

	//create all nodes with given coordinates
	for (float* coord : Coords)
		m_graph.push_back(NODE{ CVector(coord[0], coord[1]) });

	for (int* conn : Connections)
	{
		//start & end node
		int ind1 = conn[0];
		int ind2 = conn[1];

		//find reference to nodes according their index from connection list
		NODE& node1 = m_graph[ind1];
		NODE& node2 = m_graph[ind2];

		//calculate distance to save as a transfer cost
		float dist = Distance(node1.pos, node2.pos);

		//save connection index and transfer cost into connection for each node
		node1.conlist.push_back(CONNECTION{ ind2, dist });
		node2.conlist.push_back(CONNECTION{ ind1, dist });
	}

	///just For testing
	if(EnableNodesVisual)
	for (auto x : Coords)
	{
		CSprite* newObj = new CSprite();
		newObj->LoadImage("testobstacles.jpg");
		newObj->SetImage("testobstacles.jpg");
		newObj->SetSize(50, 50);
		newObj->SetPosition(CVector(x[0], x[1]));
		testNodes.push_back(newObj);
	}
	
}

PathFinder::~PathFinder()
{
	m_graph.clear();
	m_graph.shrink_to_fit();

	m_waypoints.clear();
	m_waypoints.shrink_to_fit();

	for (auto node : testNodes)
		delete node;
	testNodes.clear(); 
}


std::vector <CVector> PathFinder::PathSmoothing(std::vector <CVector> currentWaypoints, CVector entityPos, CSprite* entity)
{
	if (currentWaypoints.size() <= 1) return currentWaypoints;
	bool hasObstacle = false;
	float scpriteHalfSizeOffset = 30;
	// Initialize the bounding box for the player sprite
	CVector LeftTop = CVector(entityPos.GetX() - scpriteHalfSizeOffset, entityPos.GetY() + scpriteHalfSizeOffset);
	CVector LeftBottom = CVector(entityPos.GetX() - scpriteHalfSizeOffset, entityPos.GetY() - scpriteHalfSizeOffset);
	CVector RightTop = CVector(entityPos.GetX() + scpriteHalfSizeOffset, entityPos.GetY() + scpriteHalfSizeOffset);
	CVector RightBottom = CVector(entityPos.GetX() + scpriteHalfSizeOffset, entityPos.GetY() - scpriteHalfSizeOffset);


	// Check each obstacle to see if there’s an intersection
	for (auto obstacle : map.checkObects) 
	{
		std::vector<CVector> obstacleCorners = 
		{
			CVector(obstacle->GetLeft(), obstacle->GetTop()),    // Top-left
			CVector(obstacle->GetLeft(), obstacle->GetBottom()), // Bottom-left
			CVector(obstacle->GetRight(), obstacle->GetTop()),   // Top-right
			CVector(obstacle->GetRight(), obstacle->GetBottom()) // Bottom-right
		};

		if 
			(
				Intersection::FindIntersection(LeftTop, currentWaypoints[1], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(LeftTop, currentWaypoints[1], obstacleCorners[1], obstacleCorners[2]) ||
				Intersection::FindIntersection(LeftBottom, currentWaypoints[1], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(LeftBottom, currentWaypoints[1], obstacleCorners[1], obstacleCorners[2]) ||
				Intersection::FindIntersection(RightTop, currentWaypoints[1], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(RightTop, currentWaypoints[1], obstacleCorners[1], obstacleCorners[2]) ||
				Intersection::FindIntersection(RightBottom, currentWaypoints[1], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(RightBottom, currentWaypoints[1], obstacleCorners[1], obstacleCorners[2])
			)
		{
			hasObstacle = true;
			break;
		}
	}


	//if (!hasObstacle || Dot(entity->GetVelocity(), currentWaypoints[0] - entity->GetPos()) < 0)
	if (!hasObstacle || Distance(entity->GetPos(), currentWaypoints[0]) <= 25)
		currentWaypoints.erase(currentWaypoints.begin());

	return currentWaypoints;
}

 
void PathFinder::canSeeNextNode(CVector initVector, vector<CVector>* currentWaypoints, int deleteStartIndex)
{
	bool hasObstacle = false;
	float scpriteHalfSizeOffset = 30;
	// Initialize the bounding box for the player sprite
	CVector LeftTop = CVector(initVector.GetX() - scpriteHalfSizeOffset, initVector.GetY() + scpriteHalfSizeOffset);
	CVector LeftBottom = CVector(initVector.GetX() - scpriteHalfSizeOffset, initVector.GetY() - scpriteHalfSizeOffset);
	CVector RightTop = CVector(initVector.GetX() + scpriteHalfSizeOffset, initVector.GetY() + scpriteHalfSizeOffset);
	CVector RightBottom = CVector(initVector.GetX() + scpriteHalfSizeOffset, initVector.GetY() - scpriteHalfSizeOffset);


	// Iterate through the waypoints in reverse order
	for (int i = currentWaypoints->size() - 1; i > 1; i--)
	{
		for (auto obstacle : map.checkObects)
		{
	
			vector<CVector> obstacleCorners =
			{
				CVector(obstacle->GetLeft(), obstacle->GetTop()),    // Top-left
				CVector(obstacle->GetLeft(), obstacle->GetBottom()), // Bottom-left
				CVector(obstacle->GetRight(), obstacle->GetTop()),   // Top-right
				CVector(obstacle->GetRight(), obstacle->GetBottom()) // Bottom-right
			};

			if (Intersection::FindIntersection(LeftTop, (*currentWaypoints)[i], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(LeftTop, (*currentWaypoints)[i], obstacleCorners[1], obstacleCorners[2]) ||
				Intersection::FindIntersection(LeftBottom, (*currentWaypoints)[i], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(LeftBottom, (*currentWaypoints)[i], obstacleCorners[1], obstacleCorners[2]) ||
				Intersection::FindIntersection(RightTop, (*currentWaypoints)[i], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(RightTop, (*currentWaypoints)[i], obstacleCorners[1], obstacleCorners[2]) ||
				Intersection::FindIntersection(RightBottom, (*currentWaypoints)[i], obstacleCorners[0], obstacleCorners[3]) ||
				Intersection::FindIntersection(RightBottom, (*currentWaypoints)[i], obstacleCorners[1], obstacleCorners[2])
				)
			{
				hasObstacle = true;
				break;
			}
		}
	
		// If no obstacle is found, delete the waypoints between index 1 and i
		if (!hasObstacle)
		{
			currentWaypoints->erase(currentWaypoints->begin() + deleteStartIndex + 1, currentWaypoints->begin() + i );
			break;
		}	
	}
}

vector<CVector> PathFinder::NodeCleaner(vector<CVector> currentWaypoints)
{

	for (int i= 0; i < currentWaypoints.size() - 2; i++)
		canSeeNextNode(currentWaypoints[i], &currentWaypoints, i);
	return currentWaypoints;
}


bool PathFinder::PathFind(int nStart, int nGoal, vector<int>& path)
{
	list<int> openNodes;
	// mark all nodes in the graph as unvisited
	for (auto& node : m_graph)
		node.open = false;

	// Closes node to player EG. START node
	m_graph[nStart].open = true;
	m_graph[nStart].costSoFar = 0;
	m_graph[nStart].nConnection = -1;
	openNodes.push_back(nStart);

	//_____________
	while (!openNodes.empty())
	{
		// lambda returns the smallest costSoFar in current open nodes;
		//first loop lest say node[0] cost = 0;
		//+ Heuristic prediction A* algo
		int curNode = *min_element(openNodes.begin(), openNodes.end(),
			[this, nGoal](int n1, int n2) {
				float f1 = m_graph[n1].costSoFar + Distance(m_graph[n1].pos, m_graph[nGoal].pos);
				float f2 = m_graph[n2].costSoFar + Distance(m_graph[n2].pos, m_graph[nGoal].pos);
				return f1 < f2;
			});

		// Found the goal node?
		if (curNode == nGoal)
			break;

		//for each conn in node[0] end connection id 1 (as we have only 1 now)
		for each (auto conn in m_graph[curNode].conlist)
		{
			//id1
			int endNode = conn.nEnd;
			float newCostSoFar = m_graph[curNode].costSoFar + conn.cost;

			// for open nodes, ignore if the current route worse then the old one
			if (m_graph[endNode].open && m_graph[endNode].costSoFar <= newCostSoFar)
				continue;

			// Wow, we've found a better route!
			m_graph[endNode].costSoFar = newCostSoFar;
			m_graph[endNode].nConnection = curNode;

			// if unvisited yet, add to the open list
			if (!m_graph[endNode].open)
			{
				m_graph[endNode].open = true;
				//add endNode to the list of open nodes;
				openNodes.push_back(endNode);
			}
		}
 
		//	remove the current node from the open nodes list;
		openNodes.remove(curNode);
	}

	// Collect the path from the generated graph data
	if (openNodes.empty())
		return false; // path not found!
	int i = nGoal;

	while (m_graph[i].nConnection >= 0)
	{
		path.push_back(i);
		i = m_graph[i].nConnection;
	}

	path.push_back(i);
	reverse(path.begin(), path.end());

	return true;
}

//********* checks  Is Place Allowed
bool PathFinder::IsPlaceAllowed(CVector mousePos)
{
	
	for (auto obstacle : map.checkObects)
		if (obstacle->HitTest(mousePos))
		{
			return false;
			m_waypoints.clear();
		}
	if (mousePos.Y() < 50) return false; // Bug fix :)
	return true;
}


//********* on mouse click action, returns CVectors array
std::vector <CVector> PathFinder::Move(Uint16 x, Uint16 y, CVector entityPos, bool mapOfscroll)
{
	CVector dest(x, y);
	if(mapOfscroll)  dest -= map.currentScrollOffset;  


	if (!IsPlaceAllowed(dest))
	{
		m_waypoints.clear();
		return m_waypoints;
	}

	//  NPC node
	vector<NODE>::iterator iFirst =
		min_element(m_graph.begin(), m_graph.end(), [this, entityPos](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, entityPos) < Distance(n2.pos, entityPos);
			});
	int nFirst = iFirst - m_graph.begin();

	//Destination node
	vector<NODE>::iterator iLast =
		min_element(m_graph.begin(), m_graph.end(), [dest](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, dest) < Distance(n2.pos, dest);
			});
	int nLast = iLast - m_graph.begin();

	// just reset if mouse clicked again
	if (!m_waypoints.empty())
		m_waypoints.clear();

	// call the path finding algorithm to complete the waypoints
	vector<int> path;

	if (PathFind(nFirst, nLast, path))
	{
		for (int nodeI : path)
			m_waypoints.push_back(m_graph[nodeI].pos);
		m_waypoints.push_back(dest); //+ finall coordinate where mouse where clicked
	}
	m_waypoints.insert(m_waypoints.begin(), entityPos); // add start player pos, to calculate correct path smooth

	
	/*
	for (auto point : m_waypoints)
		cout << "X " << point.GetX() << " Y " << point.GetY() << endl;
	cout << endl << endl << endl;
		*/

	vector<CVector> smoothPath = NodeCleaner(m_waypoints);
	smoothPath.erase(smoothPath.begin()); //delete start player pos



	return smoothPath;
}

std::vector<CVector> PathFinder::GenerateAiPatrolPoints(CVector currentAiPos)
{
	int depthOfPatrolPoints = 3;
	std::vector<CVector> patrolPoints;

	vector<NODE>::iterator iFirst =
		min_element(m_graph.begin(), m_graph.end(), [this, currentAiPos](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, currentAiPos) < Distance(n2.pos, currentAiPos);
			});


	int nFirst = iFirst - m_graph.begin();
	patrolPoints.push_back(m_graph[nFirst].pos);

	for (int i = 0; i < depthOfPatrolPoints; i++)
	{
		int nextCon = m_graph[nFirst].conlist.begin()->nEnd;
		patrolPoints.push_back(m_graph[nextCon].pos);
		nFirst = nextCon;
	}

	//put middle Vectors back to make circular patroling , patroll point depth - 1 (for start  pos) > 0 to not include start pos
	for (int i = depthOfPatrolPoints - 1; i > 0; i--)
	{
		CVector prevCon = patrolPoints[i];
		patrolPoints.push_back(prevCon);
	}




	return patrolPoints;
}
