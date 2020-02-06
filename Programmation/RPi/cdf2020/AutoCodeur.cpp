#include "AutoCodeur.hpp"

void Codeur(int nb_code) {
	std::ifstream myfileI;
	myfileI.open("Codeur.txt");
	if(!myfileI) {
        std::cout << "no file" << std::endl;
        return;
	}
	char inChar = '\0';
	int CptFile = 0;
	do {
		myfileI >> inChar;
		if(inChar==NEXT) {
			CptFile++;
		}
		if(inChar==END) {
			std::cout << "error no prgm" << std::endl;
			return;
		}
	}while(CptFile!=nb_code);
	CptFile = 1;
	myfileI >> inChar;
	while(inChar!=END || inChar!=NEXT) {
		switch(inChar) {
			case PUMP: {
                myfileI >> inChar;
                inChar -= 48;
				if(inChar) {
                    myfileI >> inChar;
                    inChar -= 48;
					if(inChar < 3) {
						actFront.SetPump(inChar,1);
						//std::cout << "actFront.SetPump(" << (int)(inChar) << ",1);" << std::endl;
					} else {
						actBack.SetPump(inChar-3,1);
						//std::cout << "actBack.SetPump(" << (int)(inChar-3) << ",1);" << std::endl;
					}
				} else {
                    myfileI >> inChar;
                    inChar -= 48;
					if(inChar < 3) {
						actFront.SetPump(inChar,0);
						//std::cout << "actFront.SetPump(" << (int)(inChar) << ",1);" << std::endl;
					} else {
						actBack.SetPump(inChar-3,0);
						//std::cout << "actBack.SetPump(" << (int)(inChar-3) << ",1);" << std::endl;
					}
				}
				}break;
			case SERVO: {
                myfileI >> inChar;
                inChar -= 48;
				if(inChar) {
                    myfileI >> inChar;
                    inChar -= 48;
					if(inChar < 3) {
						actFront.MoveServo(inChar,SERVO_VALUE_HIGH);
						//std::cout << "actFront.MoveServo(" << (int)(inChar) << ",SERVO_VALUE_HIGH);" << std::endl;
					} else {
						actBack.MoveServo(inChar-3,SERVO_VALUE_HIGH);
						//std::cout << "actBack.MoveServo(" << (int)(inChar-3) << ",SERVO_VALUE_HIGH);" << std::endl;
					}
				} else {
                    myfileI >> inChar;
                    inChar -= 48;
					if(inChar < 3) {
						actFront.MoveServo(inChar,SERVO_VALUE_LOW);
						//std::cout << "actFront.MoveServo(" << (int)(inChar) << ",SERVO_VALUE_LOW);" << std::endl;
					} else {
						actBack.MoveServo(inChar-3,SERVO_VALUE_LOW);
						//std::cout << "actBack.MoveServo(" << (int)(inChar-3) << ",SERVO_VALUE_LOW);" << std::endl;
					}
				}
				}break;
			case MOT_LIN: {
                myfileI >> inChar;
                inChar -= 48;
				if(inChar) {
					dspic.setMotLin(1);
					//std::cout << "dspic.setMotLin(1);" << std::endl;
				} else {
					dspic.setMotLin(0);
					//std::cout << "dspic.setMotLin(0);" << std::endl;
				}
				}break;
			case MOVE: {
				int x_cod = 0, y_cod = 0;
				//x
                myfileI >> inChar;
                inChar -= 48;
				x_cod = 100*inChar;
                myfileI >> inChar;
                inChar -= 48;
				x_cod += 10*inChar;
                myfileI >> inChar;
                inChar -= 48;
				x_cod += inChar;
				//y
                myfileI >> inChar;
                inChar -= 48;
				y_cod = 100*inChar;
                myfileI >> inChar;
                inChar -= 48;
				y_cod += 10*inChar;
                myfileI >> inChar;
                inChar -= 48;
				y_cod += inChar;
				//code
                myfileI >> inChar;
                inChar -= 48;
				if(inChar) {
					dspic->go(x_cod,y_cod,0,1);
					//std::cout << "dspic->go(" << (int)(x_cod) << "," << (int)(y_cod) << ",0,1);" << std::endl;
				} else {
					dspic->go(x_cod,y_cod,0,0);
					//std::cout << "dspic->go(" << (int)(x_cod) << "," << (int)(y_cod) << ",0,0);" << std::endl;
				}
				}break;
			case END:
			case NEXT: {
				std::cout << "fin" << std::endl;
				return;
				}break;
			default: {
				std::cout << "error readfile : " << CptFile << std::endl;
				}break;
		}
        myfileI >> inChar;
        CptFile++;
	}
}



