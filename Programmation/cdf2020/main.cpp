#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <queue>
#include "web.hpp"
#include "dspic.hpp"
#include "actuators.hpp"
#include "SPI.hpp"
#include "lidar.hpp"
#include "actuators.hpp"
#include "hmi.hpp"

#include <wiringPi.h>
#include <wiringPiSPI.h>

// DStarIncludes
#include <utility>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h>
#include <limits>
#include <map>
#include "mapGeneration.hpp"
#include "trajectoryHandle.hpp"
#include "dStarLite.hpp"



#include <fstream>
#include <sstream>

#define DEBUG_ENABLE_PRINT			1

#if(DEBUG_ENABLE_PRINT)
#define DEBUG_PRINT(x) 				std::cout << "DEBUG_PRINT>> " << x << std::endl;
#else
#define DEBUG_PRINT(x)
#endif

#define MAP_MM_PER_ARRAY_ELEMENT 	10


#define MM_ROWS						2000
#define MM_COLUMNS					3000

#define MM_START_X					1000
#define MM_START_Y					320

#define MM_SIZE_ENNEMY 				200
#define MM_SIZE_ENNEMY_MARGIN		220

#define SIZE_ENNEMY					MM_SIZE_ENNEMY / MAP_MM_PER_ARRAY_ELEMENT	//size of ennemy /2 4 element in array map -> (here 40cm)WARNING depends of map resolution
#define SIZE_ENNEMY_MARGIN			MM_SIZE_ENNEMY_MARGIN / MAP_MM_PER_ARRAY_ELEMENT

#define ARRAY_START_X				MM_START_X / MAP_MM_PER_ARRAY_ELEMENT
#define ARRAY_START_Y				MM_START_Y / MAP_MM_PER_ARRAY_ELEMENT
//DStarGlobal
int mapRows {MM_ROWS / MAP_MM_PER_ARRAY_ELEMENT};
int mapColumns {MM_COLUMNS / MAP_MM_PER_ARRAY_ELEMENT};
int km {0}; // variable for the D*

std::vector<std::vector<int>> mapVector; // the robot's map

bool obstacleDetection {false};
bool pointReached {false};

Node startNode = {std::pair<int,int>(ARRAY_START_X,ARRAY_START_Y)};
Node goalNode;
Node fromNode;
Node lastNode;

priorityList uList; // priority List
mappedNodes knownNodes; // node the robot can see


const int enemyWidth = 500; // The distance the enemy takes place on the map, it is represented as a square

std::vector<Node> simplifiedPath;
std::vector<Node> completePath;

// Strategy include
#include "LK.h"

// Debug
std::string command = "rm ./logMaps/*.txt";
const char *commandPointer = command.c_str();

