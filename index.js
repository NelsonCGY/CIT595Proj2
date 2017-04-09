Pebble.addEventListener("appmessage", 
    function(e) {
      if(e.payload){
        if(e.payload.temperature){
          sendToServer("temperature"); 
        }else if(e.payload.switch){
          sendToServer("switch");
        }else{
          Pebble.sendAppMessage({"0": "nokey"});
        }
      }else{
        Pebble.sendAppMessage({"0": "nopayload"});
      }  
    }
);
function sendToServer(param) {
    var req = new XMLHttpRequest();
    var ipAddress = "192.168.1.12"; // Hard coded IP address
    var port = "3001"; // Same port specified as argument to server 
    var url = "http://" + ipAddress + ":" + port + "/";
    var method = "POST";
    var async = true;
    req.onload = function(e) {
        // see what came back
        var msg = "no response";
        var response = JSON.parse(req.responseText); 
        if (response) {
          if (response.real) {
            msg = "N: " + response.real + "\n";
            if(response.average){
              msg += "A: " + response.average + "\n";
            }
            if(response.max){
              msg += "M: " + response.max + "\n";
            }
            if(response.min){
              msg += "M: " + response.min + "\n";
            }
            if(response.degree){
              msg += "D: " + response.degree + "\n";
            }
            
          }else msg = "notemp"; 
        }
        // sends message back to pebble 
        Pebble.sendAppMessage({ "0": msg });
    };
    req.open(method, url, async);
    if(param == "temperature"){
      req.send("temperature");
    }else req.send("switch");
}
