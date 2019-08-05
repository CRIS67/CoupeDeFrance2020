<?php
	//error_reporting(E_ALL);
	error_reporting(0);
	/* Lit le port du service WWW. */
	$service_port = 8888;//getservbyname('www', 'tcp');

	/* Lit l'adresse IP du serveur de destination */
	$address = "localhost";//gethostbyname('www.example.com');

	/* Crée un socket TCP/IP. */
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket === false) {
		echo "socket_create() a échoué : raison :  " . socket_strerror(socket_last_error()) . "<br/>";
	}
	else{
		socket_set_nonblock($socket);
		//socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, array('sec' => 0, 'usec' => 1000));
		//socket_set_option($socket, SOL_SOCKET, SO_SNDTIMEO, array('sec' => 0, 'usec' => 1000));
		//echo "Essai de connexion à '$address' sur le port '$service_port'...";
		//$result = socket_connect($socket, $address, $service_port);
		$connected = false;
		$connected = socket_connect($socket, $address, $service_port);
		if (!$connected)
		{
			$error = socket_last_error();
			if ($error != 10035 && $error != SOCKET_EINPROGRESS && $error != SOCKET_EALREADY) {
				echo "Error Connecting Socket: ".socket_strerror($error) . "\n";
				socket_close($socket);
				exit();
			}
		}
		$writables = array();
		$writables[] = $socket;
		$readable = array();
		$e = array();
		$result = socket_select($readable, $writables, $e, 0,5000);
		if (!$result)
			die("Unable to connect to socket: Timeout");
		/*if ($result === false) {
			echo "socket_connect() a échoué : raison : ($result) " . socket_strerror(socket_last_error($socket)) . "<br/>";
		}*/
		else{
			if(!socket_set_block($socket))
				die("Unable to block the socket");
						/*$in = "HEAD / HTTP/1.0<br/><br/>";
			$in .= "Host: www.example.com<br/>";
			$in .= "Connection: Close<br/><br/>";*/
			
			$in = "";
			$i = 0;
			foreach($_POST as $key => $value) {
				$i++;
				if($i == 1){
					$in .= "$key=$value";
				}
				else{
					$in .= "&$key=$value";
				} 
				//echo "POST parameter '$key' has '$value'";
			}
			//echo $in;
			$lenght = strlen($in);
			//echo $lenght;
			//$lenght = 357;
			$L = $lenght & 0xFF;
			$H = $lenght >> 8 ;
			//echo "H = $H & L = $L ";
			$L = chr($L);
			$H = chr($H);
			//echo "H = $H & L = $L ";
			$bufIn = "$H$L$in";
			//echo "bufIn = $bufIn ";
			$out = '';

			//echo "Envoi de la requête HTTP HEAD...";
			socket_write($socket, $bufIn, strlen($bufIn));
			//echo "OK.<br/>";

			//echo "Lire la réponse : <br/><br/>";
			$buf = 'Ceci est mon buffer.';
			$ret ="";
			//flush();
			//ob_flush();
			
			//while(1){
				$i = 0;
				$size = 4;
				$receiveSize = 0;
				while ($i < $size + 2) {
					if(false !== ($bytes = socket_recv($socket, $buf, 1, MSG_WAITALL))){
						/*receive size*/
						$i++;
						if($i == 1){
							$receiveSize = ord($buf) << 8;
						}
						else if($i == 2){
							$receiveSize += ord($buf);
							$size = $receiveSize;
							//$i = 0;
							//echo $size;
							//return;
						}
						else{
							$ret .= $buf;
						}
						/*if($buf == "\n")
							echo "<br/>";
						else
							echo "$buf";*/
						
						/*if($buf == "\n")
							break;*/
					}
					else {
						//echo $ret;
						echo "socket_recv() a échoué; raison: " . socket_strerror(socket_last_error($socket)) . "\n";
						return;
						//break;
					}
				} 
				echo $ret;
				//
				//flush();
				//ob_flush();
			//}
			/*while ($out = socket_recv($socket, 30)) {//socket_read($socket, 30)) {
				echo $out;
			}*/

			//echo "Fermeture du socket...";
			socket_close($socket);
			//echo "OK.<br/><br/>";
		}
	}
?>