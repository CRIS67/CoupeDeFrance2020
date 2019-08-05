#include "dStarLite.hpp"

extern int km;
extern int mapRows, mapColumns, obstaclesNumber;
int distance(int x1, int y1, int x2, int y2) {
	//return static_cast<int>(std::sqrt((x1-x2)*(x1-x2)+ (y1-y2)*(y1-y2)));
	//return (x1-x2)*(x1-x2)+ (y1-y2)*(y1-y2);
	return std::abs(x1-x2) + std::abs(y1-y2); // Manhattan
	//return std::max(std::abs(x1-x2),std::abs(y1-y2)); // Norme infinie

	//int dx = std::abs(x1-x2);
	//int dy = std::abs(y1-y2);
	//return linearCost * (dx + dy) + (diagonalCost - 2 * linearCost) * std::min(dx, dy);
}

// Compare the keys of two nodes and returns true if k1<k2
bool compareKeys(std::pair<int, int> k1, std::pair<int,int> k2){

	if(k1.first < k2.first)
		return true;
	else if (k1.first > k2.first)
		return false;
	else if (k1.first == k2.first){
		if (k1.second < k2.second)
			return true;
		else
			return false;
	}
}


// returns the smallest priority of all vertices in priority queue
std::pair<int,int> topKey(priorityList& uList){

    if( uList.size() == 0 ){
        std::pair<int, int> key(infinity,infinity);
        return key;
    }
	std::pair<int,int> nodeKey = uList.begin()->first;

	for(priorityList::iterator i = uList.begin(); i!= uList.end(); i++){

		if(compareKeys(i->first, nodeKey))
			nodeKey = i->first;
	}

	return nodeKey;
}

// returns a vertex with the smallest priority of all vertices in priority queue
Node top(priorityList& uList){

	std::pair<int,int> nodeKey= topKey(uList);
	return uList[nodeKey];
}

// deletes the vertex with the smallest priority in priority queue and returns-it
Node pop(priorityList& uList){
	std::pair<int,int>  nodeKey = topKey(uList);
	Node tmp  = uList[nodeKey];
	uList.erase(nodeKey);
	return tmp;
}

// Updates the priority of a node in the priorityList
void update( std::pair<int,int> oldKey, std::pair<int,int> newKey, priorityList& uList){
	priorityList::iterator itr = uList.find(oldKey);

	if(itr != uList.end()){
		Node tmpNode = itr->second;
		uList.erase(oldKey);
		uList[newKey] = tmpNode;
	}
}

int distance2(Node node1, Node node2){
	return distance(node1.coord.first, node1.coord.second, node2.coord.first, node2.coord.second);
}


std::pair<int, int> calculateKey(Node node, Node startNode){
	std::pair<int,int> key;
	int k1, k2;

	k1 = std::min(node.costG, node.costRHS) + distance2(node, startNode) + km;
	k2 = std::min(node.costG, node.costRHS);

	key.first = k1;
	key.second = k2;

	return key;

}

void initialize(std::vector<std::vector<int>>& randomMap, mappedNodes& knownNodes, priorityList& uList, Node startNode, Node goalNode){

	int rows = randomMap.size();
	int columns = randomMap[0].size();

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			std::pair<int,int> coord(i,j);
			Node tmp;
			tmp.costG =infinity;
			tmp.costRHS = infinity;
			tmp.coord = coord;
			tmp.key = calculateKey(tmp, startNode);
			if(randomMap[i][j] == 1)
				tmp.isObstacle = true;

			knownNodes[coord] = tmp;
		}
	}

	knownNodes[goalNode.coord].costRHS = 0;
	uList[calculateKey(goalNode, startNode)] = goalNode;

}