void Record(void) {
	//init
	char state_motLin = 1;
	char state_servo1 = 1;
	char state_servo2 = 1;
	char state_servo3 = 1;
	char state_servo4 = 1;
	char state_servo5 = 1;
	char state_servo6 = 1;
	char state_pump1 = 1;
	char state_pump2 = 1;
	char state_pump3 = 1;
	char state_pump4 = 1;
	char state_pump5 = 1;
	char state_pump6 = 1;
	char x_cod = 0;
	int x_codT = 0;
	char y_cod = 0;
	int y_codT = 0;
    char x100, x10, x1;
    char y100, y10, y1;

	dspic.setMotLin(0);
	actFront.SetPump(0,0);
	actFront.SetPump(1,0);
	actFront.SetPump(2,0);
	actBack.SetPump(0,0);
	actBack.SetPump(1,0);
	actBack.SetPump(2,0);
	actFront.MoveServo(0,SERVO_VALUE_LOW);
	actFront.MoveServo(1,SERVO_VALUE_LOW);
	actFront.MoveServo(2,SERVO_VALUE_LOW);
	actBack.MoveServo(0,SERVO_VALUE_LOW);
	actBack.MoveServo(1,SERVO_VALUE_LOW);
	actBack.MoveServo(2,SERVO_VALUE_LOW);
    std::cout << "X = " << dspic.getX() << std::endl;
    std::cout << "Y = " << dspic.getY() << std::endl;
    std::cout << "T = " << dspic.getT() << std::endl;
    dspic.initPos(dspic.getX(),dspic.getY(),dspic.getT());

    x_codT = dspic.getX();
    y_codT = dspic.getY();


    //code
	std::ofstream myfileO;
    myfileO.open("Codeur.txt");
    if(!myfileO) {
        std::cout << "no file" << std::endl;
        return;
    }
    std::cout << "record ON ('f' to skip)" << std::endl;
    char inChar = '\0';
    myfileO << "N\n";
    do {
    	std::cout << ">";
    	//inChar = _getch();
        cin >> inChar;
    	std::cout << inChar <<std::endl;
    	switch(inChar) {
    		case KEY_MOTLIN:
    			if(state_motLin) {
    				myfileO << MOT_LIN <<"1\n";
    			} else {
    				myfileO << MOT_LIN << "0\n";
    			}
    			dspic.setMotLin(state_motLin);
    			state_motLin = !state_motLin;
    			break;
    		case KEY_SERVO0:
    			if(state_servo1) {
    				myfileO << SERVO << "10\n";
    				actFront.MoveServo(0,SERVO_VALUE_HIGH);
    			} else {
    				myfileO << SERVO << "00\n";
    				actFront.MoveServo(0,SERVO_VALUE_LOW);
    			}
    			state_servo1 = !state_servo1;
    			break;
    		case KEY_SERVO1:
    			if(state_servo2) {
    				myfileO << SERVO << "11\n";
    				actFront.MoveServo(1,SERVO_VALUE_HIGH);
    			} else {
    				myfileO << SERVO << "01\n";
    				actFront.MoveServo(1,SERVO_VALUE_LOW);
    			}
    			state_servo2 = !state_servo2;
    			break;
    		case KEY_SERVO2:
    			if(state_servo3) {
    				myfileO << SERVO << "12\n";
    				actFront.MoveServo(2,SERVO_VALUE_HIGH);
    			} else {
    				myfileO << SERVO << "02\n";
    				actFront.MoveServo(2,SERVO_VALUE_LOW);
    			}
    			state_servo3 = !state_servo3;
    			break;
    		case KEY_SERVO3:
    			if(state_servo4) {
    				myfileO << SERVO << "13\n";
    				actBack.MoveServo(0,SERVO_VALUE_HIGH);
    			} else {
    				myfileO << SERVO << "03\n";
    				actBack.MoveServo(0,SERVO_VALUE_LOW);
    			}
    			state_servo4 = !state_servo4;
    			break;
    		case KEY_SERVO4:
    			if(state_servo5) {
    				myfileO << SERVO << "14\n";
    				actBack.MoveServo(1,SERVO_VALUE_HIGH);
    			} else {
    				myfileO << SERVO << "04\n";
    				actBack.MoveServo(1,SERVO_VALUE_LOW);
    			}
    			state_servo5 = !state_servo5;
    			break;
    		case KEY_SERVO5:
    			if(state_servo6) {
    				myfileO << SERVO << "15\n";
    				actBack.MoveServo(2,SERVO_VALUE_HIGH);
    			} else {
    				myfileO << SERVO << "05\n";
    				actBack.MoveServo(2,SERVO_VALUE_LOW);
    			}
    			state_servo6 = !state_servo6;
    			break;
    		case KEY_PUMP0:
    			if(state_pump1) {
    				myfileO << PUMP << "10\n";
    			} else {
    				myfileO << PUMP << "00\n";
    			}
    			actFront.SetPump(0,state_pump1);
    			state_pump1 = !state_pump1;
    			break;
    		case KEY_PUMP1:
    			if(state_pump2) {
    				myfileO << PUMP << "11\n";
    			} else {
    				myfileO << PUMP << "01\n";
    			}
    			actFront.SetPump(1,state_pump2);
    			state_pump2 = !state_pump2;
    			break;
    		case KEY_PUMP2:
    			if(state_pump3) {
    				myfileO << PUMP << "12\n";
    			} else {
    				myfileO << PUMP << "02\n";
    			}
    			actFront.SetPump(2,state_pump3);
    			state_pump3 = !state_pump3;
    			break;
    		case KEY_PUMP3:
    			if(state_pump4) {
    				myfileO << PUMP << "13\n";
    			} else {
    				myfileO << PUMP << "03\n";
    			}
    			actBack.SetPump(0,state_pump4);
    			state_pump4 = !state_pump4;
    			break;
    		case KEY_PUMP4:
    			if(state_pump5) {
    				myfileO << PUMP << "14\n";
    			} else {
    				myfileO << PUMP << "04\n";
    			}
    			actBack.SetPump(1,state_pump5);
    			state_pump5 = !state_pump5;
    			break;
    		case KEY_PUMP5:
    			if(state_pump6) {
    				myfileO << PUMP << "15\n";
    			} else {
    				myfileO << PUMP << "05\n";
    			}
    			actBack.SetPump(2,state_pump6);
    			state_pump6 = !state_pump6;
    			break;
    		case KEY_MOVE:
    			myfileO << MOVE;
    			std::cout << "x = ";
    			//x_cod = inChar = _getch();
    			myfileO << x_cod;
    			x_codT = x_cod*100;
    			std::cout << x_cod;
    			//x_cod = inChar = _getch();
    			myfileO << x_cod;
    			x_codT += x_cod*10;
    			std::cout << x_cod;
    			//x_cod = inChar = _getch();
    			myfileO << x_cod;
    			x_codT += x_cod;
    			std::cout << x_cod;

    			std::cout << std::endl << "y = ";
    			//y_cod = inChar = _getch();
    			myfileO << y_cod;
    			y_codT = y_cod*100;
    			std::cout << y_cod;
    			//y_cod = inChar = _getch();
    			myfileO << y_cod;
    			y_codT += y_cod*10;
    			std::cout << y_cod;
    			//y_cod = inChar = _getch();
    			myfileO << y_cod;
    			y_codT += y_cod;
    			std::cout << y_cod;
    			std::cout << std::endl;

    			myfileO << "0\n";
    			dspic->go(x_codT,y_codT,0,0);
    			break;
            case KEY_FRONT:
                x_codT += 20;
                x100 = int(x_codT/100);
                x10 = int((x_codT-x100*100)/10);
                x1 = int(x_codT-x100*100-x10*10);

                y100 = int(y_codT/100);
                y10 = int((y_codT-y100*100)/10);
                y1 = int(y_codT-y100*100-y10*10);

                myfileO << KEY_MOVE << (x100+48) << (x10+48) << (x1+48) << (y100+48) << (y10+48) << (y1+48) << "0\n";
                dspic->go(x_codT,y_codT,0,0);
                break;
            case KEY_BACK:
                x_codT -= 20;
                x100 = int(x_codT/100);
                x10 = int((x_codT-x100*100)/10);
                x1 = int(x_codT-x100*100-x10*10);

                y100 = int(y_codT/100);
                y10 = int((y_codT-y100*100)/10);
                y1 = int(y_codT-y100*100-y10*10);

                myfileO << KEY_MOVE << (x100+48) << (x10+48) << (x1+48) << (y100+48) << (y10+48) << (y1+48) << "0\n";
                dspic->go(x_codT,y_codT,0,0);
                break;
            case KEY_RIGHT:
                y_codT -= 20;
                x100 = int(x_codT/100);
                x10 = int((x_codT-x100*100)/10);
                x1 = int(x_codT-x100*100-x10*10);

                y100 = int(y_codT/100);
                y10 = int((y_codT-y100*100)/10);
                y1 = int(y_codT-y100*100-y10*10);

                myfileO << KEY_MOVE << (x100+48) << (x10+48) << (x1+48) << (y100+48) << (y10+48) << (y1+48) << "0\n";
                dspic->go(x_codT,y_codT,0,0);
                break;
            case KEY_LEFT:
                y_codT += 20;
                x100 = int(x_codT/100);
                x10 = int((x_codT-x100*100)/10);
                x1 = int(x_codT-x100*100-x10*10);

                y100 = int(y_codT/100);
                y10 = int((y_codT-y100*100)/10);
                y1 = int(y_codT-y100*100-y10*10);

                myfileO << KEY_MOVE << (x100+48) << (x10+48) << (x1+48) << (y100+48) << (y10+48) << (y1+48) << "0\n";
                dspic->go(x_codT,y_codT,0,0);
                break;
    		case KEY_NEXT:
    			myfileO << "N\n";
    		case KEY_END:
    			//do nothing
    			break;
    		default:
    			std::cout << "erro input" << std::endl;
    			break;
    	}
    } while(inChar!=KEY_END);
    std::cout << "record OFF (enter to continue)" << std::endl;
    getchar();
    myfileO << END;
	myfileO.close();
}