void debugAct();
void debugTestAllDelay();
void debugTestAllInstant();
void debugBN();
void debugGoldenium();
int main()
{
    std::system(commandPointer);
	std::cout << std::endl << "     Coupe de France 2019 by CRIS" << std::endl << std::endl;
    wiringPiSetup();

    std::cout << "Initialisation..." << std::endl;

    DsPIC dspic;
    dspic.reset();
    delay(250); //wait for reset
    dspic.async_read(); //flush rx buffer



    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI
	Actuators actFront(&spi,SPI_ID_ACT_FRONT), actBack(&spi,SPI_ID_ACT_BACK);
	/*actFront.ResetAtmega();
    actBack.ResetAtmega();*/
    Web web(&dspic, &actFront, &actBack);
    Lidar lidar(&spi, SPI_ID_LIDAR, &web);
    lidar.stop();
    HMI hmi(&spi,SPI_ID_HMI);

    puts("Hello human ! I, your fervent robot, am initialised. Press <ENTER> to continue.");
	getchar();

	lidar.start();
    dspic.startThreadReception();
    web.startThread();
    lidar.startThreadDetection();
    hmi.startThreadDetection();
    actFront.startThreadDetection();
    actBack.startThreadDetection();

    dspic.setVar8(CODE_VAR_VERBOSE,1);
    puts("verbose set to 1");
    dspic.getVar(CODE_VAR_BAT);
    dspic.loadVarDspicFromFile("config.txt");
    //dspic.initPos(1000,1500,3.14159);
    //dspic.initPos(1200,2550,-3.14159/2);
    dspic.initPos(startNode.coord.first*MAP_MM_PER_ARRAY_ELEMENT,startNode.coord.second*MAP_MM_PER_ARRAY_ELEMENT, -3.14159 -3.14159/2);
    //dspic.initPos(1000,1500,-3.14159/2);
    //dspic.initPos(0,0,0);
    //dspic.initPos(1000,3000,-3.14159/2);
    // ===================== Test des actionneurs =========================================
    puts("Test arms");
	actFront.MoveServo(0,SERVO_VALUE_LOW);
	actFront.MoveServo(1,SERVO_VALUE_LOW);
	actFront.MoveServo(2,SERVO_VALUE_LOW);
	actBack.MoveServo(0,SERVO_VALUE_LOW);
	actBack.MoveServo(1,SERVO_VALUE_LOW);
	actBack.MoveServo(2,SERVO_VALUE_LOW);
	delay(1000);
	actFront.MoveServo(0,SERVO_VALUE_HIGH);
	actFront.MoveServo(1,SERVO_VALUE_HIGH);
	actFront.MoveServo(2,SERVO_VALUE_HIGH);
	actBack.MoveServo(0,SERVO_VALUE_HIGH);
	actBack.MoveServo(1,SERVO_VALUE_HIGH);
	actBack.MoveServo(2,SERVO_VALUE_HIGH);
	 // ===================================================================================

    std::cout << "Press enter to dspic.start() " << std::endl;
    getchar();

    //dspic.reset();
    //getchar();
    dspic.start();
    std::cout << "Press enter to start the D*" << std::endl;
    getchar();

    /*=============Strategy  START===================*/
    //while(!hmi.isStopMain()){}

    std::vector<Node> strategyTour; // Contains the nodes we are going to visit
    LK strategy(100); //faire la liste des taches jusqu'a 80 secondes
    strategy.readNodes(strategyTour); // reads the nodes from the entree.txt file
    for(unsigned int i = 0; i < strategyTour.size(); i++ ){
    	strategyTour.at(i).coord.first /= MAP_MM_PER_ARRAY_ELEMENT;
    	strategyTour.at(i).coord.second /= MAP_MM_PER_ARRAY_ELEMENT;
    }
    /*strategy.optimize(strategyTour); // optimize the tour
    strategy.printSolution(); // debug
    std::cout << "Initial distance " <<  strategy.calculateDistance(strategyTour) << std::endl;
    std::cout << "Final distance " <<  strategy.getDistance() << std::endl;
    strategyTour = strategy.getSolution();  // We update the optimized tour*/
	//Rajouter les taches à faire lors des fin de matchs
    /*std::vector<Node> Dernieres_taches_a_faire;
    strategyTour.insert( strategyTour.end(), Dernieres_taches_a_faire.begin(), Dernieres_taches_a_faire.end() );*/
    // Map Generation
    generateMap(mapVector,mapRows,mapColumns); // generates empty map
    createRectangle(0,0,300/MAP_MM_PER_ARRAY_ELEMENT,mapColumns, mapVector); // creates a 400x2000 obstacle rectangle  at (1600,0)
    createRectangle(1700/MAP_MM_PER_ARRAY_ELEMENT,0,250 / MAP_MM_PER_ARRAY_ELEMENT,mapColumns, mapVector); // creates a 400x2000 obstacle rectangle  at (1600,0)
    //createRectangle(90,140,20,20, mapVector); // creates a 400x2000 obstacle rectangle  at (1600,0)
    std::vector<std::vector<int>> debugMap(mapVector);
    //createRectangle(800/MAP_MM_PER_ARRAY_ELEMENT,1500/MAP_MM_PER_ARRAY_ELEMENT,400/MAP_MM_PER_ARRAY_ELEMENT,400/MAP_MM_PER_ARRAY_ELEMENT, debugMap);

   // /*Ensemble palets*/
   // createRectangle(90,85,30,30,mapVector);
   // createRectangle(90,185,30,30,mapVector);
   //
   // /* Palets*/
   // createRectangle(41,41,7,7,mapVector);
   // createRectangle(71,41,7,7,mapVector);
   // createRectangle(103,41,7,7,mapVector);
   // createRectangle(41,246,7,7,mapVector);
   // createRectangle(71,246,7,7,mapVector);
   // createRectangle(103,246,7,7,mapVector);

    std::cout << "MAP GENERATED" << std::endl;
    //printMap(mapRows, mapColumns, mapVector);


	/*save map in a file*/
	/*std::ofstream file;
	file.open("logMaps/out_map.txt");
	for(int y = 0; y < mapRows;y++){
		for(int x = 0; x < mapColumns; x++){
			file << mapVector.at(y).at(x) << " ";
		}
		file << std::endl;
	}
	file.close();*/

	unsigned int nbStrat = 0;
	unsigned int nbPath = 0;

    for(uint i = 0; i< strategyTour.size(); i++)
    {
    	std::ofstream file;
		std::ostringstream ss;
		ss << "VECTOR_map.txt";


    	file.open(ss.str());
					for(int y = 0; y < mapRows;y++){
						for(int x = 0; x < mapColumns; x++){
							file << mapVector.at(y).at(x) << " ";
						}
						file << std::endl;
					}
					file.close();


    	DEBUG_PRINT("strategyTour iteration nb " << i);
    	nbStrat = i;
		/*=============DStarImplementation START===================*/

        //reset lists when going to next point
        uList.clear();
        knownNodes.clear();


        //change start and goal node
        lastNode = startNode;
        goalNode.coord = strategyTour.at(i).coord;

        //initialize lists
        initialize(mapVector, knownNodes, uList, startNode, goalNode);
        //printKnownNode(knownNodes);
        goalNode = knownNodes.at(goalNode.coord);

        //computing best path to goal
        std::cout << "Start computing..." << std::endl;
        computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
        std::cout << "End  computing..." << std::endl;
        //printKnownNode(knownNodes);
        startNode = knownNodes.at(startNode.coord); // we update the start node
        goalNode = knownNodes.at(goalNode.coord); // we update the goal node
		std::cout << "Start Node coord " << startNode.coord.first << " " << startNode.coord.second << "   "<<startNode.costG<<std::endl;
		std::cout << " Goal Node coord " << goalNode.coord.first << " " << goalNode.coord.second << "   "<<goalNode.costG<<std::endl;


		if(startNode.costG == INT_MAX){

			std::cout << "AOUCH (but continue to next action)" << std::endl;
      std::cout << startNode.coord.first << " " << startNode.coord.second << " " << startNode.costG << std::endl;

			continue;
		}

		std::vector<Node> completePath = getPath(mapVector, knownNodes, startNode, goalNode); // get the whole path

	  	DEBUG_PRINT("getPath ended");
		//std::cout << "debug1" << std::endl;
		/* <modifié*/
		DEBUG_PRINT("size of path = " << completePath.size());
		std::vector<Node> tempSimplifiedPath = pathTreatment(completePath);
		DEBUG_PRINT("path1");
		tempSimplifiedPath.push_back(goalNode); // we need to add the last node manually :(
		DEBUG_PRINT("path2");
		std::vector<Node> simplifiedPath = optimizePath(mapVector,tempSimplifiedPath, startNode);
		completePath.push_back(goalNode);
		DEBUG_PRINT("path1");
		//std::vector<Node> simplifiedPath = optimizePath(mapVector,completePath, startNode);


		DEBUG_PRINT("path optimized");
		/*std::cout << "tempSimplified path : " << std::endl;
		for(unsigned int j = 0 ; j < tempSimplifiedPath.size(); j++){
			std::cout << j << " : (" << tempSimplifiedPath.at(j).coord.first << " ; "  << tempSimplifiedPath.at(j).coord.second << ")" << std::endl;
		}

		std::cout << "optimized path" << std::endl;
		for(unsigned int j = 0 ; j < simplifiedPath.size(); j++){
			std::cout << j << " : (" << simplifiedPath.at(j).coord.first << " ; "  << tempSimplifiedPath.at(j).coord.second << ")" << std::endl;
		}*/
		/*modifié>*/
		/*<original*/

		/*std::vector<Node> simplifiedPath = pathTreatment(completePath);

		//std::cout << "debug2" << std::endl;
		simplifiedPath.push_back(goalNode); // we need to add the last node manually :(
		//std::cout << "debug3" << std::endl;
		*/
		/*original>*/


		int counter=0;
		/* Dstar Loop*/
		while(startNode.coord != goalNode.coord){
			DEBUG_PRINT("while start != goal iteration nb " << counter);
			if(startNode.costG == INT_MAX){
				std::cerr << "NOT KNOWN PATH" << std::endl;
				break;
			}

			//startNode = bestNode(startNode, knownNodes); // we "move" the robot
            fromNode = startNode;
			startNode = simplifiedPath.at(counter);
			counter++;
			std::cout << "PRINTING PATH" << std::endl;
			//findPath(mapVector,knownNodes,startNode,goalNode); // prints the path in the terminal

			int xSetpoint = startNode.coord.first * MAP_MM_PER_ARRAY_ELEMENT;
			int ySetpoint = startNode.coord.second * MAP_MM_PER_ARRAY_ELEMENT;
			std::cout << "send go " << xSetpoint << " ; " << ySetpoint << std::endl;
			dspic.go(xSetpoint, ySetpoint,0,0); // we move the robot to the next point
			delay(50);  //wait before asking so the dspic can start the movement /  and don't SPAM the UART channel
			//std::cout << dspic.getX() << ";" << dspic.getY()  << std::endl;

			// Wait until the robot reaches the point

			//while(!dspic.getArrived()){
			/*while( (dspic.getX() - xSetpoint)*(dspic.getX() - xSetpoint) + (dspic.getY() - ySetpoint)*(dspic.getY() - ySetpoint) > 400){
               delay(50);  //wait before asking so the dspic can start the movement /  and don't SPAM the UART channel
               dspic.getVar(CODE_VAR_ARRIVED); //send a request to update the arrived variable
               delay(50);
			}*/
			/*while((dspic.getX() - xSetpoint)*(dspic.getX() - xSetpoint) + (dspic.getY() - ySetpoint)*(dspic.getY() - ySetpoint) > 400){
				delay(50);
			}*/

			lidar.setFillBuffer(true);	//keep track of the points detected by lidar in a member buffer
			while((dspic.getX() - xSetpoint)*(dspic.getX() - xSetpoint) + (dspic.getY() - ySetpoint)*(dspic.getY() - ySetpoint) > 400){
				//std::cout << dspic.getX() << ";" << dspic.getY()  << std::endl;
				//get lidar points
				std::queue<pointFloat2d> points = lidar.getAndClearDetectedPoints();	//get detected points and clear internal buffer
				std::queue<pointFloat2d> savePoints = points; //TODO : verifier que c'est une copie et pas une copie de reference
				unsigned int nbPoints = points.size();
				std::cout << "nbPoints" << nbPoints << std::endl;
				//add on binary map
				std::vector<std::vector<int>> newMap(mapVector); //used to detect colision
				for(unsigned int j = 0; j < nbPoints;j++){
					pointFloat2d p = points.front();
					points.pop();
					int xObstacle = (int)round(p.x / MAP_MM_PER_ARRAY_ELEMENT);
					int yObstacle = (int)round(p.y / MAP_MM_PER_ARRAY_ELEMENT);
					//std::cout << "(" << x << " ; " << y << ") ";
					for(int y = yObstacle - SIZE_ENNEMY; y < yObstacle + SIZE_ENNEMY;y++){ //TODO : verifier pk pas <=
					    for(int x = xObstacle - SIZE_ENNEMY; x < xObstacle + SIZE_ENNEMY;x++){
							if(y >= 0 && y < mapRows && x >= 0 && x < mapColumns){
								newMap.at(y).at(x) = 1;
								//std::cout << "(" << j << " ; " << k << ") ";
							}
						}
					}
					//std::cout << std::endl;
				}
				//detect Collision
				Node nRobot;
				nRobot.coord.first = (int)(dspic.getX()/MAP_MM_PER_ARRAY_ELEMENT);
				nRobot.coord.second = (int)(dspic.getY()/MAP_MM_PER_ARRAY_ELEMENT);
				DEBUG_PRINT("detect collision begin");
				obstacleDetection = detectCollision(newMap,simplifiedPath, nRobot, startNode);	//ATTENTION à changer plus tard pour ne pas prendre en compte la partie du trajet déjà fait
				std::cout<<obstacleDetection<<std::endl;
				DEBUG_PRINT("detect collision ended");
				//bool obstacleDetection = detectCollisionLine(100,50,100,250,newMap);	//ATTENTION à changer plus tard pour ne pas prendre en compte la partie du trajet déjà fait
				if(obstacleDetection){
					DEBUG_PRINT("collision detected !");
					lidar.setFillBuffer(false);
					std::vector<std::vector<int>> augmentedMap(mapVector); //used to compute modified path
					DEBUG_PRINT("Filling augmentedMap ...");
					for(unsigned int j = 0; j < nbPoints;j++){
						pointFloat2d p = savePoints.front();
						savePoints.pop();
                        int xObstacle = (int)round(p.x / MAP_MM_PER_ARRAY_ELEMENT);
                        int yObstacle = (int)round(p.y / MAP_MM_PER_ARRAY_ELEMENT);
                        //std::cout << "(" << x << " ; " << y << ") ";
                        for(int y = yObstacle - SIZE_ENNEMY_MARGIN; y < yObstacle + SIZE_ENNEMY_MARGIN;y++){
                            for(int x = xObstacle - SIZE_ENNEMY_MARGIN; x < xObstacle + SIZE_ENNEMY_MARGIN;x++){
                                if(y >= 0 && y < mapRows && x >= 0 && x < mapColumns){
                                    augmentedMap.at(y).at(x) = 1;
                                    if(newMap.at(y).at(x) == 0){
                                        newMap.at(y).at(x) = 10;
                                    }
                                    //std::cout << "(" << x << " ; " << y << ") ";
                                }
                            }
                        }
						//std::cout << std::endl;
					}
					DEBUG_PRINT("AugmentedMap filled ");

                    if(augmentedMap.at(nRobot.coord.second).at(nRobot.coord.first) == 1){ // If the nRobot is an obstacle
                        std::cout << "Start Node is an obstacle ! Finding new startNode..." << std::endl;
                        std::cout << "fromNode " << fromNode.coord.first << " " << fromNode.coord.second << std::endl;
                        std::cout << "nRobot Node " << nRobot.coord.first << " " << nRobot.coord.second << std::endl;
                        int dx =  nRobot.coord.first - fromNode.coord.first ;
                        int dy = nRobot.coord.second - fromNode.coord.second ;
                        startNode = searchNewStartNode(nRobot.coord.first, nRobot.coord.second, fromNode.coord.first, fromNode.coord.second, augmentedMap);
                        std::cout << "nRobot Node " << nRobot.coord.first << " " << nRobot.coord.second << std::endl;
                    }else{
                        startNode.coord.first = nRobot.coord.first;
                        startNode.coord.second = nRobot.coord.second;
                    }
					startNode = knownNodes.at(startNode.coord); // we update the start node
                    std::cout << "New start Node " << startNode.coord.first << " " << startNode.coord.second << " Is obstacle : " << startNode.isObstacle << std::endl;
                    std::cout << "Augmented map New start Node : " << augmentedMap.at(startNode.coord.second).at(startNode.coord.first) << std::endl;

                    DEBUG_PRINT("printing MAP");
					std::ofstream file;
					std::ostringstream ss;
					//ss << "logMaps/out_map" << nbStrat << "_" << nbPath++ << ".txt";
					ss << "out_map.txt";
					file.open(ss.str());
                    std::vector<std::vector<int>> debugMap(augmentedMap);
					debugMap.at(startNode.coord.second).at(startNode.coord.first) = 3;
					for(int y = 0; y < mapRows;y++){
						for(int x = 0; x < mapColumns; x++){
							file << debugMap.at(y).at(x) << " ";
						}
						file << std::endl;
					}
					file.close();
					DEBUG_PRINT("printing MAP ended");

					if(augmentedMap.at(goalNode.coord.second).at(goalNode.coord.first) == 1){ // If the goalNode is an obstacle
                        std::cout << "AOUCH goalNode is an obstcale" << std::endl;
                        std::cout << goalNode.coord.first << ";" << goalNode.coord.second << std::endl;
                        dspic.stop();
                        dspic.setVar8(CODE_VAR_VERBOSE,0);
                        dspic.stopThreadReception();
                        puts("verbose set to 0");
                        puts("exiting ...");
                        lidar.stop();
                        lidar.stopThreadDetection();
                        web.stopThread();

                        puts("exiting...");

                        delay(200);
					    //return -1;
					}

                    std::cout << "Goal Node" << goalNode.coord.first << " " << goalNode.coord.second << std::endl;
					std::cout << " WARNING : collision !" << std::endl;
					km = km + distance2(lastNode, startNode);
					lastNode = startNode;
					updateMap(knownNodes, augmentedMap, uList, startNode.coord, goalNode); // we update all the changed nodes
					std::cout << "computing new path" << std::endl;
					computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
					std::cout << "new path found ! =)" << std::endl;
					startNode = knownNodes.at(startNode.coord); // we update the start node
					goalNode = knownNodes.at(goalNode.coord);
					DEBUG_PRINT("Getting path ...");
					if(startNode.costG == INT_MAX){
						std::cout << "not new recalculated path found :( " << std::endl;

						dspic.stop();
						dspic.setVar8(CODE_VAR_VERBOSE,0);
					    dspic.stopThreadReception();
						puts("verbose set to 0");
					    puts("exiting ...");
						lidar.stop();
					    lidar.stopThreadDetection();
					    web.stopThread();

					    puts("exiting...");

						delay(200);

					    return -1;

					}
					completePath = getPath(augmentedMap, knownNodes, startNode, goalNode); // get the whole path
					DEBUG_PRINT("get Path ended");
					tempSimplifiedPath = pathTreatment(completePath);
					std::cout << "debug2.1" << std::endl;
					tempSimplifiedPath.push_back(goalNode); // we need to add the last node manually :(
					std::cout << "debug2.2" << std::endl;
					simplifiedPath = optimizePath(augmentedMap,tempSimplifiedPath, startNode);
					//simplifiedPath = optimizePath(augmentedMap,completePath, startNode);
					std::cout << "debug2.3" << std::endl;
					counter=0;
					std::cout << "debug3" << std::endl;

					DEBUG_PRINT("printing MAP");
					//ss << "logMaps/out_map" << nbStrat << "_" << nbPath++ << ".txt";
					ss << "logMaps/out_map" << nbPath++ << ".txt";
					file.open(ss.str());
					for(unsigned int i = 0; i < simplifiedPath.size();i++){
						newMap.at(simplifiedPath.at(i).coord.first).at(simplifiedPath.at(i).coord.second) = 4;
					}
					newMap.at(nRobot.coord.first).at(nRobot.coord.second) = 3;
					for(int y = 0; y < mapRows;y++){
						for(int x = 0; x < mapColumns; x++){
							file << newMap.at(y).at(x) << " ";
						}
						file << std::endl;
					}
					file.close();
					DEBUG_PRINT("printing MAP ended");

					break;
				}
				//std::cout << "collision = " << obstacleDetection << std::endl;
				//std::cout << "collision = " << detectCollisionLine(100,100,100,250,newMap) << std::endl;
				DEBUG_PRINT("printing MAP");
				std::ofstream file;
				std::ostringstream ss;
				//ss << "logMaps/out_map" << nbStrat << "_" << nbPath++ << ".txt";
				ss << "logMaps/out_map" << nbPath++ << ".txt";
				file.open(ss.str());
				for(unsigned int j = 0; j < simplifiedPath.size();j++){
					newMap.at(simplifiedPath.at(j).coord.second).at(simplifiedPath.at(j).coord.first) = 4;
				}
				newMap.at(nRobot.coord.second).at(nRobot.coord.first) = 3;
				for(int y = 0; y < mapRows;y++){
					for(int x = 0; x < mapColumns; x++){
						file << newMap.at(y).at(x) << " ";
					}
					file << std::endl;
				}
				file.close();
				DEBUG_PRINT("printing MAP ended");
				//printMap(mapRows,mapColumns,newMap);
				delay(50);
			}
			lidar.setFillBuffer(false);	//stop keeping lidar points to avoid running out of memory

          /*
          while(!pointReached)
          {

              TO BE IMPLEMENTED
              - getPointReached() from the dsPic
              - readSensorValues()
              - obstcaleDetection = sensorTreatment() -> determine if we have to update the map

            if(obstacleDetection)
              {
                  km = km + distance2(lastNode, startNode);
                  lastNode = startNode;
                  updateMap(knownNodes, mapVector, uList, startNode.coord, goalNode); // we update all the changed nodes
                  computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
                  startNode = knownNodes.at(startNode.coord); // we update the start node
                  goalNode = knownNodes.at(goalNode.coord);


                  TO BE IMPLEMENTED
                  - simplifiedPath = pathTreatment(getPath()) set critical points to go to
                  - create a vector with those points and update them if changes in the map


              }
          }*/

          // Debug
          std::cout << "Press nothing to continue to the next point" << std::endl;
          //getchar();
      }

      /*=============DStarImplementation END===================*/
      startNode = goalNode;
      std::cout << "Press nothing for next action"  << std::endl;
      //getchar();
      //strategyTour.erase(strategyTour.begin()+i); // We remove the action
    }


    /*=============Strategy  END ===================*/

    dspic.stop();
	dspic.setVar8(CODE_VAR_VERBOSE,0);
    dspic.stopThreadReception();
	puts("verbose set to 0");
    puts("exiting ...");
	lidar.stop();
	actFront.stopThreadDetection();
    actBack.stopThreadDetection();
    lidar.stopThreadDetection();
    web.stopThread();

    puts("exiting...");

	delay(200);
	if(hmi.isStopPrgm()) {
        system("sudo shutdown now");
    }

    return 0;
}

