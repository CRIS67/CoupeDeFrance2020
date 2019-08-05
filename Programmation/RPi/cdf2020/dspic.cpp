#include "dspic.hpp"

DsPIC::DsPIC(){
    fd = serialOpen ("/dev/serial0", BAUDRATE);
}
DsPIC::~DsPIC(){
}
void DsPIC::startThreadReception(){
    m_mutex.lock();
    m_continueThread = true;
    m_mutex.unlock();
    int rc;
    rc = pthread_create(&m_threadReception, NULL, thread_reception, this);
    if (rc) {
        std::cout << "Error:unable to create dspic thread" << rc << std::endl;
    }
}
void DsPIC::stopThreadReception(){
    m_mutex.lock();
    m_continueThread = false;
    m_mutex.unlock();
}
/**
Initialize constant parameters of dspic : PID, odometry, maximum speed, maximum acceleration
*/
/*void DsPIC::initVarDspic(){
	//PID speed left
    setVarDouble64b(CODE_VAR_P_SPEED_L_LD,KP_SPEED_LEFT);
    setVarDouble64b(CODE_VAR_I_SPEED_L_LD,KI_SPEED_LEFT);
    setVarDouble64b(CODE_VAR_D_SPEED_L_LD,KD_SPEED_LEFT);
    //PID speed left
    setVarDouble64b(CODE_VAR_P_SPEED_R_LD,KP_SPEED_RIGHT);
    setVarDouble64b(CODE_VAR_I_SPEED_R_LD,KI_SPEED_RIGHT);
    setVarDouble64b(CODE_VAR_D_SPEED_R_LD,KD_SPEED_RIGHT);
    //PID distance
    setVarDouble64b(CODE_VAR_P_DISTANCE_LD,KP_DISTANCE);
    setVarDouble64b(CODE_VAR_I_DISTANCE_LD,KI_DISTANCE);
    setVarDouble64b(CODE_VAR_D_DISTANCE_LD,KD_DISTANCE);
    //PID angle
    setVarDouble64b(CODE_VAR_P_ANGLE_LD,KP_ANGLE);
    setVarDouble64b(CODE_VAR_I_ANGLE_LD,KI_ANGLE);
    setVarDouble64b(CODE_VAR_D_ANGLE_LD,KD_ANGLE);
	//Odometry
    setVarDouble64b(CODE_VAR_COEF_DISSYMETRY_LD,COEF_DISSYMETRY);
    setVarDouble64b(CODE_VAR_MM_PER_TICKS_LD,MM_PER_TICKS);
    setVarDouble64b(CODE_VAR_RAD_PER_TICKS_LD,RAD_PER_TICKS);
	//Linear speed & acceleration for trajectory generation
    setVarDouble64b(CODE_VAR_TRAJ_LIN_SPEED_LD,TRAJ_LIN_SPEED);
    setVarDouble64b(CODE_VAR_TRAJ_LIN_ACC_LD,TRAJ_LIN_ACC);
	//Angular speed & acceleration for trajectory generation
    setVarDouble64b(CODE_VAR_TRAJ_ROT_SPEED_LD,TRAJ_ROT_SPEED);
    setVarDouble64b(CODE_VAR_TRAJ_ROT_ACC_LD,TRAJ_ACC_SPEED);
}*/
/**
arg : 	<path> 	: path of config file ("config.txt" for example)

Initialize constant parameters of dspic from a config file : PID, odometry, maximum speed, maximum acceleration
*/
void DsPIC::loadVarDspicFromFile(std::string path){

	double kpR;
	double kiR;
	double kdR;

	double kpL;
	double kiL;
	double kdL;

	double kpD;
	double kiD;
	double kdD;

	double kpA;
	double kiA;
	double kdA;

	double coefDissymetry;
	double mmPerTick;
	double radPerTick;

	double linSpeed;
	double linAcc;
	double rotSpeed;
	double rotAcc;

	std::string line;
	std::ifstream myfile (path.c_str());
	if (myfile.is_open()){
		while (getline(myfile,line)){
			if(line.length() > 0){
				if(line.at(0) != '#'){
					//cout << line << '\n';
					std::string option,value;
					std::stringstream input_stringstream(line);
					getline(input_stringstream,option,'=');
					getline(input_stringstream,value,'=');
					//cout << option << '\n';
					//cout << value << '\n';

					//PID speed right
					if(option.compare("PidSpeedRightKp") == 0){
						std::stringstream ss(value);
						ss >> kpR;
					}
					else if(option.compare("PidSpeedRightKi") == 0){
						std::stringstream ss(value);
						ss >> kiR;
					}
					else if(option.compare("PidSpeedRightKd") == 0){
						std::stringstream ss(value);
						ss >> kdR;
					}
					//PID speed left
					else if(option.compare("PidSpeedLeftKp") == 0){
						std::stringstream ss(value);
						ss >> kpL;
					}
					else if(option.compare("PidSpeedLeftKi") == 0){
						std::stringstream ss(value);
						ss >> kiL;
					}
					else if(option.compare("PidSpeedLeftKd") == 0){
						std::stringstream ss(value);
						ss >> kdL;
					}
					//PID distance
					else if(option.compare("PidDistanceKp") == 0){
						std::stringstream ss(value);
						ss >> kpD;
					}
					else if(option.compare("PidDistanceKi") == 0){
						std::stringstream ss(value);
						ss >> kiD;
					}
					else if(option.compare("PidDistanceKd") == 0){
						std::stringstream ss(value);
						ss >> kdD;
					}
					//PID angle
					else if(option.compare("PidAngleKp") == 0){
						std::stringstream ss(value);
						ss >> kpA;
					}
					else if(option.compare("PidAngleKi") == 0){
						std::stringstream ss(value);
						ss >> kiA;
					}
					else if(option.compare("PidAngleKd") == 0){
						std::stringstream ss(value);
						ss >> kdA;
					}
					//Odometry
					else if(option.compare("coefDissymetry") == 0){
						std::stringstream ss(value);
						ss >> coefDissymetry;
					}
					else if(option.compare("mmPerTick") == 0){
						std::stringstream ss(value);
						ss >> mmPerTick;
					}
					else if(option.compare("radPerTick") == 0){
						std::stringstream ss(value);
						ss >> radPerTick;
					}
					//trajectory generation
					else if(option.compare("linSpeed") == 0){
						std::stringstream ss(value);
						ss >> linSpeed;
					}
					else if(option.compare("linAcc") == 0){
						std::stringstream ss(value);
						ss >> linAcc;
					}
					else if(option.compare("rotSpeed") == 0){
						std::stringstream ss(value);
						ss >> rotSpeed;
					}
					else if(option.compare("rotAcc") == 0){
						std::stringstream ss(value);
						ss >> rotAcc;
					}
				}
			}
		}
		myfile.close();
	}
	else{
		std::cout << "Unable to open file";
	}

	/*
    std::cout <<  kpR << std::endl;
	std::cout <<  kiR << std::endl;
	std::cout <<  kdR << std::endl;

	std::cout <<  kpL << std::endl;
	std::cout <<  kiL << std::endl;
	std::cout <<  kdL << std::endl;

	std::cout <<  kpD << std::endl;
	std::cout <<  kiD << std::endl;
	std::cout <<  kdD << std::endl;

	std::cout <<  kpA << std::endl;
	std::cout <<  kiA << std::endl;
	std::cout <<  kdA << std::endl;

	std::cout <<  coefDissymetry << std::endl;
	std::cout <<  mmPerTick << std::endl;
	std::cout <<  radPerTick << std::endl;

	std::cout <<  linSpeed << std::endl;
	std::cout <<  linAcc << std::endl;
	std::cout <<  rotSpeed << std::endl;
	std::cout <<  rotAcc << std::endl;
    */

	//PID speed left
    setVarDouble64b(CODE_VAR_P_SPEED_L_LD,kpR);
    setVarDouble64b(CODE_VAR_I_SPEED_L_LD,kiR);
    setVarDouble64b(CODE_VAR_D_SPEED_L_LD,kdR);
    //PID speed left
    setVarDouble64b(CODE_VAR_P_SPEED_R_LD,kpL);
    setVarDouble64b(CODE_VAR_I_SPEED_R_LD,kiL);
    setVarDouble64b(CODE_VAR_D_SPEED_R_LD,kdL);
    //PID distance
    setVarDouble64b(CODE_VAR_P_DISTANCE_LD,kpD);
    setVarDouble64b(CODE_VAR_I_DISTANCE_LD,kiD);
    setVarDouble64b(CODE_VAR_D_DISTANCE_LD,kdD);
    //PID angle
    setVarDouble64b(CODE_VAR_P_ANGLE_LD,kpA);
    setVarDouble64b(CODE_VAR_I_ANGLE_LD,kiA);
    setVarDouble64b(CODE_VAR_D_ANGLE_LD,kdA);
	//Odometry
    setVarDouble64b(CODE_VAR_COEF_DISSYMETRY_LD,coefDissymetry);
    setVarDouble64b(CODE_VAR_MM_PER_TICKS_LD,mmPerTick);
    setVarDouble64b(CODE_VAR_RAD_PER_TICKS_LD,radPerTick);
	//Linear speed & acceleration for trajectory generation
    setVarDouble64b(CODE_VAR_TRAJ_LIN_SPEED_LD,linSpeed);
    setVarDouble64b(CODE_VAR_TRAJ_LIN_ACC_LD,linAcc);
	//Angular speed & acceleration for trajectory generation
    setVarDouble64b(CODE_VAR_TRAJ_ROT_SPEED_LD,rotSpeed);
    setVarDouble64b(CODE_VAR_TRAJ_ROT_ACC_LD,rotAcc);
}
/**
arg : 	<id> 	: [0;3] 		: 	id of servo (depends of number of available servomotors controllable by the dspic)
		<value> : [0;20000] µs 	: 	length of pulse in microseconds (µs) sent to control the servomotor

Move servo n° <id> to the position specified by <value>
*/
void DsPIC::servo(uint8_t id, uint16_t value){
    uint8_t buffer[RX_SIZE_SERVO + 1];
    buffer[0] = RX_SIZE_SERVO;
    buffer[1] = RX_CODE_SERVO;
    buffer[2] = id;
    buffer[3] = (uint8_t)(value >> 8);
    buffer[4] = (uint8_t)(value & 0xFF);
    buffer[5] = 0;
    for(int i = 0; i < RX_SIZE_SERVO; i++){
        buffer[5] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SERVO + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void DsPIC::AX12(uint8_t id, uint16_t value){
    uint8_t buffer[RX_SIZE_AX12 + 1];
    buffer[0] = RX_SIZE_AX12;
    buffer[1] = RX_CODE_AX12;
    buffer[2] = id;
    buffer[3] = (uint8_t)(value >> 8);
    buffer[4] = (uint8_t)(value & 0xFF);
    buffer[5] = 0;
    for(int i = 0; i < RX_SIZE_AX12; i++){
        buffer[5] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_AX12 + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<id> 	: [0;1] 		: 	id of actuator motor [0-1] (depends of number of available motors controllable by the dspic)
		<value> : [-100;100] % 	: 	percentage (%) of duty cycle sent to motor

Change voltage applied to motor n° <id> to the value specified by <value> ( sign of value determine direction of rotation)
*/
void DsPIC::motor(uint8_t id, int8_t value){
    uint8_t buffer[RX_SIZE_MOTOR + 1];
    buffer[0] = RX_SIZE_MOTOR;
    buffer[1] = RX_CODE_MOTOR;
    buffer[2] = id;
    buffer[3] = value;
    buffer[4] = 0;
    for(int i = 0; i < RX_SIZE_MOTOR; i++){
        buffer[4] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_MOTOR + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<id> 	: [0;1] 		: 	id of actuator motor [0-1] (depends of number of available motors controllable by the dspic)
		<value> : [-Vbat;Vbat] % 	: 	voltage applied to the motor (max voltage is the battery voltage)

Change voltage applied to motor n° <id> to the value specified by <value> ( sign of value determine direction of rotation)
*/
void DsPIC::motorVoltage(uint8_t id, float value){
	uint8_t *ptr = (uint8_t*)&value;
    uint8_t buffer[RX_SIZE_MOTOR_VOLTAGE + 1];
    buffer[0] = RX_SIZE_MOTOR_VOLTAGE;
    buffer[1] = RX_CODE_MOTOR_VOLTAGE;
    buffer[2] = id;
    buffer[3] = ptr[0];
    buffer[4] = ptr[1];
    buffer[5] = ptr[2];
    buffer[6] = ptr[3];
    buffer[7] = 0;
    for(int i = 0; i < RX_SIZE_MOTOR_VOLTAGE; i++){
        buffer[7] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_MOTOR_VOLTAGE + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<state> 	: [0;1] 		: 	0 -> out / 1 -> in

Extend or retract the linear arm
*/
void DsPIC::setMotLin(uint8_t state){
    uint8_t buffer[RX_SIZE_SET_MOT_LIN + 1];
    buffer[0] = RX_SIZE_SET_MOT_LIN;
    buffer[1] = RX_CODE_SET_MOT_LIN;
    buffer[2] = state;
    buffer[3] = 0;
    for(int i = 0; i < RX_SIZE_SET_MOT_LIN; i++){
        buffer[3] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_MOT_LIN + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
Enable propulsion motors
*/
void DsPIC::start(){
    uint8_t buffer[RX_SIZE_START + 1];
    buffer[0] = RX_SIZE_START;
    buffer[1] = RX_CODE_START;
    buffer[2] = 0;
    for(int i = 0; i < RX_SIZE_START; i++){
        buffer[2] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_START + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
Disable propulsion motors
*/
void DsPIC::stop(){
    uint8_t buffer[RX_SIZE_STOP + 1];
    buffer[0] = RX_SIZE_STOP;
    buffer[1] = RX_CODE_STOP;
    buffer[2] = 0;
    for(int i = 0; i < RX_SIZE_STOP; i++){
        buffer[2] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_STOP + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void DsPIC::brake(){
    uint8_t buffer[RX_SIZE_BRAKE + 1];
    buffer[0] = RX_SIZE_BRAKE;
    buffer[1] = RX_CODE_BRAKE;
    buffer[2] = 0;
    for(int i = 0; i < RX_SIZE_BRAKE; i++){
        buffer[2] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_BRAKE + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void DsPIC::reset(){
    uint8_t buffer[RX_SIZE_RESET + 1];
    buffer[0] = RX_SIZE_RESET;
    buffer[1] = RX_CODE_RESET;
    buffer[2] = 0;
    for(int i = 0; i < RX_SIZE_RESET; i++){
        buffer[2] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_RESET + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<x> 		: [?] mm	: 	x position in mm. Must be in [0;2000] range if relative = 0 or in [-xRobot;2000-xRobot] range if relative = 1
		<y> 		: [?] mm	: 	y position in mm. Must be in [0;3000] range if relative = 0 or in [-yRobot;3000-yRobot] range if relative = 1
		<rev> 		: [0;1]  	: 	0 -> forward movement / -> backward movement
		<relative> 	: [0;1]  	: 	0 -> absolute position / -> relative position

Change voltage applied to motor n° <id> to the value specified by <value> ( sign of value determine direction of rotation)
*/
void DsPIC::go(int16_t x, int16_t y,unsigned char rev, unsigned char relative){
    uint8_t option = 0;
    if(rev){
        option += MASK_OPTION_REVERSE;
    }
    if(relative){
        option += MASK_OPTION_RELATIVE;
    }
    uint8_t buffer[RX_SIZE_GO + 1];
    buffer[0] = RX_SIZE_GO;
    buffer[1] = RX_CODE_GO;
    buffer[2] = option;
    buffer[3] = (uint8_t)(x >> 8);
    buffer[4] = (uint8_t)(x & 0xFF);
    buffer[5] = (uint8_t)(y >> 8);
    buffer[6] = (uint8_t)(y & 0xFF);
    buffer[7] = 0;
    for(int i = 0; i < RX_SIZE_GO; i++){
        buffer[7] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_GO + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<t> 		: [?] deg	: 	angle in degrees
		<relative> 	: [0;1]  	: 	0 -> absolute angle / -> relative angle

Change voltage applied to motor n° <id> to the value specified by <value> ( sign of value determine direction of rotation)
*/
void DsPIC::turn(int16_t t, unsigned char relative){
    uint8_t option = 0;
    if(relative){
        option += MASK_OPTION_RELATIVE;
    }
    uint8_t buffer[RX_SIZE_TURN + 1];
    buffer[0] = RX_SIZE_TURN;
    buffer[1] = RX_CODE_TURN;
    buffer[2] = option;
    buffer[3] = (uint8_t)(t >> 8);
    buffer[4] = (uint8_t)(t & 0xFF);
    buffer[5] = 0;
    for(int i = 0; i < RX_SIZE_TURN; i++){
        buffer[5] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_TURN + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
void DsPIC::initPos(double x, double y, double t){
	//change position
    setVarDouble64b(CODE_VAR_X_LD,x);
    setVarDouble64b(CODE_VAR_Y_LD,y);
    setVarDouble64b(CODE_VAR_T_LD,t);
	//change setpoint
    setVarDouble64b(CODE_VAR_XC_LD,x);
    setVarDouble64b(CODE_VAR_YC_LD,y);
    setVarDouble64b(CODE_VAR_TC_LD,t);
	//change final setpoint
    setVarDouble64b(CODE_VAR_XF_LD,x);
    setVarDouble64b(CODE_VAR_YF_LD,y);
    setVarDouble64b(CODE_VAR_TF_LD,t);
}
/**
arg : 	<varCode> 	: [0;255] 				: 	code of desired variable (see #define)
		<var> 		: decimal real value  	: 	value of the variable

Set value of a 64bits decimal variable of dspic ("long double" on the dspic / "double" on raspbian).
*/
void DsPIC::setVarDouble64b(uint8_t varCode, double Var){
    double *ptrVar = &Var;
    uint8_t *ptr = (uint8_t*)ptrVar;
    uint8_t buffer[RX_SIZE_SET_64b + 1];
    buffer[0] = RX_SIZE_SET_64b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_LD_64b;
    buffer[4] = ptr[0];
    buffer[5] = ptr[1];
    buffer[6] = ptr[2];
    buffer[7] = ptr[3];
    buffer[8] = ptr[4];
    buffer[9] = ptr[5];
    buffer[10] = ptr[6];
    buffer[11] = ptr[7];
    buffer[12] = 0;
    for(int i = 0; i < RX_SIZE_SET_64b; i++){
        buffer[12] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_64b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<varCode> 	: [0;255] 			: 	code of desired variable (see #define)
		<var> 		: [0;4294967295]  	: 	value of the variable

Set value of an unsigned or signed integer 32bits variable of dspic (uint32_t / int32_t). Signed integer can be passed as unsigned integer and will be casted to integer when received by the dspic
*/
void DsPIC::setVar32(uint8_t varCode, uint32_t var){

    uint8_t buffer[RX_SIZE_SET_32b + 1];
    buffer[0] = RX_SIZE_SET_32b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_32b;
    buffer[4] = (uint8_t)(var >> 24);
    buffer[5] = (uint8_t)(var >> 16);
    buffer[6] = (uint8_t)(var >> 8);
    buffer[7] = (uint8_t)(var & 0xFF);
    buffer[8] = 0;
    for(int i = 0; i < RX_SIZE_SET_32b; i++){
        buffer[8] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_32b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<varCode> 	: [0;255] 	: 	code of desired variable (see #define)
		<var> 		: [0;255]  	: 	value of the variable

Set value of an unsigned or signed integer 8bits variable of dspic (uint8_t / int8_t). Signed integer can be passed as unsigned integer and will be casted to integer when received by the dspic
*/
void DsPIC::setVar8(uint8_t varCode, uint8_t var){

    uint8_t buffer[RX_SIZE_SET_8b + 1];
    buffer[0] = RX_SIZE_SET_8b;
    buffer[1] = RX_CODE_SET;
    buffer[2] = varCode;
    buffer[3] = VAR_8b;
    buffer[4] = var;
    buffer[5] = 0;
    for(int i = 0; i < RX_SIZE_SET_8b; i++){
        buffer[5] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_SET_8b + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
arg : 	<varCode> 		: [0;255] 	: 	code of desired variable (see #define)

Ask for variable n° <varCode>. Respone will be processed in the UART reception thread
*/
void DsPIC::getVar(uint8_t varCode){
    uint8_t buffer[RX_SIZE_GET + 1];
    buffer[0] = RX_SIZE_GET;
    buffer[1] = RX_CODE_GET;
    buffer[2] = varCode;
    buffer[3] = 0;
    for(int i = 0; i < RX_SIZE_GET; i++){
        buffer[3] += buffer[i]; //checksum
    }
    for(int i = 0; i < RX_SIZE_GET + 1; i++){
        serialPutchar (fd, buffer[i]);
    }
}
/**
Ask for all PID variables. Respone will be processed in the UART reception thread
*/
void DsPIC::loadPID(){
    getVar(CODE_VAR_ALLPID);
}
/**
Clear content of UART reception buffer
*/
std::string DsPIC::async_read(){
    std::string s("");
    while (serialDataAvail(fd)){
      s += serialGetchar(fd);
    }
    return s;
}
/**
Wait for a complete packet ( size + header + content + checksum ) and return it as a vector of bytes.

return value : vector containing bytes of a complete packet
*/
 std::vector<uint8_t> DsPIC::readMsg(){
	int receiveByte = serialGetchar(fd);
 	while(receiveByte == -1){	//no reception during 10 sec
 		receiveByte = serialGetchar(fd);
 	}
 	uint8_t RxSize = receiveByte;	//first byte is the size of the packet
    std::vector<uint8_t> RxBuf;
    RxBuf.push_back(RxSize);

	for(int i = 0; i < RxSize; i++){	//receive full packet
    	receiveByte = serialGetchar(fd);
    	while(receiveByte == -1){
 			receiveByte = serialGetchar(fd);
 		}
        RxBuf.push_back(receiveByte);
    }
    return RxBuf;
}

void DsPIC::setX(double x){
	m_mutex.lock();
	this->x_ld = x;
	this->updatedX = true;
	m_mutex.unlock();
}
double DsPIC::getX(){
	m_mutex.lock();
	double x = this->x_ld;
	this->updatedX = false;
	m_mutex.unlock();
	return x;
}
bool DsPIC::isUpdatedX(){
	m_mutex.lock();
	bool updated = this->updatedX;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setY(double y){
	m_mutex.lock();
	this->y_ld = y;
	this->updatedY = true;
	m_mutex.unlock();
}
double DsPIC::getY(){
	m_mutex.lock();
	double y = this->y_ld;
	this->updatedY = false;
	m_mutex.unlock();
	return y;
}
bool DsPIC::isUpdatedY(){
	m_mutex.lock();
	bool updated = this->updatedY;
	m_mutex.unlock();
	return updated;
}
void DsPIC::setPos(double x, double y, double t){
    m_mutex.lock();
    this->x_ld = x;
    this->y_ld = y;
    this->t_ld = t;
    
    this->updatedX = true;
    this->updatedY = true;
    this->updatedT = true;
    m_mutex.unlock();
}

void DsPIC::setArrived(bool arrived){
    m_mutex.lock();
    this->arrived = arrived;
    this->updatedArrived = true;
    m_mutex.unlock();
}
bool DsPIC::getArrived(){
    m_mutex.lock();
    bool arrived = this->arrived;
    this->updatedArrived = false;
    m_mutex.unlock();
    return arrived;
}
bool DsPIC::isUpdatedArrived(){
    m_mutex.lock();
    bool updated = this->updatedArrived;
    m_mutex.unlock();
    return updated;
}

void DsPIC::setT(double t){
	m_mutex.lock();
	this->t_ld = t;
	this->updatedT = true;
	m_mutex.unlock();
}
double DsPIC::getT(){
	m_mutex.lock();
	double t = this->t_ld;
	this->updatedT = false;
	m_mutex.unlock();
	return t;
}
bool DsPIC::isUpdatedT(){
	m_mutex.lock();
	bool updated = this->updatedT;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setBat(float bat){
	m_mutex.lock();
	this->bat = bat;
	this->updatedBat = true;
	m_mutex.unlock();
}
float DsPIC::getBat(){
	m_mutex.lock();
	float bat = this->bat;
	this->updatedBat = false;
	m_mutex.unlock();
	return bat;
}
bool DsPIC::isUpdatedBat(){
	m_mutex.lock();
	bool updated = this->updatedBat;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setRupt(microswitch rupt){
	m_mutex.lock();
	this->rupt = rupt;
	this->updatedRupt = true;
	m_mutex.unlock();
}
microswitch DsPIC::getRupt(){
	m_mutex.lock();
	microswitch rupt = this->rupt;
	this->updatedRupt = false;
	m_mutex.unlock();
	return rupt;
}
bool DsPIC::isUpdatedRupt(){
	m_mutex.lock();
	bool updated = this->updatedRupt;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setUS(US us){
	m_mutex.lock();
	this->us = us;
	this->updatedUS = true;
	m_mutex.unlock();
}
US DsPIC::getUS(){
	m_mutex.lock();
	US us = this->us;
	this->updatedUS = false;
	m_mutex.unlock();
	return us;
}
bool DsPIC::isUpdatedUS(){
	m_mutex.lock();
	bool updated = this->updatedUS;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setPidSpeedLeft(pid p){
	m_mutex.lock();
	this->pidSpeedLeft = p;
	this->updatedPidSpeedLeft = true;
	m_mutex.unlock();
}
pid DsPIC::getPidSpeedLeft(){
	m_mutex.lock();
	pid p = this->pidSpeedLeft;
	this->updatedPidSpeedLeft = false;
	m_mutex.unlock();
	return p;
}
bool DsPIC::isUpdatedPidSpeedLeft(){
	m_mutex.lock();
	bool updated = this->updatedPidSpeedLeft;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setPidSpeedRight(pid p){
	m_mutex.lock();
	this->pidSpeedRight = p;
	this->updatedPidSpeedRight = true;
	m_mutex.unlock();
}
pid DsPIC::getPidSpeedRight(){
	m_mutex.lock();
	pid p = this->pidSpeedRight;
	this->updatedPidSpeedRight = false;
	m_mutex.unlock();
	return p;
}
bool DsPIC::isUpdatedPidSpeedRight(){
	m_mutex.lock();
	bool updated = this->updatedPidSpeedRight;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setPidDistance(pid p){
	m_mutex.lock();
	this->pidDistance = p;
	this->updatedPidDistance = true;
	m_mutex.unlock();
}
pid DsPIC::getPidDistance(){
	m_mutex.lock();
	pid p = this->pidDistance;
	this->updatedPidDistance = false;
	m_mutex.unlock();
	return p;
}
bool DsPIC::isUpdatedPidDistance(){
	m_mutex.lock();
	bool updated = this->updatedPidDistance;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setPidAngle(pid p){
	m_mutex.lock();
	this->pidAngle = p;
	this->updatedPidAngle = true;
	m_mutex.unlock();
}
pid DsPIC::getPidAngle(){
	m_mutex.lock();
	pid p = this->pidAngle;
	this->updatedPidAngle = false;
	m_mutex.unlock();
	return p;
}
bool DsPIC::isUpdatedPidAngle(){
	m_mutex.lock();
	bool updated = this->updatedPidAngle;
	m_mutex.unlock();
	return updated;
}

void DsPIC::setUpdatedAllPid(bool val){
	m_mutex.lock();
	this->updatedAllPid = val;
	m_mutex.unlock();
}
bool DsPIC::isUpdatedAllPid(){
	m_mutex.lock();
	bool b = this->updatedAllPid;
	m_mutex.unlock();
	return b;
}

void DsPIC::addLog(std::string str){
    m_mutex.lock();
	logs.push(str);
	updatedLogs = true;
	m_mutex.unlock();
}
void DsPIC::clearLogs(){
    m_mutex.lock();
	std::queue<std::string> empty;
    std::swap( logs, empty );
	m_mutex.unlock();
}
std::queue<std::string> DsPIC::getLogs(){
    m_mutex.lock();
    std::queue<std::string> logs = this->logs;
	updatedLogs = false;
	m_mutex.unlock();
    return logs;
}
bool DsPIC::isUpdatedLogs(){
    m_mutex.lock();
	bool b = this->updatedLogs;
	m_mutex.unlock();
	return b;
}

void DsPIC::addPlot(point p){
    m_mutex.lock();
	plots.push(p);
	updatedPlots = true;
	m_mutex.unlock();
}
void DsPIC::clearPlots(){
    m_mutex.lock();
	std::queue<point> empty;
    std::swap( plots, empty );
	m_mutex.unlock();
}
std::queue<point> DsPIC::getPlots(){
    m_mutex.lock();
    std::queue<point> plots = this->plots;
    updatedPlots = false;
	m_mutex.unlock();
    return plots;
}
bool DsPIC::isUpdatedPlots(){
    m_mutex.lock();
	bool b = this->updatedPlots;
	m_mutex.unlock();
	return b;
}
bool DsPIC::isContinueThread(){
    m_mutex.lock();
    bool b = m_continueThread;
    m_mutex.unlock();
    return b;
}
/**
Thread for handling dsPIC UART response

arg : 	<ptr> 		: 	pointer to instance of dspic
*/
void *thread_reception(void *ptr) {
    //std::cout << "DsPIC thread >Hello World!" << std::endl;
	DsPIC *dspic = (DsPIC*)ptr;
	while(dspic->isContinueThread()){
        std::vector<uint8_t> msg = dspic->readMsg();
        uint8_t checksum = 0;
        for(unsigned int i = 0; i < msg.size() - 1; i++){
            checksum += msg[i];
        }
        if(checksum != msg[msg.size() - 1]){
            //std::cout << "CHECKSUM ERROR !" << std::endl;
            DEBUG_DSPIC_PRINT("CHECKSUM ERROR !");
			/*std::cout << "CE dec :";
            for(unsigned int i = 0; i < msg.size(); i++){
                std::cout << " & [" << i << "] = " << (int)msg[i];
            }
			std::cout << std::endl << "CE char :";*/

            for(unsigned int i = 0; i < msg.size(); i++){
                if(msg[i] > 31 && msg[i] < 127)
                    std::cout << msg[i];
            }
        }
        else{
            if(msg.size() > 1){
                switch(msg[1]){	//message type
                    case 0 :    //error

                        break;
                    case TX_CODE_VAR :    //variable
                        switch(msg[2]){
							case CODE_VAR_BAT :{
                                if(msg.size() > 6){
                                    float vbat;
									float *ptr = &vbat;
									uint8_t *ptrChar = (uint8_t*)ptr;
									for(int i = 0; i < 4; i++){
										ptrChar[i] = msg[3+i];
									}
									//std::cout << "received from DsPIC : VBAT = " << vbat << std::endl;
                                    DEBUG_DSPIC_PRINT("VBAT = " << vbat);
									//dspic->bat = vbat;
									dspic->setBat(vbat);
                                }
							break;
							}
                            /*case CODE_VAR_X :
                                if(msg.size() > 4){
                                    dspic->x = ((msg[3] << 8) + msg[4]);
                                    //std::cout << "received from DsPIC : x = " << dspic->x << std::endl;
                                }
                                break;
                            case CODE_VAR_Y :
                                if(msg.size() > 4){
                                    dspic->y = ((msg[3] << 8) + msg[4]);
                                    //std::cout << "received from DsPIC : y = " << dspic->y << std::endl;
                                }
                                break;
                            case CODE_VAR_T :
                                if(msg.size() > 4){
                                    dspic->t = ((msg[3] << 8) + msg[4]);
                                    //std::cout << "received from DsPIC : t = " << dspic->t << " & H = " << (int)msg[3] << " & L = " << (int)msg[4] << std::endl;
                                }*/
                            case CODE_VAR_X_LD :
                                if(msg.size() > 8){
                                    double x_ld;
                                    double *ptr = &x_ld;
                                    uint8_t *ptrChar = (uint8_t*)ptr;
                                    for(int i = 0; i < 8; i++){
                                        ptrChar[i] = msg[3+i];
                                    }
                                    /*std::cout.precision(11);
                                    std::cout << "received from DsPIC : x_ld = " << x_ld << std::endl;
                                    std::cout.precision(6);*/
                                    //dspic->bat = vbat;
                                    //dspic->x_ld = x_ld;
									dspic->setX(x_ld);
                                }
                                break;
                            case CODE_VAR_Y_LD :
                                if(msg.size() > 8){
                                    double y_ld;
                                    double *ptr = &y_ld;
                                    uint8_t *ptrChar = (uint8_t*)ptr;
                                    for(int i = 0; i < 8; i++){
                                        ptrChar[i] = msg[3+i];
                                    }
                                    //dspic->y_ld = y_ld;
                                    dspic->setY(y_ld);
									/*std::cout.precision(11);
                                    std::cout << "received from DsPIC : y_ld = " << y_ld << std::endl;
                                    std::cout.precision(6);*/
                                    //dspic->bat = vbat;
                                }
                                break;
                            case CODE_VAR_T_LD :
                                if(msg.size() > 8){
                                    double t_ld;
                                    double *ptr = &t_ld;
                                    uint8_t *ptrChar = (uint8_t*)ptr;
                                    for(int i = 0; i < 8; i++){
                                        ptrChar[i] = msg[3+i];
                                    }
                                    //dspic->t_ld = t_ld;
                                    dspic->setT(t_ld);
									/*std::cout.precision(11);
                                    std::cout << "received from DsPIC : t_ld = " << t_ld << std::endl;
                                    std::cout.precision(6);*/
                                    //dspic->bat = vbat;
                                }
                                break;
                            case CODE_VAR_RUPT :
                                if(msg.size() > 4){
                                    //printf("msg[3] = %d / 0x01 = %d / msg[3] & 0x01 = %d\n",msg[3],0x01,msg[3] & 0x01);
                                    //printf("msg[3] = %d / 0x02 = %d / msg[3] & 0x02 = %d\n",msg[3],0x02,msg[3] & 0x02);
                                    //printf("msg[3] = %d / 0x04 = %d / msg[3] & 0x04 = %d\n",msg[3],0x04,msg[3] & 0x04);
                                    //printf("msg[3] = %d / 0x08 = %d / msg[3] & 0x08 = %d\n",msg[3],0x08,msg[3] & 0x08);

                                    /*dspic->rupt.ass0 = msg[3] & 0x01;
                                    dspic->rupt.ass1 = (msg[3] & 0x02) >> 1;
                                    dspic->rupt.ass2 = (msg[3] & 0x04) >> 2;
                                    dspic->rupt.ass3 = (msg[3] & 0x08) >> 3;
                                    dspic->rupt.act0 = (msg[3] & 0x10) >> 4;
                                    dspic->rupt.act1 = (msg[3] & 0x20) >> 5;
                                    dspic->rupt.act2 = (msg[3] & 0x40) >> 6;
                                    dspic->rupt.act3 = (msg[3] & 0x80) >> 7;
                                    dspic->rupt.act4 = msg[4] & 0x01;
                                    dspic->rupt.act5 = (msg[4] & 0x02) >> 1;*/

									microswitch rupt;
									rupt.ass0 = msg[3] & 0x01;
                                    rupt.ass1 = (msg[3] & 0x02) >> 1;
                                    rupt.ass2 = (msg[3] & 0x04) >> 2;
                                    rupt.ass3 = (msg[3] & 0x08) >> 3;
                                    rupt.act0 = (msg[3] & 0x10) >> 4;
                                    rupt.act1 = (msg[3] & 0x20) >> 5;
                                    rupt.act2 = (msg[3] & 0x40) >> 6;
                                    rupt.act3 = (msg[3] & 0x80) >> 7;
                                    rupt.act4 = msg[4] & 0x01;
                                    rupt.act5 = (msg[4] & 0x02) >> 1;
									dspic->setRupt(rupt);

                                    //std::cout << "received from DsPIC : rupt = H:" << (int)msg[3] << " & L: " << (int)msg[4] << " & " << dspic->rupt.ass0 << "/" << dspic->rupt.ass1 << "/" << dspic->rupt.ass2 << "/" << dspic->rupt.ass3 << "/" << dspic->rupt.act0 << "/" << dspic->rupt.act1 << "/" << dspic->rupt.act2 << "/" << dspic->rupt.act3 << "/" << dspic->rupt.act4 << "/" << dspic->rupt.act5 << "/"  << std::endl;
                                }
                                break;
                            case 100 :
                                if(msg.size() > 4){
                                    //dspic->US[0] = ((msg[3] << 8) + msg[4]);
                                    US us = dspic->getUS();
                                    us.US0 = ((msg[3] << 8) + msg[4]);
                                    dspic->setUS(us);
                                    //std::cout << "received from DsPIC : US[0] = " << dspic->US[0] << " (H = " << (int)msg[3] << " & L = " << (int)msg[4] << ")" << std::endl;
                                }
                                break;
                            case 101 :
                                if(msg.size() > 4){
                                    //dspic->US[1] = ((msg[3] << 8) + msg[4]);
                                    US us = dspic->getUS();
                                    us.US1 = ((msg[3] << 8) + msg[4]);
                                    dspic->setUS(us);
                                    //std::cout << "received from DsPIC : US[1] = " << dspic->US[1] << " (H = " << (int)msg[3] << " & L = " << (int)msg[4] << ")" << std::endl;
                                }
                                break;
                            case 102 :
                                if(msg.size() > 4){
                                    //dspic->US[2] = ((msg[3] << 8) + msg[4]);
                                    US us = dspic->getUS();
                                    us.US2 = ((msg[3] << 8) + msg[4]);
                                    dspic->setUS(us);
                                    //std::cout << "received from DsPIC : US[2] = " << dspic->US[2] << " (H = " << (int)msg[3] << " & L = " << (int)msg[4] << ")" << std::endl;
                                }
                                break;
                            case 103 :
                                if(msg.size() > 4){
                                    //dspic->US[3] = ((msg[3] << 8) + msg[4]);
                                    US us = dspic->getUS();
                                    us.US3 = ((msg[3] << 8) + msg[4]);
                                    dspic->setUS(us);
                                    //std::cout << "received from DsPIC : US[3] = " << dspic->US[3] << " (H = " << (int)msg[3] << " & L = " << (int)msg[4] << ")" << std::endl;
                                }
                                break;
                            case 104 :
                                if(msg.size() > 4){
                                    //dspic->US[4] = ((msg[3] << 8) + msg[4]);
                                    US us = dspic->getUS();
                                    us.US4 = ((msg[3] << 8) + msg[4]);
                                    dspic->setUS(us);
                                    //std::cout << "received from DsPIC : US[4] = " << dspic->US[4] << " (H = " << (int)msg[3] << " & L = " << (int)msg[4] << ")" << std::endl;
                                }
                                break;
                            case 105 :
                                if(msg.size() > 4){
                                    //dspic->US[5] = ((msg[3] << 8) + msg[4]);
                                    US us = dspic->getUS();
                                    us.US5 = ((msg[3] << 8) + msg[4]);
                                    dspic->setUS(us);
                                    //std::cout << "received from DsPIC : US[5] = " << dspic->US[5] << " (H = " << (int)msg[3] << " & L = " << (int)msg[4] << ")" << std::endl;
                                }
                                break;
							case CODE_VAR_P_SPEED_L :{
								//dspic->pidSpeedLeft.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidSpeedLeft();
								p.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidSpeedLeft(p);
								break;
							}
							case CODE_VAR_I_SPEED_L :{
								//dspic->pidSpeedLeft.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidSpeedLeft();
								p.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidSpeedLeft(p);
								break;
							}
							case CODE_VAR_D_SPEED_L :{
								//dspic->pidSpeedLeft.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidSpeedLeft();
								p.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidSpeedLeft(p);
								break;
							}
							
							case CODE_VAR_P_SPEED_R :{
								//dspic->pidSpeedRight.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidSpeedRight();
								p.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidSpeedRight(p);
								break;
							}
							case CODE_VAR_I_SPEED_R :{
								//dspic->pidSpeedRight.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidSpeedRight();
								p.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidSpeedRight(p);
								break;
							}
							case CODE_VAR_D_SPEED_R :{
								//dspic->pidSpeedRight.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidSpeedRight();
								p.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidSpeedRight(p);
								break;
							}

							case CODE_VAR_P_DISTANCE :{
								//dspic->pidDistance.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidDistance();
								p.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidDistance(p);
								break;
							}
							case CODE_VAR_I_DISTANCE :{
								//dspic->pidDistance.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidDistance();
								p.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidDistance(p);
								break;
							}
							case CODE_VAR_D_DISTANCE :{
								//dspic->pidDistance.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidDistance();
								p.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidDistance(p);
								break;
							}

							case CODE_VAR_P_ANGLE :{
								//dspic->pidAngle.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidAngle();
								p.Kp = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidAngle(p);
								break;
							}
							case CODE_VAR_I_ANGLE :{
								//dspic->pidAngle.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidAngle();
								p.Ki = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidAngle(p);
								break;
							}
							case CODE_VAR_D_ANGLE :{
								//dspic->pidAngle.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								pid p = dspic->getPidAngle();
								p.Kd = ((uint32_t)msg[3] << 24) + ((uint32_t)msg[4] << 16) + ((uint32_t)msg[5] << 8) + msg[6];
								dspic->setPidAngle(p);
								//dspic->isPIDUpdated = true;
								dspic->setUpdatedAllPid(true);
								break;
							}
                            case CODE_VAR_COEF_DISSYMETRY_LD:
                                if(msg.size() > 8){
                                    double var;
                                    double *ptr = &var;
                                    uint8_t *ptrChar = (uint8_t*)ptr;
                                    for(int i = 0; i < 8; i++){
                                        ptrChar[i] = msg[3+i];
                                    }
                                    //std::cout << "received from DsPIC : coef_Dissymetry_ld = " << var << std::endl;
                                    DEBUG_DSPIC_PRINT("received from DsPIC : coef_Dissymetry_ld = " << var);
                                }
                                break;
                            case CODE_VAR_MM_PER_TICKS_LD:
                                if(msg.size() > 8){
                                    double var;
                                    double *ptr = &var;
                                    uint8_t *ptrChar = (uint8_t*)ptr;
                                    for(int i = 0; i < 8; i++){
                                        ptrChar[i] = msg[3+i];
                                    }
                                    //std::cout << "received from DsPIC : mm_per_ticks_ld = " << var << std::endl;
                                    DEBUG_DSPIC_PRINT("received from DsPIC : mm_per_ticks_ld = " << var);
                                }
                                break;
                            case CODE_VAR_RAD_PER_TICKS_LD:
                                if(msg.size() > 8){
                                    double var;
                                    double *ptr = &var;
                                    uint8_t *ptrChar = (uint8_t*)ptr;
                                    for(int i = 0; i < 8; i++){
                                        ptrChar[i] = msg[3+i];
                                    }
                                    //std::cout << "received from DsPIC : rad_per_ticks_ld = " << var << std::endl;
                                    DEBUG_DSPIC_PRINT("received from DsPIC : rad_per_ticks_ld = " << var);
                                }
                                break;
                            default :
                                //std::cout << "Received wrong variable code from DsPIC : " << (int)msg[2] << std::endl;
                                DEBUG_DSPIC_PRINT("Received wrong variable code from DsPIC : " << (int)msg[2]);

                                break;

                        }
                        break;
                    case TX_CODE_LOG :{    //log
                        std::string s;
                        for(unsigned int i = 2; i < msg.size() - 1; i++)
                            s += msg[i];
                        //std::cout << "Received log from DsPIC : " << s << std::endl;
                        DEBUG_DSPIC_PRINT("Received log : " << s);
                        //dspic->logs.push(s);
                        dspic->addLog(s);
                        //sendMsg("l=" + s);
                        break;
                    }
                    case TX_CODE_PLOT :{    //plot
                        uint8_t id = msg[2];
                        //std::cout << "plot id = " << id << "/" << (int)msg[2] << std::endl;
                        uint32_t x = (msg[3] << 24) + (msg[4] << 16) + (msg[5] << 8) + msg[6];
                        int32_t y = (msg[7] << 24) + (msg[8] << 16) + (msg[9] << 8) + msg[10];
                        point p = {id, x, y};
                        //dspic->plots.push(p);
                        dspic->addPlot(p);
                        break;
                    }
                    default :
                        //std::cout << "Received wrong message code from DsPIC : " << msg[1] << std::endl;
                        DEBUG_DSPIC_PRINT("Received wrong message code from DsPIC : " << msg[1]);
                        break;
                }
            }
        }


            //std::cout << "DsPIC >>" << s << std::endl;

	   //delay(100);
   }
   DEBUG_DSPIC_PRINT("Thread ended");
   //std::cout << "Hello World!" << std::endl;
   pthread_exit(NULL);
}