// Updates the lookahead value RHS of the node
void updateNode( Node node, priorityList& uList, mappedNodes& knownNodes, std::pair<int,int> startCoord, Node goalNode){

	if( node.coord != goalNode.coord){ // we find the best sucessor and we update the rhs
		node.costRHS = minSuccessor(node, knownNodes);
		knownNodes.at(node.coord) = node; // we update the node in the knownNodes list
	}


    priorityList::iterator itr = uList.find(node.key); // iterator to the found node

	if( itr != uList.end() ) { // if the node is in the priority list
        if(itr->second.coord == node.coord){
            //std::cout << std::endl << std::endl << itr->second.coord.first << "," << itr->second.coord.second << "\t" << node.coord.first << "," << node.coord.second << std::endl << std::endl;
            uList.erase(node.key);
        }
    }

	if( node.costG != node.costRHS){ // if the node is localy inconsistent, we added-it to the priorityList
		std::pair<int,int> newKey = calculateKey(node, knownNodes.at(startCoord));
		node.key = newKey;
		knownNodes.at(node.coord) = node; // we update the node in the knownNodes list
		//uList[newKey] = node;
		uList.insert(std::pair<std::pair<int, int>, Node>(newKey,node));
	}


}

void computeShortestPath(priorityList& uList, mappedNodes& knownNodes, std::pair<int,int> startCoord, Node goalNode) {

    //std::cout << "topKey ";
    //printKey(topKey(uList));
    //std::cout << "Calculate key start ";
    //printKey(calculateKey(knownNodes.at(startCoord), knownNodes.at(startCoord)));
    //std::cout << "StartNode RHS " << knownNodes.at(startCoord).costRHS << "  StartNode G " << knownNodes.at(startCoord).costG << std::endl;

	while( ( compareKeys(topKey(uList), calculateKey(knownNodes.at(startCoord), knownNodes.at(startCoord)))  ) ||
                                            ( knownNodes.at(startCoord).costRHS != knownNodes.at(startCoord).costG) ) {

		std::pair<int, int> oldKey = topKey(uList);
		Node uNode = pop(uList);

        //printNodesAndKeys(knownNodes);
		//std::cout << "uNode Coord " << uNode.coord.first << ","<< uNode.coord.second << "\t" << "IsObstacle : " << uNode.isObstacle << std::endl;

		if(compareKeys(oldKey, calculateKey(uNode, knownNodes.at(startCoord)))){
                std::pair<int,int> newKey = calculateKey(uNode, knownNodes.at(startCoord));// calculate new key
                uNode.key = newKey;
                knownNodes.at(uNode.coord) = uNode; // update the map
                uList.insert(std::pair<std::pair<int, int>, Node>(newKey,uNode)); // inserts uNode to the priorityList
                //uList[calculateKey(uNode, knownNodes.at(startCoord))] = uNode; // inserts uNode to the priorityList
            }


		else if (uNode.costG > uNode.costRHS){
			uNode.costG = uNode.costRHS;
			knownNodes.at(uNode.coord) = uNode;    // we update the node in the map
			updateAdjacents(uNode, uList, knownNodes, startCoord, goalNode);
		}
		else {

			uNode.costG = infinity;
			knownNodes.at(uNode.coord) = uNode; // we update the node in the map
			updateAdjacents(uNode, uList, knownNodes, startCoord,goalNode);
			updateNode(uNode,uList, knownNodes,startCoord,goalNode);

		}
	}
}

// finds the best successor and returns the RHS value
int minSuccessor(Node node, mappedNodes& knownNodes){

	std::vector<int> rhsValues;
	// Search the possible successors
	for (int i = node.coord.first -1 ; i <= node.coord.first +1 ; i++)
	{
		if( (i<0) || (i>=mapRows)) // outside of the map
			continue;

		for (int j = node.coord.second-1 ; j <= node.coord.second +1; j++)
		{
			if( (j<0) || (j>= mapColumns)) // outside of the map
				continue;
			if( (i == node.coord.first ) && (j == node.coord.second) ) // current position
				continue;

			std::pair<int,int> nodeCoord(i,j);

			if(knownNodes.at(nodeCoord).isObstacle)
				rhsValues.push_back( infinity );
			else{

				if(knownNodes.at(nodeCoord).costG == infinity)
					rhsValues.push_back(knownNodes.at(nodeCoord).costG);
				else
					rhsValues.push_back(nodeCost+knownNodes.at(nodeCoord).costG);
			}
        }

	}

	return *std::min_element(rhsValues.begin(), rhsValues.end()); // returns the value of the best succesor
}

