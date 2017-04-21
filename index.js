Pebble.addEventListener("appmessage", 
    function(e) {
      if(e.payload){
        if(e.payload.temperature){
          sendToServer("temperature"); 
        }else if(e.payload.switch){
          sendToServer("switch");
        }else if(e.payload.hour){
          sendToServer("time");
        }else if(e.payload.standby){
          sendToServer("standby");
        }
        else{
          Pebble.sendAppMessage({"0": "nokey"});
        }
      }else{
        Pebble.sendAppMessage({"0": "nopayload"});
      }  
    }
);
function sendToServer(param) {
    var req = new XMLHttpRequest();
    var ipAddress = "158.130.169.90"; // Hard coded IP address
    var port = "3001"; // Same port specified as argument to server 
    var url = "http://" + ipAddress + ":" + port + "/";
    var method = "POST";
    var async = true;
    req.timeout = 3500;
    req.onload = function(e) {
        // see what came back
        var msg = "no response";
        var response = JSON.parse(req.responseText); 
        if (response) {
          if (response.real) {
            msg = "Now: " + response.real + "\n";
            if(response.average){
              msg += "Ave: " + response.average + "\n";
            }
            if(response.max){
              msg += "Max: " + response.max + "\n";
            }
            if(response.min){
              msg += "Min: " + response.min + "\n";
            }
            if(response.degree){
              msg += "Unit: " + response.degree + "\n";
            }
          }else if(response.time){
            msg = " Current time:\n" + response.time + "\n";
          }
          else msg = "notemp&time";           
        }
        // sends message back to pebble 
        if(param != "standby") Pebble.sendAppMessage({ "0": msg });
    };
    req.ontimeout = function(e) {
      Pebble.sendAppMessage({ "0": "timeout" });
    };
    req.onerror = function(e) {
      Pebble.sendAppMessage({ "0": "network error" });
    };
    req.open(method, url, async);
    req.send(param);
}
