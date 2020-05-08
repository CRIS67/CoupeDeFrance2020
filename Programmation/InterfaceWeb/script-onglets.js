window.onload = function () {
		
		jQuery('#tab-nav a').click(function(e){
		/* On désactive l'action par défaut des liens */
		e.preventDefault();
		
		/* On récupère la valeur de l'onglet à activer */
		var tab = jQuery(this).data('tab');
		
		/* On masque tous les contenus */
		jQuery('.tab').removeClass('tab-active');
		
		/* On affiche le contenu qui doit l'être */
		jQuery('#'+tab).addClass('tab-active');
		
		/* On désactive tous les onglets */
		jQuery('#tab-nav a').removeClass('tab-nav-active');
		
		/* On active l'onglet qui a été cliqué */
		jQuery(this).addClass('tab-nav-active');
		if(tab == "onglet3"){
			$("#output").scrollTop($("#output")[0].scrollHeight);
			$("#output2").scrollTop($("#output2")[0].scrollHeight);
		}
     });
	
      var dps = [];   //dataPoints. 
	  var dps2 = [];

      var chart = new CanvasJS.Chart("chartContainer",{
		zoomEnabled:true,
      	title :{
      		text: "CRIS Data"
      	},
      	axisX: {						
      		title: "Axis X",
			//minimum: 0,
			maximum: 150
      	},
      	axisY: {						
      		title: "Axis Y"
      	},
      	data: [{
      		type: "line",
			//markerType: "none",
      		markerType: "cross",
			markerSize: 5,
			dataPoints : dps
      	},
		{
      		type: "line",
			//markerType: "none",
      		markerType: "cross",
			markerSize: 5,
			dataPoints : dps2
      	}
		]
      });

      chart.render();
      var xVal = dps.length + 1;
      var yVal = 15;

      var updateChart = function () {
			chart.render();	
		};

setInterval(function(){chart.render();}, 50); 
var buffer = "";
	var tCMD =  ["","","","","","","","","",""];
	var nCMD = 0;
	var dataChart = [];
	webiopi().ready(function() {
		$("#inputText").keyup(function(event){
		    if(event.keyCode == 13){
		        sendDataRet();
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
		$("#inputText2").keyup(function(event){
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
		$.get("/devices/*", function(data) {
			var devices = $("#devices");
			var added = false;
			for (i in data) {
				if (data[i].type=="Serial") {
					added = true;
					devices.append($("<option>" + data[i].name + "</option>"))
				}
			}
			if (added) {
				readData2();
			}
		});
		$("#testButton1").click(function(event){
		    webiopi().Serial($("#devices").val()).write("\n");
		});
		$("#testButton2").click(function(event){
			clearChart();
			webiopi().Serial($("#devices").val()).write("#10 0\n#20 1\n#30 -1\n#40 3\n#50 -3\n#60 8\n#70 -8\n#80 0\n");
		});
		$("#testButton3").click(function(event){
			clearChart();
			/*var x = [];
			var y = [];*/
			data = "";
			for (var i = 0; i <= 150; i++) {
				data += "#";
				data += i;
				data += " ";
				var b = Math.sin(i/20);
				data += b
				data += "\n";
				var a = 1;
				if(b < 0){
					a = -1;
				}
				dps2.push({x: i,y: a});
			}
			webiopi().Serial($("#devices").val()).write(data);
		});
	});
	
	function readData() {
		webiopi().Serial($("#devices").val()).read(function(data) {
			if (data.length > 0) {
				buffer = buffer + data;
				var d = $("#output").text() + data;
				$("#output").text(d);
				var $textarea = $("#output");
				$textarea.scrollTop($textarea[0].scrollHeight);
				if(buffer[buffer.length - 1] == '\n'){
					if (buffer == "hello\n"){
						var d = $("#output").text() + "received hello CMD\n"
						$("#output").text(d);
					}
					else if(buffer[0] == '#'){
						buffer = buffer.slice(1);	//remove #
						var subString = buffer.split(" ");
						var xVal = Number(subString[0]);
						var yVal = Number(subString[1]);
						dps.push({x: xVal,y: yVal});
						$("#output").text($("#output").text() + "Chart CMD x = " + xVal + " & y = " + yVal + "\n");
						//chart.render();			
					}
					else{
					$("#output").text($("#output").text() + "Invalid CMD\n");
						
					/*dps.push({x: dps.length + 1,y: 15});
					chart.render();*/
					//updateChart();
					
					
					}
					buffer = "";
				}
			}
		});
		setTimeout(readData, 100);
	}
	
	function readData2() {
		webiopi().Serial($("#devices").val()).read(function(data) {
		
			var length = data.length;
			if(length > 0){
				var d = $("#output").text() + data;
				$("#output").text(d);
				var $textarea = $("#output");
				$textarea.scrollTop($textarea[0].scrollHeight);
			}
			for (var i = 0; i < length; i++) {
			  if(data[i] == '\n'){
				if (buffer == "hello"){
					var d = $("#output2").text() + "received hello CMD\n"
					$("#output2").text(d);
				}
				else if(buffer[0] == '#'){
					buffer = buffer.slice(1);	//remove #
					var subString = buffer.split(" ");
					var xVal = Number(subString[0]);
					var yVal = Number(subString[1]);
					dps.push({x: xVal,y: yVal});
					$("#output2").text($("#output2").text() + "Chart CMD x = " + xVal + " & y = " + yVal + "\n");
					//chart.render();			
				}
				else{
					$("#output2").text($("#output2").text() + "Invalid CMD\n");
				}
				var $textarea = $("#output2");
				$textarea.scrollTop($textarea[0].scrollHeight);
				buffer = "";
			  }
			  else{
				buffer = buffer + data[i];
			  }
			}
		});
		setTimeout(readData2, 100);
	}
	
	function sendData() {
		var data = $("#inputText2").val();// + "\n";
		if (data != ""){
			for (i = 10; i > 1; i--) {
				tCMD[i] = tCMD[i-1];
			}
			tCMD[1] = data;
		}
		webiopi().Serial($("#devices").val()).write(data);
		$("#inputText2").val("");
		nCMD = 0;
	}
	function sendDataRet() {
		var data = $("#inputText").val() + "\n";
		if (data != ""){
			for (i = 10; i > 1; i--) {
				tCMD[i] = tCMD[i-1];
			}
			tCMD[1] = data;
		}
		webiopi().Serial($("#devices").val()).write(data);
		$("#inputText").val("");
		nCMD = 0;
	}
	
	function deviceChanged() {
		$("#output").text("");
	}
	function clearChart() {
		dps.length = 0;// = [];
		dps2.length = 0;
		//chart.render();
	}
	
	
}