// Updates all the adjacents Nodes of the current node, thus all the predecesor/succesors
void updateAdjacents(Node node, priorityList& uList, mappedNodes& knownNodes, std::pair<int,int> startCoord, Node goalNode){

	// Search adjacents nodes
	for (int i = node.coord.first -1 ; i <= node.coord.first +1 ; i++)
	{
		if( (i<0) || (i>=mapRows)) // outside of the map
			continue;

		for (int j = node.coord.second-1 ; j <= node.coord.second +1; j++)
		{
			if( (j<0) || (j>= mapColumns)) // outside of the map
				continue;
			if( (i == node.coord.first ) && (j == node.coord.second) ) // current position
				continue;

            std::pair<int,int> nodeCoord(i,j);
            Node tmp = knownNodes.at(nodeCoord);
            updateNode(tmp, uList, knownNodes,startCoord,goalNode);

		}



	}
}

/*
After computePath returns, we can find the best path by finding the successors that minimize c(s,s') + g(s')
*/
void findPath(std::vector<std::vector<int>>& randomMap, mappedNodes& knownNodes, Node currentNode, Node goalNode){
    std::vector<Node> path;
    path.push_back(currentNode);

    Node tmp = currentNode;
    std::vector<std::vector<int>> printedMap = randomMap; // we copy the map in order to print the path

    while(tmp.coord != goalNode.coord){
        tmp = bestNode(tmp, knownNodes,goalNode);
        path.push_back(tmp);
    }

    int x,y;

    for(uint i = 0; i< path.size(); i++){
        x = path.at(i).coord.first;
        y = path.at(i).coord.second;

        printedMap[x][y] = 2;
    }
    printMap(printedMap.size(), printedMap[0].size(), printedMap);
}

/*
Returns a Node vector with the calculated path
*/
std::vector<Node> getPath(std::vector<std::vector<int>>& randomMap, mappedNodes& knownNodes, Node currentNode, Node goalNode){
    std::vector<Node> path;
    path.push_back(currentNode);

    Node tmp = currentNode;
    std::vector<std::vector<int>> printedMap = randomMap; // we copy the map in order to print the path

    while(tmp.coord != goalNode.coord){
        tmp = bestNode(tmp, knownNodes, goalNode);
        path.push_back(tmp);
    }

    return path; 
}


/*
Finds the best successors that minimzes c(s,s') + g(s')
*/
Node bestNode(Node currentNode ,mappedNodes& knownNodes, Node goalNode){

    Node tmp; // temporary Node used to find the best one
    tmp.costG = infinity;
    Node bestOne = tmp;

    int bestOneCost = infinity;
    int tmpCost;

//    std::cout << std::endl << "=================================" << std::endl ;
//    std::cout << "Current Node Coord : " << currentNode.coord.first << ","<< currentNode.coord.second << std::endl;

    // Search the possible successors
	for (int i = currentNode.coord.first -1 ; i <= currentNode.coord.first +1 ; i++)
	{
		if( (i<0) || (i>=mapRows)) // outside of the map
			continue;

		for (int j = currentNode.coord.second-1 ; j <= currentNode.coord.second +1; j++)
		{
			if( (j<0) || (j>= mapColumns)) // outside of the map
				continue;
			if( (i == currentNode.coord.first ) && (j == currentNode.coord.second) ) // current position
				continue;

            std::pair<int,int> nodeCoord(i,j);
            tmp = knownNodes.at(nodeCoord);

            if(tmp.isObstacle)
                tmpCost =  infinity;
            else{
            	if(tmp.costG == infinity)
            		tmpCost = infinity;
            	else
            		tmpCost = tmp.costG +nodeCost;
            }

//            std::cout << "Tmp Node Coord : " << tmp.coord.first << "," << tmp.coord.second << "\tCost : " << tmpCost << "\tIsObstacle " << tmp.isObstacle << std::endl;
//            std::cout << "Best Node Coord : " << bestOne.coord.first << "," << bestOne.coord.second << "\tCost : " << bestOneCost << std::endl;

            if(tmpCost < bestOneCost){
                bestOne = tmp;
                bestOneCost = tmpCost;
            }
            else if ( tmpCost == bestOneCost){

                int distanceOne = distance2(bestOne,goalNode);
                int distanceTwo = distance2(tmp, goalNode);
//                std::cout << "=======================================" << std::endl;
//                std::cout << "Tmp Node Coord : " << tmp.coord.first << "," << tmp.coord.second << "\tCost : " << tmpCost << "\tDistance " << distanceTwo << std::endl;
//                std::cout << "Best Node Coord : " << bestOne.coord.first << "," << bestOne.coord.second << "\tCost : " << bestOneCost << "\tDistance " << distanceOne << std::endl;
//                std::cout << "=======================================" << std::endl;

                if(distanceOne > distanceTwo){
                    bestOne = tmp;
                    bestOneCost = tmpCost;
                }
            }
        }
    }
    return bestOne;
}

