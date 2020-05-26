<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link href='styles.css' type='text/css' rel='stylesheet'>
	<title>CRIS Web Interface : Serial monitor</title>
	<script type="text/javascript" src="jquery-3.2.1.min.js"></script>
	<script type="text/javascript" src="canvasjs.min.js"></script>
	<script type="text/javascript" src="script.js?1"></script>
	<script src="createjs.min.js"></script>
	<script src="hammer.min.js"></script>
</head>
<body>
	<!-- Onglets -->
	<ul id="tab-nav">
		 <li><a href="#" data-tab="onglet1" class="tab-nav-active">Control</a></li>
		 <li><a href="#" data-tab="onglet2">Graph</a></li>
		 <li><a href="#" data-tab="onglet3">Terminal</a></li>
		 <li><a href="#" data-tab="onglet4">PID Tuning</a></li>
		 <li><a href="#" data-tab="onglet5">Debug</a></li>
		 <li><a href="#" data-tab="onglet6">Settings</a></li>
	</ul>
	
	<!-- Contenu des onglets -->
	<div id="onglet1" class="tab tab-active">
		<div class="flexContainer">
			<div id="flexChild_Actuators" class="flexChild_Control">
				<div id="FlexChild_Actuators_Title">
					Actuators
				</div>
				<div>
					<button class="btn carte0" id="Carte0" onclick="CARTE0()">Actionneur</button>
					<button class="btn carte1" id="Carte1" onclick="CARTE1()">Scara</button>
					<button class="btn carte2" id="Carte2" onclick="CARTE2()">XBee</button>
					<br><br>
					<img id="Carte" alt="Act">
					<br><br>
					<button onclick="funAction()" class="btn_fun">Fun action</button>
					<br><br>
					<!-- Slideshow container -->
					<div class="slideshow-container">
						<!-- The dots/circles -->
						<div style="text-align:center">
					  		<span class="dot" onclick="currentSlide(1)"></span>
					  		<span class="dot" onclick="currentSlide(2)"></span>
					  		<span class="dot" onclick="currentSlide(3)"></span>
					  		<span class="dot" onclick="currentSlide(4)"></span>
					  		<span class="dot" onclick="currentSlide(5)"></span>
						</div>
					 	<!-- Full-width images with number and caption text -->
					  	<div class="mySlides fade">
					  		<p>Sermotor</p>
							<table>
								<tr>
									<td>
										0
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo0" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo0" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo0Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										1
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo1" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo1" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo1Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										2
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo2" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo2" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo2Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										3
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo3" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo3" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo3Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										4
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo4" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo4" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo4Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										5
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo5" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo5" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo5Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										6
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo6" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo6" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo6Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										7
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo7" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo7" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo7Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										8
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo8" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo8" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo8Custom" type="text" value="800"/>
									</td>
								</tr>
								<tr>
									<td>
										9
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="servo9" min="544" max="2400" value="800" class="sliderServo" id = "sliderServo9" onchange="TactilServo()">
										</div>
									</td>
									<td>
										<input class="inputText_servoCustom" id="inputText_servo9Custom" type="text" value="800"/>
									</td>
								</tr>
							</table>
						</div>
						<div class="mySlides fade">
							<p>Motor</p>
							<table>
								<tr>
									<td>
									</td>
									<td>
										0
									</td>
									<td>
										1
									</td>
								</tr>
								<tr>
									<td>
										0
									</td>
									<td>
										<input type="radio" name="motor0" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor0" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										1
									</td>
									<td>
										<input type="radio" name="motor1" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor1" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										2
									</td>
									<td>
										<input type="radio" name="motor2" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor2" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										3
									</td>
									<td>
										<input type="radio" name="motor3" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor3" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										4
									</td>
									<td>
										<input type="radio" name="motor4" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor4" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										5
									</td>
									<td>
										<input type="radio" name="motor5" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor5" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										6
									</td>
									<td>
										<input type="radio" name="motor6" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor6" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										7
									</td>
									<td>
										<input type="radio" name="motor7" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor7" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										8
									</td>
									<td>
										<input type="radio" name="motor8" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor8" value="1" class="radioMotor">
									</td>
								</tr>
								<tr>
									<td>
										9
									</td>
									<td>
										<input type="radio" name="motor9" value="0" class="radioMotor" checked="checked">
									</td>
									<td>
										<input type="radio" name="motor9" value="1" class="radioMotor">
									</td>
								</tr>
							</table>
						</div>
					  	<div class="mySlides fade">
					  		<p>Motor 4 Q</p>
					  		<table>
								<tr>
									<td>
										0
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q0" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor0" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q0Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										1
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q1" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor1" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q1Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										2
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q2" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor2" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q2Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										3
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q3" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor3" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q3Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										4
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q4" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor4" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q4Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										5
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q5" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor5" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q5Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										6
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q6" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor6" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q6Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										7
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q7" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor7" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q7Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										8
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q8" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor8" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q8Custom" type="text" value="0"/>
									</td>
								</tr>
								<tr>
									<td>
										9
									</td>
									<td>
										<div class="slidecontainer">
  											<input type="range" name="motor4q9" min="0" max="1000" value="0" class="sliderMotor" id = "sliderMotor9" onchange="TactilMotor()">
										</div>
									</td>
									<td>
										<input class="inputText_motorCustom" id="inputText_motor4q9Custom" type="text" value="0"/>
									</td>
								</tr>
							</table>
					  	</div>
					  	<div class="mySlides fade">
					  		<p>AX12</p>
					  		<div class="center-align">
      							<canvas id="joystickAx" height="300" width="300" onclick="JoyDragDownAx()" onclose="JoyDragUpAx()"></canvas>
    						</div>
					  	</div>
					  	<div class="mySlides fade">
					  		<p>Recoder</p>
					  		<div class="center-align">
					  			<table>
					  				<tr>
					  					<td></td>
					  					<td>
					  						<button onclick="Rec()" class="btn_rec" id="RecBtn">Record</button>
					  					</td>
					  				</tr>
					  				<tr>
					  					<td>
					  						<p>Nom (string)</p>
					  					</td>
					  					<td>
					  						<input class="inputText_recCustom" id="inputText_NomCustom" type="text" value=""/>
					  					</td>
					  				</tr>
					  				<tr>
					  					<td>
					  						<p>Id (char)</p>
					  					</td>
					  					<td>
					  						<input class="inputText_recCustom" id="inputText_IdCustom" type="text" value=""/>
					  					</td>
					  				</tr>
					  				<tr>
					  					<td>
					  						<p>Score (nb/element)</p>
					  					</td>
					  					<td>
					  						<input class="inputText_recCustom" id="inputText_SocreCustom" type="text" value=""/>
					  					</td>
					  				</tr>
					  				<tr>
					  					<td>
					  						<span title="(-1 déposable, 0 élément fixe, >0 stock)">
					  							<p>Dépot ?</p>
					  						</span>
					  					</td>
					  					<td>
					  						<span title="(-1 déposable, 0 élément fixe, >0 stock)">
					  							<input class="inputText_recCustom" id="inputText_DepotCustom" type="text" value=""/>
					  						</span>
					  					</td>
					  				</tr>
					  				
					  			</table>
					  		</div>
					  	</div>	
					</div>
				</div>
			</div>
			<div id="flexChild_Sensors" class="flexChild_Control">
				<div id="flexChild_Sensors_Title">
					Sensors
				</div>
				<div>
					Battery<br/><br/>
					Voltage : <input class="outputText" id="outputText_BatteryVoltage" type="text" value=""/> V<br/>
					Percentage : <input class="outputText" id="outputText_BatteryPercentage" type="text" value=""/> %<br/>
					<br/><br/>Ultrasonic<br/><br/>
					1 : <input class="outputText" id="outputText_US1" type="text" value=""/> cm<br/>
					2 : <input class="outputText" id="outputText_US2" type="text" value=""/> cm<br/>
					3 : <input class="outputText" id="outputText_US3" type="text" value=""/> cm<br/>
					4 : <input class="outputText" id="outputText_US4" type="text" value=""/> cm<br/>
					5 : <input class="outputText" id="outputText_US5" type="text" value=""/> cm<br/>
					6 : <input class="outputText" id="outputText_US6" type="text" value=""/> cm<br/>
					<br/><br/>Microswitch<br/><br/>
					Asserv'<br/><br/>
					<table>
						<tr>
							<td>1 : </td>
							<td><div class="led-green" id="led_rupt1"></div></td>
						</tr>
						<tr>
							<td>2 : </td>
							<td><div class="led-red" id="led_rupt2"></div></td>
						</tr>
						<tr>
							<td>3 : </td>
							<td><div class="led-green" id="led_rupt3"></div></td>
						</tr>
						<tr>
							<td>4 : </td>
							<td><div class="led-red" id="led_rupt4"></div></td>
						</tr>
					</table>
					<br/><br/>Actuators<br/><br/>
					<table>
						<tr>
							<td>1 : </td>
							<td><div class="led-green" id="led_rupt5"></div></td>
						</tr>
						<tr>
							<td>2 : </td>
							<td><div class="led-green" id="led_rupt6"></div></td>
						</tr>
						<tr>
							<td>3 : </td>
							<td><div class="led-green" id="led_rupt7"></div></td>
						</tr>
						<tr>
							<td>4 : </td>
							<td><div class="led-red" id="led_rupt8"></div></td>
						</tr>
						<tr>
							<td>5 : </td>
							<td><div class="led-red" id="led_rupt9"></div></td>
						</tr>
						<tr>
							<td>6 : </td>
							<td><div class="led-red" id="led_rupt10"></div></td>
						</tr>
					</table>
				</div>
			</div>
			<div id="flexChild_PlayingArea" class="flexChild_Control">
			<!-- <img src="terrain_couleur.png" alt="terrain de jeu"> -->
				<canvas id="canvasPlayingArea" width="873" height="588"></canvas>
			</div>

			<div id="flexChild_Movement" class="flexChild_Control">
				<div id="flexChild_Movement_Title">
					Movement
				</div>
				<br>
				<div>
					<table>
						<tr>
							<td>
							</td>
							<td>
								Position
							</td>
							<td>
								Destination
							</td>
							
						</tr>
						<tr>
							<td>
								X
							</td>
							<td>
								<input class="outputText" id="outputText_posX" type="text" value=""/>
							</td>
							<td>
								<input class="inputText"Text" id="inputText_destX" type="text" value=""/>
							</td>
							
						</tr>
						<tr>
							<td>
								Y
							</td>
							<td>
								<input class="outputText" id="outputText_posY" type="text" value=""/>
							</td>
							<td>
								<input class="inputText"Text" id="inputText_destY" type="text" value=""/>
							</td>
						</tr>
					</table>
					<input type="button" id="button_go" value="Go">
					<table>
						<tr>
							<td>
							</td>
							<td>
								Position
							</td>
							<td>
								Destination
							</td>
							
						</tr>
						<tr>
							<td>
								T
							</td>
							<td>
								<input class="outputText" id="outputText_posT" type="text" value=""/>
							</td>
							<td>
								<input class="inputText"Text" id="inputText_destT" type="text" value=""/>
							</td>
						</tr>
					</table>
					<input type="button" id="button_turn" value="Go">
				</div>
				<br><br><br><br><br><br><br><br><br><br><br><br>
				<div>Joystick</div>
				<div class="center-align">
					<br>
      				<canvas id="joystick" height="300" width="300"></canvas>
    			</div>
			</div>

		</div>
	</div>
	<div id="onglet2" class="tab">
		<div id="chartContainer"></div>
		<input type="button" id="Button_ClearChart"value="Clear chart">
		<br/><input type="button" id="bt_downloadCharts"value="Download Charts">
	</div>
	<div id="onglet3" class="tab">
		<h1>CRIS Serial Monitor</h1>
		<span>Received : </span><br/>
		<textarea id="output" class="output" rows="20" cols="100" disabled="disabled"></textarea>
		<textarea id="output2" class="output" rows="20" cols="100" disabled="disabled"></textarea><br/>
		<span>Send : </span><input id="inputText" type="text"/><br/>
		<input type="button" id="Button_ClearTerminal"value="Clear"><br/>
		<input type="button" id="Button_DownloadLogTerminal"value="Download Log Terminal">
	</div>
	<div id="onglet4" class="tab">
		<br/>
		Coef Dissymetry : <input type="number" min="0" max="2" step="0.0001" value="1" id="numberOdo1" script=""/><input type="button" id="ButtonSendOdo1" value="Send"><br/>
		micrometer per tick : <input type="number" min="0" max="100" step="0.001" value="18.86" id="numberOdo2"/><input type="button" id="ButtonSendOdo2" value="Send"><br/>
		distance between encoder wheels (mm): <input type="number" min="0" max="1000" step="0.001" value="300" id="numberOdo3"/><input type="button" id="ButtonSendOdo3" value="Send"><br/>
		<br/>
		<input type="button" id="ButtonClearChartsPID"value="Clear charts"><br/>
		<input type="button" id="ButtonLoadPID"value="Load PID"><br/>
		<input type="button" id="ButtonSendPID"value="Save PID"><br/>
		<div id="chartPID1"></div>
		<div class="slidecontainer">
		  <p>Left Speed:</p>
		  P : <input type="number" min="0" max="100" step="0.1" value="0" id="numberP1"/><br/>	<!--<input class="outputText" id="outputValueP1" type="text" value="0"/><input type="range" min="0" step="0.1" max="100" value="0" id="sliderP1"><br/>-->
		  I : <input type="number" min="0" max="100" step="0.1" value="0" id="numberI1"/><br/>	<!--<input class="outputText" id="outputValueI1" type="text" value="0"/><input type="range" min="0" max="100" value="0" id="sliderI1"><br/>-->
		  D : <input type="number" min="0" max="100" step="0.1" value="0" id="numberD1"/><br/>	<!--<input class="outputText" id="outputValueD1" type="text" value="0"/><input type="range" min="0" max="100" value="0" id="sliderD1"><br/>-->
		</div>
		<div id="chartPID2"></div>
		<div class="slidecontainer">
		  <p>Right Speed:</p>
		  P : <input type="number" min="0" max="100" step="0.1" value="0" id="numberP2"/><br/>
		  I : <input type="number" min="0" max="100" step="0.1" value="0" id="numberI2"/><br/>
		  D : <input type="number" min="0" max="100" step="0.1" value="0" id="numberD2"/><br/>
		</div>
		<div id="chartPID3"></div>
		<div class="slidecontainer">
		  <p>Right Speed:</p>
		  P : <input type="number" min="0" max="100" step="0.1" value="0" id="numberP3"/><br/>
		  I : <input type="number" min="0" max="100" step="0.1" value="0" id="numberI3"/><br/>
		  D : <input type="number" min="0" max="100" step="0.1" value="0" id="numberD3"/><br/>
		</div>
		<div id="chartPID4"></div>
		<div class="slidecontainer">
		  <p>Right Speed:</p>
		  P : <input type="number" min="0" max="100" step="0.1" value="0" id="numberP4"/><br/>
		  I : <input type="number" min="0" max="100" step="0.1" value="0" id="numberI4"/><br/>
		  D : <input type="number" min="0" max="100" step="0.1" value="0" id="numberD4"/><br/>
		</div>
		
	</div>
	<div id="onglet5" class="tab">
		Onglet 5
	</div>
	<div id="onglet6" class="tab">
		Request refresh rate : <input id="inputText_requestRefreshRate" type="text" value="20" style="width:30px;"/> ms <input type="checkbox" id="checkbox_request" checked>Send request<br><br/>
		Chart refresh rate : <input id="inputText_chartRrefreshRate" type="text" value="50" style="width:30px;"/> ms <input type="checkbox" id="checkbox_chart" checked> Update chart<br>
		UI refresh rate : <input id="inputText_UIRefreshRate" type="text" value="20" style="width:30px;"/> ms <input type="checkbox" id="checkbox_UI" checked>Update UI<br><br/>
		Canvas refresh rate : <input id="inputText_canvasRrefreshRate" type="text" value="50" style="width:30px;"/> ms <input type="checkbox" id="checkbox_canvas" checked> Update canvas<br>
		Terminal : <br/>
		&nbsp&nbsp <input type="checkbox" id="checkbox_logAll"> Log all communication<br/>
		&nbsp&nbsp <input type="checkbox" id="checkbox_logPosition"> Log position<br/>
		&nbsp&nbsp <input type="checkbox" id="checkbox_logCurves"> Log curves<br/>
		&nbsp&nbsp <input type="checkbox" id="checkbox_logSensors"> Log sensors<br/>
		&nbsp&nbsp <input type="checkbox" id="checkbox_logActuators"> Log actuators<br/>
	</div>



</body>
</html>
