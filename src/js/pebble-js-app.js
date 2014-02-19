function sendDirection(direction) {
	var url = "http://datarelayer.appspot.com/set/accel/";
	var req = new XMLHttpRequest();
	req.open('GET', url + direction, true);
	console.log("Sending direction " + direction);
	req.onload = function(response) {
		if (this.readyState == 4 && this.status == 200) {
			//console.log("Got response: " + this.responseText);
		} 
		else {
			//console.log("OnLoad! " + JSON.stringify(event));
		}
	};
	req.send();
}

Pebble.addEventListener('appmessage', function(e) {
	var angle = Number(e.payload['1']);

	if (angle <= 15 && angle >= -15){
		sendDirection('s');
	}
	else if (angle < -15){
		sendDirection('l');
	}		
	else if (angle > 15){
		sendDirection('r');
	}		
});
