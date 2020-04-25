#include "trajectoryHandle.hpp"

/*
Detects where the angles of the trajectory change and thus
simplifies the trajectory
*/
std::vector<Node> pathTreatment(std::vector<Node> path)
{
  std::vector<Node> simplifiedPath;
  if(path.size() < 3)
    return path;
  for(uint i = 0; i<path.size()-2; i++)
  {
    //std::cout << "debug pathTreatment : " << i << " & " << path.size() << std::endl;
    Node tmpNode = path.at(i);
    Node nextNode = path.at(i+1);
    Node furtherNode = path.at(i+2);

    int xVector = nextNode.coord.first - tmpNode.coord.first;
    int yVector = nextNode.coord.second - tmpNode.coord.second;

    int xVectorF = furtherNode.coord.first -  nextNode.coord.first ;
    int yVectorF = furtherNode.coord.second - nextNode.coord.second;

    double firstAngle = atan2(yVector,xVector);
    double secondAngle = atan2(yVectorF, xVectorF);
    if(firstAngle != secondAngle)
    {
      simplifiedPath.push_back(nextNode);
    }

  }

  return simplifiedPath;
}

bool sensorTreatment(int enemyX, int enemyY, int enemyWidth,
  std::vector<std::vector<int>>& mapVector, std::vector<Node> path)
{
  clearMap(mapVector); // clears the map
  generateMap(mapVector, mapVector.size() , mapVector.at(0).size()); // initializes the original map
  //createRectangle( enemyX, enemyY, enemyWidth, enemyWidth, mapVector); // generates the obstacle zone

  // Check if the path passes through the obsctale
  for(uint i = 0; i< path.size(); i++)
  {
    int x = path.at(i).coord.first;
    int y = path.at(i).coord.second;

    if(mapVector[y][x] == 1)
    {
      return true;
    }
  }

  return false;
}

void printPath(std::vector<Node> path, std::vector<std::vector<int>>& mapVector)
{
  std::cout <<"===== PRINTING PATH =====" << std::endl << std::endl;
  std::vector<std::vector<int>> tmpMap = mapVector;

  int x,y;
  for(uint i = 0; i< path.size(); i++)
  {
    x = path.at(i).coord.first;
    y = path.at(i).coord.second;

    tmpMap[y][x] = 2;
  }

  printMap(tmpMap.size(), tmpMap[0].size(), tmpMap);
}

bool detectCollision(std::vector<std::vector<int> > &map, std::vector<Node> path, Node start, Node nextDestination){
  unsigned int pathSize = path.size();
  if(pathSize == 0){
    std::cout << "path size = 0" << std::endl;
    return false;
  }

  //path.insert(path.begin(),start);
  //std::cout << "debug detectCollision : size of path = " << path.size() <<std::endl;
  unsigned int indexOfNextDestination = 0;
  for(unsigned int i = 0; i < path.size(); i++){
    if(path.at(i).coord.first == nextDestination.coord.first && path.at(i).coord.second == nextDestination.coord.second){
      indexOfNextDestination = i;
      break;
    }
  }
  if(detectCollisionLine(start.coord.first, start.coord.second, path.at(indexOfNextDestination).coord.first, path.at(indexOfNextDestination).coord.second,map))
    return true;
  for(unsigned int i = indexOfNextDestination; i < path.size() -1; i++){
    /*int xA = path.at(i).coord.first;
    int yA = path.at(i).coord.second;
    int xB = path.at(i).coord.first;
    int yB = path.at(i).coord.second;*/
    //std::cout << path.at(i).coord.first << " & " << path.at(i).coord.second << " -> " <<  path.at(i+1).coord.first << " & " << path.at(i+1).coord.second << std::endl;

    if(detectCollisionLine(path.at(i).coord.first, path.at(i).coord.second, path.at(i+1).coord.first, path.at(i+1).coord.second,map))
            return true;

    //std::cout << "draw" << std::endl;
        //drawLine(path.at(i).coord.first, path.at(i).coord.second, path.at(i+1).coord.first, path.at(i+1).coord.second,map);
  }
  return false;
}

