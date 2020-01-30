CDF: web.o vision.o dspic.o SPI.o lidar.o actuators.o hmi.o main.o  
	g++ -o cdf web.o vision.o dspic.o SPI.o lidar.o actuators.o hmi.o main.o -Wall -lwiringPi -pthread -I/usr/local/include/ -lraspicam -lraspicam_cv `pkg-config --cflags  opencv` `pkg-config --cflags --libs opencv`

vision.o: vision.cpp
	g++ -o vision.o -c vision.cpp -Wall -lwiringPi -g -pthread -I/usr/local/include/ -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util -lopencv_contrib `pkg-config --cflags  opencv` `pkg-config --cflags --libs opencv`

web.o: web.cpp
	g++ -o web.o -c web.cpp -Wall -lwiringPi -pthread 

dspic.o: dspic.cpp 
	g++ -o dspic.o -c dspic.cpp -Wall -lwiringPi -pthread

SPI.o: SPI.cpp 
	g++ -o SPI.o -c SPI.cpp -Wall -lwiringPi -pthread 

lidar.o: lidar.cpp 
	g++ -o lidar.o -c lidar.cpp -Wall -lwiringPi -pthread 

actuators.o: actuators.cpp 
	g++ -o actuators.o -c actuators.cpp -Wall -lwiringPi -pthread 

hmi.o: hmi.cpp 
	g++ -o hmi.o -c hmi.cpp -Wall -lwiringPi -pthread 

main.o: main.cpp web.hpp dspic.hpp SPI.hpp lidar.hpp actuators.hpp hmi.hpp vision.hpp
	g++ -o main.o -c main.cpp -Wall -lwiringPi -pthread `pkg-config --cflags  opencv` `pkg-config --cflags --libs opencv`
