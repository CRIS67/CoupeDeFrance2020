var COEF_SCALE_COEF_DISSYMETRY = 10000;
var COEF_SCALE_MM_PER_TICKS = 1000;
var COEF_SCALE_DISTANCE_BETWEEN_ENCODER_WHEELS = 1000;
var COEF_SCALE_PID = 1000000;

var SIZE_LIDAR_POINTS_CIRCULAR = 20;

var lidarPoints = [];
var lidarPointsCircular = [];
var iLidar = 0;

window.onload = function () {
		/*variables*/
		var dps = [];   //dataPoints. 
		var dps2 = [];
		var dps3 = [];
		var dps4 = [];
		var dps5 = [];
		var dps6 = [];
		var dps7 = [];
		var dps8 = [];
		var dps9 = [];
		
		var dps11 = [];	//dataPoints PID1 : 1x -> PID1 / x1 -> courbe 1 => 14 -> PID1 courbe 4 
		var dps12 = [];
		var dps13 = [];
		var dps14 = [];
		var dps15 = [];
		var dps16 = [];
		var dps17 = [];
		var dps18 = [];
		var dps19 = [];
		
		var dps21 = [];	//dataPoints PID2 : 1x -> PID2 / x2 -> courbe 2 => 25 -> PID2 courbe 5 
		var dps22 = [];
		var dps23 = [];
		var dps24 = [];
		var dps25 = [];
		var dps26 = [];
		var dps27 = [];
		var dps28 = [];
		var dps29 = [];
		
		var dps31 = [];	//dataPoints PID3 : 3x -> PID3 / x3 -> courbe 3 => 36 -> PID3 courbe 6 
		var dps32 = [];
		var dps33 = [];
		var dps34 = [];
		var dps35 = [];
		var dps36 = [];
		var dps37 = [];
		var dps38 = [];
		var dps39 = [];
		
		var dps41 = [];	//dataPoints PID4 : 4x -> PID4 / x9 -> courbe 9 => 47 -> PID4 courbe 7 
		var dps42 = [];
		var dps43 = [];
		var dps44 = [];
		var dps45 = [];
		var dps46 = [];
		var dps47 = [];
		var dps48 = [];
		var dps49 = [];
		
		var xVal = dps.length + 1;
		var yVal = 15;
		var buffer = "";
		var tCMD =  ["","","","","","","","","",""];
		var nCMD = 0;
		
		var xRobot1 = -100;
		var yRobot1 = -100;
		var tRobot1 = 0;
		var xRobot2 = 0;
		var yRobot2 = 0;
		var tRobot2 = 0;
		
		var requestInterval = setInterval(function(){xhrRequest();}, 100);
		var chartInterval;// = setInterval(function(){chart.render();}, 50); 
		var chartIntervalPID;
		var canvasInterval;
		var UIInterval;
		
		var ctx = document.getElementById('canvasPlayingArea').getContext('2d');
		var img = new Image();
			img.onload = function() {
			ctx.drawImage(img, 0, 0);
			/*ctx.beginPath();
			ctx.moveTo(30, 96);
			ctx.lineTo(70, 66);
			ctx.lineTo(103, 76);
			ctx.lineTo(170, 15);
			ctx.stroke();*/
			//drawCenteredRotatedRect(ctx, 400, 300, 50, 50, 45, "blue");
			//drawCenteredRotatedRect(ctx, 25, 25, 50, 50, 0, "red");
		};
		img.src = "terrain_couleur.png";
		
		var chart = new CanvasJS.Chart("chartContainer",{
			zoomEnabled:true,
			
			title :{
				text: "CRIS Data"
			},
			axisX: {						
				title: "Axis X",
				//minimum: 0,
				//maximum: 150
			},
			axisY: {						
				title: "Axis Y"
			},
			data: [{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 1",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 2",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps2
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 3",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps3
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 4",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps4
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 5",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps5
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 6",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps6
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 7",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps7
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 8",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps8
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 9",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps9
				}
			]
		});
		
		var chartPID1 = new CanvasJS.Chart("chartPID1",{
			zoomEnabled:true,
			
			title :{
				text: "Left Speed"
			},
			axisX: {						
				title: "Axis X",
				//minimum: 0,
				//maximum: 150
			},
			axisY: {						
				title: "Axis Y"
			},
			data: [{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 1",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps11
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 2",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps12
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 3",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps13
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 4",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps14
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 5",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps15
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 6",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps16
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 7",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps17
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 8",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps18
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 9",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps19
				}
			]
		});
		var chartPID2 = new CanvasJS.Chart("chartPID2",{
			zoomEnabled:true,
			
			title :{
				text: "Right Speed"
			},
			axisX: {						
				title: "Axis X",
				//minimum: 0,
				//maximum: 150
			},
			axisY: {						
				title: "Axis Y"
			},
			data: [{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 1",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps21
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 2",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps22
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 3",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps23
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 4",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps24
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 5",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps25
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 6",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps26
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 7",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps27
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 8",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps28
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 9",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps29
				}
			]
		});
		var chartPID3 = new CanvasJS.Chart("chartPID3",{
			zoomEnabled:true,
			
			title :{
				text: "Distance"
			},
			axisX: {						
				title: "Axis X",
				//minimum: 0,
				//maximum: 150
			},
			axisY: {						
				title: "Axis Y"
			},
			data: [{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 1",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps31
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 2",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps32
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 3",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps33
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 4",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps34
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 5",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps35
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 6",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps36
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 7",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps37
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 8",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps38
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 9",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps39
				}
			]
		});
		var chartPID4 = new CanvasJS.Chart("chartPID4",{
			zoomEnabled:true,
			
			title :{
				text: "Angle"
			},
			axisX: {						
				title: "Axis X",
				//minimum: 0,
				//maximum: 150
			},
			axisY: {						
				title: "Axis Y"
			},
			data: [{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 1",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps41
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 2",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps42
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 3",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps43
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 4",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps44
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 5",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps45
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 6",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps46
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 7",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps47
				},
				{
					type: "line",
					showInLegend: true,
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 8",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps48
				},
				{
					type: "line",
					showInLegend: true, 
					//markerType: "none",
					markerType: "cross",
					markerSize: 5,
					name: "data 9",
					toolTipContent: "{name} : x={x}, y={y}",
					dataPoints : dps49
				}
			]
		});

		document.addEventListener('keypress', (event) => {
		  if(event.key == 32 || event.key === ' '){
			  sendCmd('stop=1');
			  alert('Emergency stop pressed');
		  }
		//alert('Évènement keypress\n\n' + 'touche : ' + nomTouche);

		  
		});
		/*INIT ONGLET 1*/
		if(document.getElementById("checkbox_canvas").checked){
			var intervalCanvas = Number(document.getElementById("inputText_canvasRrefreshRate").value);
			if(isNaN(intervalCanvas)){
				alert("erreur : " + document.getElementById("inputText_canvasRrefreshRate").value + " n'est pas une valeur valable.");
				return;
			}
			else if(intervalCanvas <= 0){
				alert("Veuillez entrer une valeur positive.");
			}
			else{
				canvasInterval = setInterval(function(){drawCanvas();}, intervalCanvas);
			}
		}
		if(document.getElementById("checkbox_UI").checked){
			var intervalUI = Number(document.getElementById("inputText_UIRefreshRate").value);
			if(isNaN(intervalUI)){
				alert("erreur : " + document.getElementById("inputText_UIRefreshRate").value + " n'est pas une valeur valable.");
				return;
			}
			else if(intervalUI <= 0){
				alert("Veuillez entrer une valeur positive.");
			}
			else{
				UIInterval = setInterval(function(){updateUI();}, intervalUI);
			}
		}
		
		$("#inputText").keyup(function(event){
		    if(event.keyCode == 13){
		        sendData();
		    }
			else if(event.keyCode == 38){	//up arrow
				nCMD++;
				if(nCMD > 9){
					nCMD = 9;
				}
				if(nCMD == 1){
					tCMD[0] = $("#inputText").val();	//save CMD
				}
				$("#inputText").val(tCMD[nCMD]);
		    }
			else if(event.keyCode == 40){	//down arrow
				nCMD--;
				if(nCMD < 0){
					nCMD = 0;
				}
				$("#inputText").val(tCMD[nCMD]);
		    }
		});
		
		$("#Button_ClearChart").click(function(event){
			clearChart();
		});
		$("#ButtonClearChartsPID").click(function(event){
			clearChartPID();
		});
		$("#button_go").click(function(event){
			var xDest = Number(document.getElementById("inputText_destX").value);
			var yDest = Number(document.getElementById("inputText_destY").value);
			if(isNaN(xDest) || isNaN(yDest)){
				alert("Error : invalid destination");
			}
			else{
				sendCmd("x=" + xDest + "&y=" + yDest + "&go");
			}
		});
		$("#button_turn").click(function(event){
			var tDest = Number(document.getElementById("inputText_destT").value);
			if(isNaN(tDest)){
				alert("Error : invalid angle");
			}
			else{
				sendCmd("turn=" + tDest);
			}
		});
		$("#Button_ClearTerminal").click(function(event){
			$("#output").text("");
			$("#output2").text("");
		});
		$("#bt_downloadCharts").click(function(event){
			downloadCharts();
		});
		$("#Button_DownloadLogTerminal").click(function(event){
			downloadLogTerminal();
		});
		$("#ButtonSendPID").click(function(event){
			var cmd = "p1=" + parseInt(document.getElementById("numberP1").value * COEF_SCALE_PID);
			cmd += "&i1=" + parseInt(document.getElementById("numberI1").value * COEF_SCALE_PID);
			cmd += "&d1=" + parseInt(document.getElementById("numberD1").value * COEF_SCALE_PID);
			cmd += "&p2=" + parseInt(document.getElementById("numberP2").value * COEF_SCALE_PID);
			cmd += "&i2=" + parseInt(document.getElementById("numberI2").value * COEF_SCALE_PID);
			cmd += "&d2=" + parseInt(document.getElementById("numberD2").value * COEF_SCALE_PID);
			cmd += "&p3=" + parseInt(document.getElementById("numberP3").value * COEF_SCALE_PID);
			cmd += "&i3=" + parseInt(document.getElementById("numberI3").value * COEF_SCALE_PID);
			cmd += "&d3=" + parseInt(document.getElementById("numberD3").value * COEF_SCALE_PID);
			cmd += "&p4=" + parseInt(document.getElementById("numberP4").value * COEF_SCALE_PID);
			cmd += "&i4=" + parseInt(document.getElementById("numberI4").value * COEF_SCALE_PID);
			cmd += "&d4=" + parseInt(document.getElementById("numberD4").value * COEF_SCALE_PID);
			sendCmd(cmd);
		});
		$("#ButtonLoadPID").click(function(event){
			sendCmd("loadPID=");
		});
		$('#tab-nav a').click(function(e){
			/* On désactive l'action par défaut des liens */
			e.preventDefault();
			
			/* On récupère la valeur de l'onglet à activer */
			var tab = $(this).data('tab');
			
			/* On masque tous les contenus */
			$('.tab').removeClass('tab-active');
			
			/* On affiche le contenu qui doit l'être */
			$('#'+tab).addClass('tab-active');
			
			/* On désactive tous les onglets */
			$('#tab-nav a').removeClass('tab-nav-active');
			
			/* On active l'onglet qui a été cliqué */
			$(this).addClass('tab-nav-active');
			if(tab == "onglet3"){
				$("#output").scrollTop($("#output")[0].scrollHeight);
				$("#output2").scrollTop($("#output2")[0].scrollHeight);
			}
			if(tab == "onglet2"){
				if(document.getElementById("checkbox_chart").checked){
					var interval = Number(document.getElementById("inputText_chartRrefreshRate").value);
					if(isNaN(interval)){
						alert("erreur : " + document.getElementById("inputText_chartRrefreshRate").value + " n'est pas une valeur valable.");
						return;
					}
					else if(interval <= 0){
						alert("Veuillez entrer une valeur positive.");
					}
					else{
						chartInterval = setInterval(function(){chart.render();}, interval);
					}
				}
			}
			else{
				clearInterval(chartInterval);
			}
			if(tab == "onglet1"){
				if(document.getElementById("checkbox_canvas").checked){
					var intervalCanvas = Number(document.getElementById("inputText_canvasRrefreshRate").value);
					if(isNaN(intervalCanvas)){
						alert("erreur : " + document.getElementById("inputText_canvasRrefreshRate").value + " n'est pas une valeur valable.");
						return;
					}
					else if(intervalCanvas <= 0){
						alert("Veuillez entrer une valeur positive.");
					}
					else{
						canvasInterval = setInterval(function(){drawCanvas();}, intervalCanvas);
					}
				}
				if(document.getElementById("checkbox_UI").checked){
					var intervalUI = Number(document.getElementById("inputText_UIRefreshRate").value);
					if(isNaN(intervalUI)){
						alert("erreur : " + document.getElementById("inputText_UIRefreshRate").value + " n'est pas une valeur valable.");
						return;
					}
					else if(intervalUI <= 0){
						alert("Veuillez entrer une valeur positive.");
					}
					else{
						UIInterval = setInterval(function(){updateUI();}, intervalUI);
					}
				}
			}
			else{
				//$("#chartContainer").prop( "disabled", true );
				clearInterval(canvasInterval);
				clearInterval(UIInterval);
			}
			if(tab == "onglet4"){
				chartPID1.render();
				chartPID2.render();
				chartPID3.render();
				chartPID4.render();
				sendCmd("loadPID=");
				if(document.getElementById("checkbox_chart").checked){
					var interval = Number(document.getElementById("inputText_chartRrefreshRate").value);
					if(isNaN(interval)){
						alert("erreur : " + document.getElementById("inputText_chartRrefreshRate").value + " n'est pas une valeur valable.");
						return;
					}
					else if(interval <= 0){
						alert("Veuillez entrer une valeur positive.");
					}
					else{
						chartIntervalPID = setInterval(function(){
							chartPID1.render();
							chartPID2.render();
							chartPID3.render();
							chartPID4.render();
							}, interval);
					}
				}
			}
			else{
				clearInterval(chartIntervalPID);
			}
		});
		$('.outputText').attr('disabled', 'disabled');
		$('#canvasPlayingArea').click(function(evt){
			var offset = $('#canvasPlayingArea').offset();
			// Then refer to 
			var x = evt.pageX - offset.left;
			var y = evt.pageY - offset.top;
			
			var saveX = x;
			x = y * 3000 / $("#canvasPlayingArea").width();
			y = saveX * 2000 / $("#canvasPlayingArea").height();
			//drawRobot(ctx, x, y, 50, 50, 60, "blue", "GR");
			$("#inputText_destX").val(x);
			$("#inputText_destY").val(y);
			sendCmd("x=" + x + "&y=" + y + "&go");
			
			lidarPoints.push({x: x,y: y});
			
		});
		$(".radioMotor").click(function(event){
			var motorId = event.target.name.replace("motor","");
			var val = event.target.value;
			if(val == 3){
				var id = "inputText_" + event.target.name + "Custom";
				val = Number(document.getElementById(id).value);
				if(isNaN(val)){
					alert("erreur : " + document.getElementById(id).value + " n'est pas une valeur valable.");
					return;
				}
				if(val < 0 || val > 100){
					alert("erreur : " + val + " n'est pas une valeur valable.");
					return;
				}
				//alert("motor " + motorId + " " + val);
				sendCmd("m:" + motorId + "=" + val);
			}
			else{
				//alert("motorPreset " + motorId + " " + val);
				sendCmd("mp:" + motorId + "=" + val);
			}
		});
		$(".radioServo").click(function(event){
			var servoId = event.target.name.replace("servo","");
			var val = event.target.value;
			if(val == 3){
				var id = "inputText_" + event.target.name + "Custom";
				val = Number(document.getElementById(id).value);
				if(isNaN(val)){
					alert("erreur : " + document.getElementById(id).value + " n'est pas une valeur valable.");
					return;
				}
				if(val < 0 || val > 20000){
					alert("erreur : " + val + " n'est pas une valeur valable.");
					return;
				}
				//alert("servo " + servoId + " " + val);
				sendCmd("s:" + servoId + "=" + val);
			}
			else{
				//alert("servoPreset " + servoId + " " + val);
				sendCmd("sp:" + servoId + "=" + val);
			}
		});
		$(".inputText_servoCustom").keypress(function(event){
			if(event.keyCode == '13'){
				var id = event.target.id;	//get id of servo
				id = id.replace("inputText_servo","");
				id = id.replace("Custom","");
				var name = "servo" + id;
				var selector = "input[name=" + name + "]:checked";
				var checkVal = document.querySelector(selector).value;	//get which radio button is checked
				if(checkVal == 3){
					var val = Number(event.target.value);
					//alert("var id = " + id + " & checkVal = " + checkVal);
					//alert("servo " + id + " " + val);
					if(isNaN(val)){
						alert("erreur : " + event.target.value + " n'est pas une valeur valable.");
						return;
					}
					if(val < 0 || val > 20000){
						alert("erreur : " + val + " n'est pas une valeur valable.");
						return;
					}
					//alert("servo " + id + " " + val);
					sendCmd("s:" + id + "=" + val);
					//sendCmd("s:" + id + "=" + val + "&s:2=42&s:3=452&m:1=95&mp:2=1");
				}
			}
		});
		$(".inputText_motorCustom").keypress(function(event){
			if(event.keyCode == '13'){
				var id = event.target.id;	//get id of servo
				id = id.replace("inputText_motor","");
				id = id.replace("Custom","");
				var name = "motor" + id;
				var selector = "input[name=" + name + "]:checked";
				var checkVal = document.querySelector(selector).value;	//get which radio button is checked
				if(checkVal == 3){
					var val = Number(event.target.value);
					//alert("var id = " + id + " & checkVal = " + checkVal);
					//alert("servo " + id + " " + val);
					if(isNaN(val)){
						alert("erreur : " + event.target.value + " n'est pas une valeur valable.");
						return;
					}
					if(val < 0 || val > 100){
						alert("erreur : " + val + " n'est pas une valeur valable.");
						return;
					}
					//alert("motor " + id + " " + val);
					sendCmd("m:" + id + "=" + val);
				}
			}
		});
		$("#inputText_requestRefreshRate").keypress(function(event){
			if(event.keyCode == '13'){
				var interval = Number(event.target.value);
				if(isNaN(interval)){
					alert("erreur : " + event.target.value + " n'est pas une valeur valable.");
					return;
				}
				else if(interval <= 0){
					alert("Veuillez entrer une valeur positive.");
				}
				else{
					clearInterval(requestInterval);
					requestInterval = setInterval(function(){xhrRequest();}, interval);
				}
			}
		});
		
		/*$("#sliderP1").onchange(function(event){
			document.getElementById("outputValueP1").value = this.value;
		}*/
		/*var sliderP1 = document.getElementById("sliderP1");
		var sliderI1 = document.getElementById("sliderI1");
		var sliderD1 = document.getElementById("sliderD1");

		sliderP1.addEventListener('input', function() {
			document.getElementById("outputValueP1").value = this.value;
		});
		sliderI1.addEventListener('input', function() {
			document.getElementById("outputValueI1").value = this.value;
		});
		sliderD1.addEventListener('input', function() {
			document.getElementById("outputValueD1").value = this.value;
		});*/
		
		$("#checkbox_request").change(function() {
			if(this.checked) {
				var interval = Number(document.getElementById("inputText_requestRefreshRate").value);
				if(isNaN(interval)){
					alert("erreur : " + document.getElementById("inputText_requestRefreshRate").value + " n'est pas une valeur valable.");
					return;
				}
				else if(interval <= 0){
					alert("Veuillez entrer une valeur positive.");
				}
				else{
					requestInterval = setInterval(function(){xhrRequest();}, interval);
				}
			}
			else{
				clearInterval(requestInterval);
			}
		});
		$("#checkbox_logAll").change(function() {
			if(this.checked) {
				document.getElementById("checkbox_logPosition").checked = true;
				document.getElementById("checkbox_logCurves").checked = true;
				document.getElementById("checkbox_logSensors").checked = true;
				document.getElementById("checkbox_logActuators").checked = true;
			}
			else{
				document.getElementById("checkbox_logPosition").checked = false;
				document.getElementById("checkbox_logCurves").checked = false;
				document.getElementById("checkbox_logSensors").checked = false;
				document.getElementById("checkbox_logActuators").checked = false;
			}
		});
		/*$("#inputText_chartRrefreshRate").keypress(function(event){
			if(event.keyCode == '13'){
				var interval = Number(event.target.value);
				if(isNaN(interval)){
					alert("erreur : " + event.target.value + " n'est pas une valeur valable.");
					return;
				}
				else if(interval <= 0){
					alert("Veuillez entrer une valeur positive.");
				}
				else{
					clearInterval(chartInterval);
					chartInterval = setInterval(function(){chart.render();}, interval);
				}
			}
		});
		
		$("#checkbox_chart").change(function() {
			if(this.checked) {
				var interval = Number(document.getElementById("inputText_chartRrefreshRate").value);
				if(isNaN(interval)){
					alert("erreur : " + document.getElementById("inputText_chartRrefreshRate").value + " n'est pas une valeur valable.");
					return;
				}
				else if(interval <= 0){
					alert("Veuillez entrer une valeur positive.");
				}
				else{
					chartInterval = setInterval(function(){chart.render();}, interval);
				}
			}
			else{
				clearInterval(chartInterval);
			}
		});*/

		$("#ButtonSendOdo1").click(function(event){
			var cmd = "odo1=" + parseInt(document.getElementById("numberOdo1").value * COEF_SCALE_COEF_DISSYMETRY);
			sendCmd(cmd);
		});
		$("#ButtonSendOdo2").click(function(event){
			var cmd = "odo2=" + parseInt(document.getElementById("numberOdo2").value * COEF_SCALE_MM_PER_TICKS);
			sendCmd(cmd);
		});
		$("#ButtonSendOdo3").click(function(event){
			var cmd = "odo3=" + parseInt(document.getElementById("numberOdo3").value * COEF_SCALE_DISTANCE_BETWEEN_ENCODER_WHEELS);
			sendCmd(cmd);
		});
		
		function sendData() {
			var data = $("#inputText").val();
			document.getElementById("output").value += ">" + data + "\n";
			if (data != "" && data != tCMD[1]){
				for (i = 10; i > 1; i--) {
					tCMD[i] = tCMD[i-1];
				}
				tCMD[1] = data;
			}
			
			var xhr = new XMLHttpRequest();
			xhr.open("POST", "socket.php", true);

			//Send the proper header information along with the request
			xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

			xhr.onreadystatechange = function() {//Call a function when the state changes.
				if(this.readyState == XMLHttpRequest.DONE && this.status == 200) {
					var r = this.responseText;
					document.getElementById("output").value += r + "\n";
					/*var d = $("#output2").text() + "received hello CMD\n"
						$("#output2").text(d);*/
				}
			}
			xhr.send(data);
			
			$("#inputText").val("");
			nCMD = 0;
		}
		function clearChart() {
			dps.length = 0;
			dps2.length = 0;
			dps3.length = 0;
			dps4.length = 0;
			dps5.length = 0;
			dps6.length = 0;
			dps7.length = 0;
			dps8.length = 0;
			dps9.length = 0;
		}
		function clearChartPID() {
			dps11.length = 0;
			dps12.length = 0;
			dps13.length = 0;
			dps14.length = 0;
			dps15.length = 0;
			dps16.length = 0;
			dps17.length = 0;
			dps18.length = 0;
			dps19.length = 0;
			
			dps21.length = 0;
			dps22.length = 0;
			dps23.length = 0;
			dps24.length = 0;
			dps25.length = 0;
			dps26.length = 0;
			dps27.length = 0;
			dps28.length = 0;
			dps29.length = 0;
			
			dps31.length = 0;
			dps32.length = 0;
			dps33.length = 0;
			dps34.length = 0;
			dps35.length = 0;
			dps36.length = 0;
			dps37.length = 0;
			dps38.length = 0;
			dps39.length = 0;
			
			dps41.length = 0;
			dps42.length = 0;
			dps43.length = 0;
			dps44.length = 0;
			dps45.length = 0;
			dps46.length = 0;
			dps47.length = 0;
			dps48.length = 0;
			dps49.length = 0;
		}
		function download(data, filename, type) {
			var file = new Blob([data], {type: type});
			if (window.navigator.msSaveOrOpenBlob) // IE10+
				window.navigator.msSaveOrOpenBlob(file, filename);
			else { // Others
				var a = document.createElement("a"),
						url = URL.createObjectURL(file);
				a.href = url;
				a.download = filename;
				document.body.appendChild(a);
				a.click();
				setTimeout(function() {
					document.body.removeChild(a);
					window.URL.revokeObjectURL(url);  
				}, 0); 
			}
		}
		function downloadCharts(){
			var strData ="";
			var strLineX = "x1";
			var strLineY = "y1";
			for (var n in dps){
				var val = Object.values(dps[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData = strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x2";
			var strLineY = "y2";
			for (var n in dps2){
				var val = Object.values(dps2[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x3";
			var strLineY = "y3";
			for (var n in dps3){
				var val = Object.values(dps3[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x4";
			var strLineY = "y4";
			for (var n in dps4){
				var val = Object.values(dps4[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x5";
			var strLineY = "y5";
			for (var n in dps5){
				var val = Object.values(dps5[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x6";
			var strLineY = "y6";
			for (var n in dps6){
				var val = Object.values(dps6[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x7";
			var strLineY = "y7";
			for (var n in dps7){
				var val = Object.values(dps7[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x8";
			var strLineY = "y8";
			for (var n in dps8){
				var val = Object.values(dps8[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			var strLineX = "x9";
			var strLineY = "y9";
			for (var n in dps9){
				var val = Object.values(dps9[n]);
				strLineX += ";" + val[0];
				strLineY += ";" + val[1];
			}
			strData += strLineX + "\r\n" + strLineY + "\r\n";
			
			
			strData = strData.replace(/\./g, ",");
			download(strData,"data1.csv","csv")
		}
		function downloadLogTerminal(){
			var strData ="Terminal 1 :\n" + $("#output").text() + "\nTerminal 2 :\n" + $("#output2").text();
			download(strData,"log.txt","txt")
		}
		
		
		function drawCenteredRotatedRect(ctx, x, y, width, height, degrees, style) {

			// first save the untranslated/unrotated context
			ctx.save();

			ctx.beginPath();
			// move the rotation point to the center of the rect
			//ctx.translate(x + width / 2, y + height / 2);
			ctx.translate(x, y);
			// rotate the rect
			ctx.rotate(degrees * Math.PI / 180);

			// draw the rect on the transformed context
			// Note: after transforming [0,0] is visually [x,y]
			//       so the rect needs to be offset accordingly when drawn
			ctx.rect(-width / 2, -height / 2, width, height);

			ctx.fillStyle = style;
			ctx.fill();
			ctx.clearRect(-width / 2 + 5,-height / 2 + 5,width - 10,height - 10);
			ctx.fillText("GR",-width / 2 + 20,-height / 2 + 25);

			// restore the context to its untranslated/unrotated state
			ctx.restore();

		}
		function drawPoint(ctx, x, y, style) {
			var width = 5;
			var height = 5;
			var degrees = 45;
			x = x / 3000 * $("#canvasPlayingArea").width();
			y = y / 2000 * $("#canvasPlayingArea").height();
			var save = x;
			x = y;
			y = save;
			// first save the untranslated/unrotated context
			ctx.save();

			ctx.beginPath();
			// move the rotation point to the center of the rect
			//ctx.translate(x + width / 2, y + height / 2);
			ctx.translate(x, y);
			// rotate the rect
			ctx.rotate(-degrees * Math.PI / 180);

			// draw the rect on the transformed context
			// Note: after transforming [0,0] is visually [x,y]
			//       so the rect needs to be offset accordingly when drawn
			ctx.rect(-width / 2, -height / 2, width, height);

			ctx.fillStyle = style;
			ctx.fill();
			// restore the context to its untranslated/unrotated state
			ctx.restore();


		}
		function drawRobot(ctx, x, y, width, height, degrees, style, text) {

			x = x / 3000 * $("#canvasPlayingArea").width();
			y = y / 2000 * $("#canvasPlayingArea").height();
			var save = x;
			x = y;
			y = save;
			// first save the untranslated/unrotated context
			ctx.save();

			ctx.beginPath();
			// move the rotation point to the center of the rect
			//ctx.translate(x + width / 2, y + height / 2);
			ctx.translate(x, y);
			// rotate the rect
			ctx.rotate(-degrees * Math.PI / 180);

			// draw the rect on the transformed context
			// Note: after transforming [0,0] is visually [x,y]
			//       so the rect needs to be offset accordingly when drawn
			ctx.rect(-width / 2, -height / 2, width, height);

			ctx.fillStyle = style;
			ctx.fill();
			//ctx.clearRect(-width / 2 + 5,-height / 2 + 5,width - 10,height - 10);
			ctx.fillStyle = 'orange';
			ctx.fillRect(-width / 2 + 5,-height / 2 + 5,width - 10,height - 10);
			ctx.fillStyle = 'black';
			//ctx.fillStyle = 'style';
			//ctx.fill();

			ctx.fillText(text,-width / 2 + 20,-height / 2 + 25);
			/*ctx.beginPath();
			ctx.moveTo(width/2,height/2);
			//ctx.arc(0,0,100,Math.PI/4,Math.PI*3/4);
			var dist = document.getElementById("outputText_US2").value;
			ctx.arc(0,0,dist,Math.PI/4,Math.PI*3/4);
			ctx.lineTo(-width/2,height/2);
			ctx.closePath();
			//ctx.fillStyle = "orange";
			if(dist < 90)
				ctx.fillStyle = "rgba(255, 165, 0, 0.5)";
			else
				ctx.fillStyle = "rgba(0, 255, 0, 0.5)";
			ctx.fill();
			ctx.stroke();
			ctx.beginPath();
			ctx.moveTo(-width/2,-height/2);
			var dist = document.getElementById("outputText_US5").value;
			ctx.arc(0,0,dist,Math.PI/4 + Math.PI,Math.PI*3/4 + Math.PI);
			ctx.lineTo(width/2,-height/2);
			ctx.closePath();
			//ctx.fillStyle = "green";
			if(dist < 90)
				ctx.fillStyle = "rgba(255, 165, 0, 0.5)";
			else
				ctx.fillStyle = "rgba(0, 255, 0, 0.5)";
			ctx.fill();
			ctx.stroke();*/
			// restore the context to its untranslated/unrotated state
			ctx.restore();

		}
	
		function xhrRequest(){
		
			var xhr = new XMLHttpRequest();
			xhr.open("POST", "socket.php", true);

			//Send the proper header information along with the request
			xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

			xhr.onreadystatechange = function() {//Call a function when the state changes.
				if(this.readyState == XMLHttpRequest.DONE && this.status == 200) {
					//document.getElementById("txtTime").innerHTML = this.responseText;
					//var offset = $('#canvasPlayingArea').offset();
					// Then refer to 
					//var x = evt.pageX - offset.left;
					//var y = evt.pageY - offset.top;
					
					var r = this.responseText;
					//alert(r);
					var subString = r.split("&");
					
					var x;
					var y;
					var t;
					var x2;
					var y2;
					var t2;
					var updateChart = 0;
					for (var i = 0; i < subString.length; i++) {
						if(subString[i].charAt(0) == 'x'){
							subString[i].slice(2);
							x = Number(subString[i].slice(2));
							xRobot1 = x;
							updateChart = 1;
						}
						else if(subString[i].charAt(0) == 'y'){
							y = Number(subString[i].slice(2));
							yRobot1 = y;
							updateChart = 1;
						}
						else if(subString[i].charAt(0) == 't'){	//theta
							t = Number(subString[i].slice(2));
							tRobot1 = t;
							updateChart = 1;
						}
						else if(subString[i].charAt(0) == 'b'){ //battery
							vbat = Number(subString[i].slice(2));
							vbat = Math.round(vbat*100)/100;
							document.getElementById('outputText_BatteryVoltage').value = vbat;
							document.getElementById('outputText_BatteryPercentage').value = voltageToPercentage(vbat);
						}
						
						else if(subString[i].charAt(0) == 'c'){	//curve
							
							var ssTest1 = subString[i].split("=");
						ssTest1[0] = ssTest1[0].slice(1);
							//var id = Number(subString[i].charAt(1));
							var id = Number(ssTest1[0]);
							//subString[i] = subString[i].slice(3);//remove cX=
							
							var subSubString = ssTest1[1].split(";");
							var xVal = Number(subSubString[0]);
							var yVal = Number(subSubString[1]);
							switch(id){
								case 1:
									dps.push({x: xVal,y: yVal});
									break;
								case 2:
									dps2.push({x: xVal,y: yVal});
									break;
								case 3:
									dps3.push({x: xVal,y: yVal});
									break;
								case 4:
									dps4.push({x: xVal,y: yVal});
									break;
								case 5:
									dps5.push({x: xVal,y: yVal});
									break;
								case 6:
									dps6.push({x: xVal,y: yVal});
									break;
								case 7:
									dps7.push({x: xVal,y: yVal});
									break;
								case 8:
									dps8.push({x: xVal,y: yVal});
									break;
								case 9:
									dps9.push({x: xVal,y: yVal});
									break;
								case 11:
									dps11.push({x: xVal,y: yVal});
									break;
								case 12:
									dps12.push({x: xVal,y: yVal});
									break;
								case 13:
									dps13.push({x: xVal,y: yVal});
									break;
								case 14:
									dps14.push({x: xVal,y: yVal});
									break;
								case 15:
									dps15.push({x: xVal,y: yVal});
									break;
								case 16:
									dps16.push({x: xVal,y: yVal});
									break;
								case 17:
									dps17.push({x: xVal,y: yVal});
									break;
								case 18:
									dps18.push({x: xVal,y: yVal});
									break;
								case 19:
									dps19.push({x: xVal,y: yVal});
									break;
								case 21:
									dps21.push({x: xVal,y: yVal});
									break;
								case 22:
									dps22.push({x: xVal,y: yVal});
									break;
								case 23:
									dps23.push({x: xVal,y: yVal});
									break;
								case 24:
									dps24.push({x: xVal,y: yVal});
									break;
								case 25:
									dps25.push({x: xVal,y: yVal});
									break;
								case 26:
									dps26.push({x: xVal,y: yVal});
									break;
								case 27:
									dps27.push({x: xVal,y: yVal});
									break;
								case 28:
									dps28.push({x: xVal,y: yVal});
									break;
								case 29:
									dps29.push({x: xVal,y: yVal});
									break;
								case 31:
									dps31.push({x: xVal,y: yVal});
									break;
								case 32:
									dps32.push({x: xVal,y: yVal});
									break;
								case 33:
									dps33.push({x: xVal,y: yVal});
									break;
								case 34:
									dps34.push({x: xVal,y: yVal});
									break;
								case 35:
									dps35.push({x: xVal,y: yVal});
									break;
								case 36:
									dps36.push({x: xVal,y: yVal});
									break;
								case 37:
									dps37.push({x: xVal,y: yVal});
									break;
								case 38:
									dps38.push({x: xVal,y: yVal});
									break;
								case 39:
									dps39.push({x: xVal,y: yVal});
									break;
								case 41:
									dps41.push({x: xVal,y: yVal});
									break;
								case 42:
									dps42.push({x: xVal,y: yVal});
									break;
								case 43:
									dps43.push({x: xVal,y: yVal});
									break;
								case 44:
									dps44.push({x: xVal,y: yVal});
									break;
								case 45:
									dps45.push({x: xVal,y: yVal});
									break;
								case 46:
									dps46.push({x: xVal,y: yVal});
									break;
								case 47:
									dps47.push({x: xVal,y: yVal});
									break;
								case 48:
									dps48.push({x: xVal,y: yVal});
									break;
								case 49:
									dps49.push({x: xVal,y: yVal});
									break;
							}
						}
						else if(subString[i].charAt(0) == 'd'){	//distance
							updateChart = 1;
							var id = Number(subString[i].charAt(1));
							subString[i] = subString[i].slice(3);//remove dX=
							var val = Number(subString[i]);
							var jsId = "outputText_US" + id;
							document.getElementById(jsId).value = val;
						}//element.setAttribute(name, value);
						else if(subString[i].charAt(0) == 'r'){	//rupt
							subString[i] = subString[i].slice(1);
							var subSubString = subString[i].split("=");
							var id = Number(subSubString[0]);
							//subString[i] = subString[i].slice(3);//remove dX=
							var val = Number(subSubString[1]);
							if(val == 1){
								val = "led-green";
							}
							else{
								
								val = "led-red";
							}
							var jsId = "led_rupt" + id;
							document.getElementById(jsId).className = val;
						}
						else if(subString[i].charAt(0) == 'e'){	//erase
							clearChart();
						}
						else if(subString[i].charAt(0) == 'l'){	//log
							subString[i] = subString[i].slice(2); // remove l=
							document.getElementById("output").value += subString[i];//"LOG : " + subString[i];
						}
						else if(subString[i].charAt(0) == 'p'){	//P - PID
							var nb = Number(subString[i].charAt(1));
							subString[i] = subString[i].slice(3);	//remove px=
							switch(nb){
								case 1:
									document.getElementById("numberP1").value = Number(subString[i]);
									break;
								case 2:
									document.getElementById("numberP2").value = Number(subString[i]);
									break;
								case 3:
									document.getElementById("numberP3").value = Number(subString[i]);
									break;
								case 4:
									document.getElementById("numberP4").value = Number(subString[i]);
									break;
							}
						}
						else if(subString[i].charAt(0) == 'i'){	//I - PID
							var nb = Number(subString[i].charAt(1));
							subString[i] = subString[i].slice(3);	//remove px=
							switch(nb){
								case 1:
									document.getElementById("numberI1").value = Number(subString[i]);
									break;
								case 2:
									document.getElementById("numberI2").value = Number(subString[i]);
									break;
								case 3:
									document.getElementById("numberI3").value = Number(subString[i]);
									break;
								case 4:
									document.getElementById("numberI4").value = Number(subString[i]);
									break;
							}
						}
						else if(subString[i].charAt(0) == 'a'){	//D - PID
							var nb = Number(subString[i].charAt(1));
							subString[i] = subString[i].slice(3);	//remove px=
							switch(nb){
								case 1:
									document.getElementById("numberD1").value = Number(subString[i]);
									break;
								case 2:
									document.getElementById("numberD2").value = Number(subString[i]);
									break;
								case 3:
									document.getElementById("numberD3").value = Number(subString[i]);
									break;
								case 4:
									document.getElementById("numberD4").value = Number(subString[i]);
									break;
							}
						}
						else if(subString[i].charAt(0) == 's'){	//scan lidar
							subString[i]= subString[i].slice(2);	//remove s=
							
							var subSubString = subString[i].split(";");
							var xVal = Number(subSubString[0]);
							var yVal = Number(subSubString[1]);
							lidarPoints.push({x: xVal,y: yVal});
							//alert('push x : ' + xVal + ' & y : ' + yVal + '\n' );
						}
						else if(subString[i].charAt(0) == 'z'){	//scan lidar
							subString[i]= subString[i].slice(2);	//remove z=
							
							var subSubString = subString[i].split(";");
							var xVal = Number(subSubString[0]);
							var yVal = Number(subSubString[1]);
							lidarPointsCircular[iLidar] = {x: xVal,y: yVal};
							iLidar++;
							if(iLidar == SIZE_LIDAR_POINTS_CIRCULAR){
								iLidar = 0;
							}
							//alert('push x : ' + xVal + ' & y : ' + yVal + '\n' );
						}
						else if(subString[i].charAt(0) == 'w'){	//erase lidar points
							lidarPoints.length = 0;
							lidarPointsCircular.length = 0;
							iLidar = 0;
						}
					}
					/*if(updateChart){	//attention ne change pas si les capteurs bougent mais pas le robot
						if ( document.getElementById("onglet1").className.match(/(?:^|\s)tab-active(?!\S)/) ){	//attention à réactualiser quand on reclic sur l'onglet "Control"
							ctx.drawImage(img, 0, 0);
						
							x2 = 400 - (x * 100);
							y2 = 200 - (y * 100);
							t2 = -45;
							
							x = (x * 100) + 300;
							y = (y * 100) + 300;
							t = t * 360 / 2*3.14159;
							drawRobot(ctx, x, y, 50, 50, t, "blue", "GR");
							drawRobot(ctx, x2, y2, 50, 50, t2, "blue", "PR");
							document.getElementById("outputText_posX").value = x;
							document.getElementById("outputText_posY").value = y;
							document.getElementById("outputText_posT").value = t;
						}
					}*/
				}
			}
			xhr.send("update");
		}
		function sendCmd(cmd){
		
			var xhr = new XMLHttpRequest();
			xhr.open("POST", "socket.php", true);

			//Send the proper header information along with the request
			xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

			xhr.onreadystatechange = function() {//Call a function when the state changes.
				if(this.readyState == XMLHttpRequest.DONE && this.status == 200) {
					
					var r = this.responseText;
					//if(r != "OK"){
					/*if( r != null){
						if(!r.startsWith("OK")){
							alert("Error occured during SendCmd function : server did not respond \"OK\".\nResponse : " + r);
						}
					}*/
				}
			}
			//xhr.send("cmd=" + cmd);
			xhr.send(cmd);
		}
		function drawCanvas(){
			ctx.drawImage(img, 0, 0);
			drawRobot(ctx, xRobot1, yRobot1, 50, 50, tRobot1 + 180, "black", "GR");
			for(var i = 0; i < lidarPoints.length;i++){
				drawPoint(ctx, lidarPoints[i].x, lidarPoints[i].y, "red");
			}
			for(var i = 0; i < lidarPointsCircular.length;i++){
				drawPoint(ctx, lidarPointsCircular[i].x, lidarPointsCircular[i].y, "red");
			}
			//drawRobot(ctx, xRobot2, yRobot2, 50, 50, tRobot2, "red", "PR");
		}
		function updateUI(){
			document.getElementById("outputText_posX").value = xRobot1;
			document.getElementById("outputText_posY").value = yRobot1;
			document.getElementById("outputText_posT").value = tRobot1;
		}
		function voltageToPercentage(voltage){
			var percentage = 0;
			if(voltage < 12){
				percentage = 0;
			}
			else if(voltage <= 13.2){
				percentage = (voltage - 12) / 1.2 * 5;
			}
			else if(voltage <= 14.4){
				percentage = 5 + (voltage - 13.2) / 1.2 * 5;					
			}
			else if(voltage <= 14.8){
				percentage = 10 + (voltage - 14.4) / 0.4 * 10;					
			}
			else if(voltage <= 15){
				percentage = 20 + (voltage - 14.8) / 0.2 * 10;					
			}
			else if(voltage <= 15.16){
				percentage = 30 + (voltage - 15) / 0.16 * 10;					
			}
			else if(voltage <= 15.32){
				percentage = 40 + (voltage - 15.16) / 0.16 * 10;					
			}
			else if(voltage <= 15.48){
				percentage = 50 + (voltage - 15.32) / 0.16 * 10;					
			}
			else if(voltage <= 15.68){
				percentage = 60 + (voltage - 15.48) / 0.2 * 10;					
			}
			else if(voltage <= 15.88){
				percentage = 70 + (voltage - 15.68) / 0.2 * 10;					
			}
			else if(voltage <= 16.4){
				percentage = 80 + (voltage - 15.88) / 0.62 * 10;					
			}
			else if(voltage <= 16.8){
				percentage = 90 + (voltage - 16.4) / 0.4 * 10;					
			}
			else{
				percentage = 100;
			}
			
			percentage = Math.round(percentage * 100)/100;
			return percentage;
		}
}