std::vector<Node> optimizePath(std::vector<std::vector<int> > &map, std::vector<Node> path, Node &start){
  unsigned int lastIndex = path.size()-1;
  //std::cout << "path size = " << path.size() << std::endl;
  std::vector<Node> newPath;
  unsigned int ind = -1;

  int x = start.coord.first;
  int y = start.coord.second;

  Node n;

  while(ind != lastIndex){
    bool shortcut = false;
    for(unsigned int i = lastIndex; i > ind+1;i--){
      //std::cout << "i = " << i << std::endl;
      if(!detectCollisionLine(x,y,path.at(i).coord.first,path.at(i).coord.second,map)){
        shortcut = true;
        ind = i;

        x = path.at(ind).coord.first;
        y = path.at(ind).coord.second;

        n = path.at(ind);
        newPath.push_back(n);

        break;
      }
    }
    if(!shortcut){
      ind++;

      x = path.at(ind).coord.first;
      y = path.at(ind).coord.second;
      n = path.at(ind);
      newPath.push_back(n);
    }
  }
  return newPath;
}
bool detectCollisionLine( float x1, float y1, float x2, float y2 , std::vector<std::vector<int> >& mapVector){
        // Bresenham's line algorithm
  const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
  if(steep)
  {
    std::swap(x1, y1);
    std::swap(x2, y2);
  }

  if(x1 > x2)
  {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  const float dx = x2 - x1;
  const float dy = fabs(y2 - y1);

  float error = dx / 2.0f;
  const int ystep = (y1 < y2) ? 1 : -1;
  int y = (int)y1;

  const int maxX = (int)x2;

  for(int x=(int)x1; x<=maxX; x++)
  {
    //std::cout << x << " / " << y << std::endl;
    if(steep)
    {
        if(mapVector.at(y).at(x) == 1)
            return true;
    else  //debug to see the path on the map
      mapVector.at(y).at(x) = 2;
    }
    else
    {
        if(mapVector.at(x).at(y) == 1)
            return true;
    else
      mapVector.at(x).at(y) = 2;
    }

    error -= dy;
    if(error < 0)
    {
        y += ystep;
        error += dx;
    }
  }
  return false;
}
/*return a node with the coord of the first point that is not an obstacle on a line from the robot to the the "fromNode" */
Node searchNewStartNode( float xRobot, float yRobot, float xFrom, float yFrom , std::vector<std::vector<int> >& map){
        // Bresenham's line algorithm
  Node ret;
  const bool steep = (fabs(yFrom - yRobot) > fabs(xFrom - xRobot));
  if(steep)
  {
    std::swap(xRobot, yRobot);
    std::swap(xFrom, yFrom);
  }

  if(xRobot > xFrom)
  {
    std::swap(xRobot, xFrom);
    std::swap(yRobot, yFrom);
  }

  const float dx = xFrom - xRobot;
  const float dy = fabs(yFrom - yRobot);

  float error = dx / 2.0f;
  const int ystep = (yRobot < yFrom) ? 1 : -1;
  int y = (int)yRobot;

  const int maxX = (int)xFrom;
  //bool firstZero = false;
  for(int x=(int)xRobot; x<=maxX; x++)
  {
    //std::cout << x << " / " << y << std::endl;
    if(steep) {
      if(map.at(y).at(x) != 1){
          //if(firstZero){
        ret.coord.first = y;
        ret.coord.second = x;
        return ret;
      }
      //else
      //  firstZero = true;
      //}
      else{
        //firstZero = false;
        map.at(y).at(x) = 7;
      }
    }else {
      if(map.at(x).at(y) != 1){
        //if(firstZero){
        ret.coord.first = x;
        ret.coord.second = y;
        return ret;
      }
      //else
      //  firstZero = true;
      //}
      else{
        //firstZero = false;
        map.at(x).at(y) = 7;
      }
    }

    error -= dy;
    if(error < 0)
    {
        y += ystep;
        error += dx;
    }
  }
  ret.coord.first = -1;
  ret.coord.second = -1;
  return ret;
}