/*
After map modification, the function updates all the changed Nodes
*/
void updateMap(mappedNodes& knownNodes, std::vector<std::vector<int>>& randomMap, priorityList& uList, std::pair<int,int> startCoord, Node goalNode){

    for(uint i = 0 ; i< randomMap.size(); i++){

        for(uint j = 0; j< randomMap[0].size(); j++){

            std::pair<int,int> nodeCoord(i,j);

            if( randomMap[i][j] == 1 && !(knownNodes.at(nodeCoord).isObstacle) ){
                knownNodes.at(nodeCoord).isObstacle = true;
                updateAdjacents(knownNodes.at(nodeCoord),uList, knownNodes, startCoord,goalNode);
                updateNode(knownNodes.at(nodeCoord), uList, knownNodes, startCoord,goalNode);
            }
            else if (randomMap[i][j] == 0 && knownNodes.at(nodeCoord).isObstacle ){
                knownNodes.at(nodeCoord).isObstacle = false;
                updateAdjacents(knownNodes.at(nodeCoord),uList, knownNodes, startCoord,goalNode);
                updateNode(knownNodes.at(nodeCoord), uList, knownNodes, startCoord,goalNode);
            }
        }
    }
}

// debug function
void printKnownNode(mappedNodes& knownNodes){

    for(int i = 0; i < 4 ; i++ ){

        for(int j =0; j <4 ; j++){
            std::pair<int,int> nodeCoord(i,j);
            Node tmp = knownNodes.at(nodeCoord);
            std::cout << tmp.isObstacle << "\t";
        }
        std::cout << std::endl;
    }
}


// debug function
void printNodesAndKeys(mappedNodes& knownNodes){

    std::cout << std::endl << std::endl << "============================DEBUG_BEGIN==========================" << std::endl;
    for(int i = 0; i < 4 ; i++ ){

        for(int j =0; j <4 ; j++){
            std::pair<int,int> nodeCoord(i,j);
            Node tmp = knownNodes.at(nodeCoord);
            std::cout << tmp.isObstacle << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << "Costs" << std::endl;
    for(int i = 0; i < 4 ; i++ ){

        for(int j =0; j <4 ; j++){
            std::pair<int,int> nodeCoord(i,j);
            Node tmp = knownNodes.at(nodeCoord);
            std::cout << tmp.costG <<"\\" << tmp.costRHS<< "\t";
        }
        std::cout << std::endl;
    }

    std::cout << "Nodes keys"<< std::endl;
    for(int i = 0; i < 4 ; i++ ){

        for(int j =0; j <4 ; j++){
            std::pair<int,int> nodeCoord(i,j);
            Node tmp = knownNodes.at(nodeCoord);

            std::cout << tmp.key.first <<"\\" << tmp.key.second << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl << "============================DEBUG_END==========================" << std::endl;
}

void printKey(std::pair<int,int> key){
    std::cout << "K1 " << key.first << " K2 " << key.second << std::endl;
}