void debugAct(){

    wiringPiSetup();
    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI
    /*A AJOUTER : FLUSH tous les slaves*/
    Actuators actFront(&spi,SPI_ID_ACT_FRONT), actBack(&spi,SPI_ID_ACT_BACK);

    DsPIC dspic;

    dspic.async_read(); //flush rx buffer

    dspic.setVar8(CODE_VAR_VERBOSE,1);
    puts("verbose set to 1");
    dspic.getVar(CODE_VAR_BAT);
    dspic.loadVarDspicFromFile("config.txt");
    dspic.initPos(1000,1500,3.14159);

    int choice = 0;
    while(choice != -1){
        std::cout << "Press -1 to exit" << std::endl;
        std::cout << "Select arm (0-5)" << std::endl;
        std::cout << "Press 10 to set goldenium arm" << std::endl;
        std::cout << "Press 11 to reset goldenium arm" << std::endl;
        std::cin >> choice;
        if(choice >= 0 && choice <= 5){
            int id = choice;
            std::cout << "Press 1 to start pump" << std::endl;
            std::cout << "Press 2 to stop pump" << std::endl;
            std::cout << "Press 3 to measure current" << std::endl;
            std::cout << "Press 4 to set servo" << std::endl;
            std::cout << "Press 5 to testMeasure" << std::endl;
            std::cin >> choice;
            switch(choice){
                case 1:
                    if(id < 3){
                        actFront.SetPump(id,1);
                    }
                    else{
                        actBack.SetPump(id-3,1);
                    }
                    break;
                case 2:
                    if(id < 3){
                        actFront.SetPump(id,0);
                    }
                    else{
                        actBack.SetPump(id-3,0);
                    }
                    break;
                case 3:
                    if(id < 3){
                        //std::cout << "current : " << actFront.debugGetCurrent(id) << std::endl;
                    }
                    else{
                        //std::cout << "current : " << actBack.debugGetCurrent(id-3) << std::endl;
                    }
                    break;
                case 4:
                    std::cout << "enter duty cycle" << std::endl;
                    int dutyCycle;
                    std::cin >> dutyCycle;
                    if(id < 3){
                        actFront.MoveServo(id,dutyCycle);
                    }
                    else{
                        actBack.MoveServo(id-3,dutyCycle);
                    }
                    break;
                case 5:
                    if(id < 3){
                        actFront.SetPump(id,1);
                        delay(2000);
                       // std::cout << "current : " << actFront.debugGetCurrent(id) << std::endl;
                        //std::cout << "currentFull : " << actFront.debugGetCurrentFull(id) << std::endl;
                        actFront.SetPump(id,0);
                    }
                    else{
                        actBack.SetPump(id-3,1);
                        delay(500);
                        //std::cout << "current : " << actBack.debugGetCurrent(id-3) << std::endl;
                        //std::cout << "currentFull : " << actBack.debugGetCurrentFull(id-3) << std::endl;
                        actBack.SetPump(id-3,0);
                    }
                    break;
                case 6:
                    if(id < 3){
                        //std::cout << "color : " << actFront.debugGetColor(id) << std::endl;
                    }
                    else{
                        //std::cout << "color : " << actBack.debugGetColor(id-3) << std::endl;
                    }
                    break;
            }

        }
        else if(choice == 10){
            dspic.setMotLin(1);
        }
        else if(choice == 11){
            dspic.setMotLin(0);
        }
        else if(choice == 20){
            dspic.motorVoltage(0,8);
        }
        else if(choice == 21){
            dspic.motorVoltage(0,0);
        }

    }
    dspic.setVar8(CODE_VAR_VERBOSE,0);
    puts("verbose set to 0");
}
void debugTestAllDelay(){
    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI
    /*A AJOUTER : FLUSH tous les slaves*/
    Actuators actFront(&spi,SPI_ID_ACT_FRONT), actBack(&spi,SPI_ID_ACT_BACK);

    int valueH = 800;
    int valueL = 1600;
    int valueDrop = 1500;
    int valueMiddle = 1000;

    actFront.SetPump(0,1);
    actFront.MoveServo(0,valueL);
    delay(250);
    actFront.MoveServo(0,valueH);

    actFront.SetPump(1,1);
    actFront.MoveServo(1,valueL);
    delay(250);
    actFront.MoveServo(1,valueH);

    actFront.SetPump(2,1);
    actFront.MoveServo(2,valueL);
    delay(250);
    actFront.MoveServo(2,valueH);

    actBack.SetPump(0,1);
    actBack.MoveServo(0,valueL);
    delay(250);
    actBack.MoveServo(0,valueH);

    actBack.SetPump(1,1);
    actBack.MoveServo(1,valueL);
    delay(250);
    actBack.MoveServo(1,valueH);

    actBack.SetPump(2,1);
    actBack.MoveServo(2,valueL);
    delay(250);
    actBack.MoveServo(2,valueH);


    getchar();

    actFront.MoveServo(0,valueL);
    delay(250);
    actFront.SetPump(0,0);

    actFront.MoveServo(1,valueL);
    delay(250);
    actFront.SetPump(1,0);

    actFront.MoveServo(2,valueL);
    delay(250);
    actFront.SetPump(2,0);

    actBack.MoveServo(0,valueL);
    delay(250);
    actBack.SetPump(0,0);

    actBack.MoveServo(1,valueL);
    delay(250);
    actBack.SetPump(1,0);

    actBack.MoveServo(2,valueL);
    delay(250);
    actBack.SetPump(2,0);

    getchar();

    actFront.MoveServo(0,valueDrop);
    actFront.MoveServo(1,valueDrop);
    actFront.MoveServo(2,valueDrop);

    actBack.MoveServo(0,valueDrop);
    actBack.MoveServo(1,valueDrop);
    actBack.MoveServo(2,valueDrop);

    getchar();

    actFront.MoveServo(0,valueMiddle);
    actFront.MoveServo(1,valueMiddle);
    actFront.MoveServo(2,valueMiddle);

    actBack.MoveServo(0,valueMiddle);
    actBack.MoveServo(1,valueMiddle);
    actBack.MoveServo(2,valueMiddle);
}
void debugTestAllInstant(){
    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI
    /*A AJOUTER : FLUSH tous les slaves*/
    Actuators actFront(&spi,SPI_ID_ACT_FRONT), actBack(&spi,SPI_ID_ACT_BACK);

    int valueH = 800;
    int valueL = 1600;
    int valueDrop = 1500;
    int valueMiddle = 1000;

    actFront.SetPump(0,1);
    actFront.SetPump(1,1);
    actFront.SetPump(2,1);

    actBack.SetPump(0,1);
    actBack.SetPump(1,1);
    actBack.SetPump(2,1);

    actFront.MoveServo(0,valueL);
    actFront.MoveServo(1,valueL);
    actFront.MoveServo(2,valueL);

    actBack.MoveServo(0,valueL);
    actBack.MoveServo(1,valueL);
    actBack.MoveServo(2,valueL);

    delay(250);


    actFront.MoveServo(0,valueH);
    actFront.MoveServo(1,valueH);
    actFront.MoveServo(2,valueH);

    actBack.MoveServo(0,valueH);
    actBack.MoveServo(1,valueH);
    actBack.MoveServo(2,valueH);

    getchar();

    actFront.MoveServo(0,valueL);
    actFront.MoveServo(1,valueL);
    actFront.MoveServo(2,valueL);

    actBack.MoveServo(0,valueL);
    actBack.MoveServo(1,valueL);
    actBack.MoveServo(2,valueL);


    delay(250);

    actFront.SetPump(0,0);
    actFront.SetPump(1,0);
    actFront.SetPump(2,0);

    actBack.SetPump(0,0);
    actBack.SetPump(1,0);
    actBack.SetPump(2,0);

    getchar();

    actFront.MoveServo(0,valueDrop);
    actFront.MoveServo(1,valueDrop);
    actFront.MoveServo(2,valueDrop);

    actBack.MoveServo(0,valueDrop);
    actBack.MoveServo(1,valueDrop);
    actBack.MoveServo(2,valueDrop);

    getchar();

    actFront.MoveServo(0,valueMiddle);
    actFront.MoveServo(1,valueMiddle);
    actFront.MoveServo(2,valueMiddle);

    actBack.MoveServo(0,valueMiddle);
    actBack.MoveServo(1,valueMiddle);
    actBack.MoveServo(2,valueMiddle);
}
void debugBN(){
    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI
    /*A AJOUTER : FLUSH tous les slaves*/
    Actuators actFront(&spi,SPI_ID_ACT_FRONT), actBack(&spi,SPI_ID_ACT_BACK);

    //int valueH = 800;
    int valueL = 1600;
    int valueDrop = 1500;
    int valueMiddle = 1000;

    actBack.SetPump(2,1);
    delay(10);
    actBack.MoveServo(2,valueL);
    getchar();
    actBack.MoveServo(2,1000);

    getchar();
    actBack.MoveServo(2,valueL);
    delay(250);
    actBack.SetPump(2,0);
    delay(250);
    actBack.MoveServo(2,valueDrop);
    delay(250);
    actBack.MoveServo(2,valueMiddle);
}
void debugGoldenium(){
    DsPIC dspic;

    dspic.async_read(); //flush rx buffer

    std::cout << "Press enter to continue"  << std::endl;
    getchar();
    dspic.setVar8(CODE_VAR_VERBOSE,1);
    dspic.start();
    dspic.getVar(CODE_VAR_BAT);
    getchar();
    dspic.setMotLin(1);
    //dspic.motorVoltage(1,8);
    //dspic.getVar(CODE_VAR_RUPT);
    getchar();
    dspic.motorVoltage(0,10);
    getchar();
    dspic.setMotLin(0);
    getchar();
    dspic.setMotLin(1);
    getchar();
    dspic.motorVoltage(0,0);
    //dspic.motorVoltage(1,-8);
    getchar();
    dspic.motorVoltage(1,0);
    getchar();
    dspic.setMotLin(0);
    dspic.stop();
    dspic.setVar8(CODE_VAR_VERBOSE,0);
}
void reglageOdometrie(){

}
