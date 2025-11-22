#include "WebServerManager.h"

// Helper for URL decoding
int h2int(char c) {
    if (c >= '0' && c <='9') return c - '0';
    if (c >= 'a' && c <='f') return c - 'a' + 10;
    if (c >= 'A' && c <='F') return c - 'A' + 10;
    return 0;
}

WebServerManager::WebServerManager()
: server(80) {}


void WebServerManager::init() {
    server.begin();
    _running = true;
}


void WebServerManager::update(unsigned long now) {
    if (!_running) return;

    WiFiClient client = server.available();
    if (client) {
        handleClient();
    }
}


bool WebServerManager::isRunning() const {
    return _running;
}

/* ---------------------------------------------------
   Callback Attach
--------------------------------------------------- */

void WebServerManager::attachMotorOutputCallback(void (*cb)(uint8_t)) {
    motorOutputCallback = cb;
}

void WebServerManager::attachMotorDirCallback(void (*cb)(int)) {
    motorDirCallback = cb;
}

void WebServerManager::attachServoAngleCallback(void (*cb)(int)) {
    servoAngleCallback = cb;
}

/* ---------------------------------------------------
   HTTP Request handlers
--------------------------------------------------- */

void WebServerManager::handleClient() {
    WiFiClient client = server.available();

    if (client) {
        String currentLine = "";
        String requestLine = "";
        String requestBody = "";
        bool isBody = false;
        int contentLength = 0;

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        // Empty line means headers are done
                        isBody = true;

                        // Read body if POST request
                        if (contentLength > 0) {
                            for (int i=0; i < contentLength && client.available(); ++i) {
                                requestBody += (char)client.read();
                            }
                        }
                        break;
                    } else {
                        // Process header line
                        if (requestLine.length() == 0) {
                            requestLine = currentLine;
                        }

                        // Check for Content-Length
                        if (currentLine.startsWith("Content-Length: ")) {
                            contentLength = currentLine.substring(16).toInt();
                        }

                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }

        // Parse request
        String method = "";
        String path = "";

        int firstSpace = requestLine.indexOf(' ');
        int secondSpace = requestLine.indexOf(' ', firstSpace + 1);

        if (firstSpace > 0 && secondSpace > 0) {
            method = requestLine.substring(0, firstSpace);
            path = requestLine.substring(firstSpace+1, secondSpace);
        }

        //Route handling
        if (method == "GET" && path == "/") {
            sendHTMLResponse(client);
        } else if (method == "POST" && path == "/setMotorOutput") {
            String value = getParam(requestBody, "value");
            if (value.length() > 0) {
                int val = value.toInt();
                Serial.print("<Webserver log> val: ");
                Serial.println(val);
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                if (motorOutputCallback) motorOutputCallback((uint8_t)val);
                sendResponse(client, 200, "text/plain", "OK");
            } else {
                sendResponse(client, 400, "text/plain", "Missing 'value'");
            }
        } else if (method == "POST" && path == "/setMotorDir") {
            String dir = getParam(requestBody, "dir");
            if (dir.length() > 0) {
                if (motorDirCallback) motorDirCallback(dir.toInt());
                sendResponse(client, 200, "text/plain", "OK");
            } else {
                sendResponse(client, 400, "text/plain", "Missing 'dir'");
            }
        } else if (method == "POST" && path == "/setServoAngle") {
            String angle = getParam(requestBody, "angle");
            if (angle.length() > 0) {
                if (servoAngleCallback) servoAngleCallback(angle.toInt());
                sendResponse(client, 200, "text/plain", "OK");
            } else {
                sendResponse(client, 400, "text/plain", "Missing 'angle'");
            }
        } else {
            sendResponse(client, 404, "text/plain", "Page not found");
        }

        // Close connection
        client.stop();
    }
}

// Helper methods
void WebServerManager::sendResponse(WiFiClient& client, int code, const char* contentType, const char* content) {
    client.print("HTTP/1.1 ");
    client.print(code);
    client.println(code == 200 ? " OK" : (code == 400 ? " Bad Request" : " Not Found"));
    client.print("Content-Type: ");
    client.println(contentType);
    client.println("Connection: close");
    client.println();
    client.println(content);
}

void WebServerManager::sendHTMLResponse(WiFiClient& client) {
    const char html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>RC Car Control</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px}
.container{max-width:800px;margin:0 auto;background:#fff;border-radius:20px;box-shadow:0 20px 60px rgba(0,0,0,0.3);overflow:hidden}
.header{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:#fff;padding:20px;text-align:center}
h1{font-size:28px;margin-bottom:5px}
.status{font-size:14px;opacity:0.9}
.camera-section{background:#000;position:relative;padding-bottom:56.25%;overflow:hidden}
#cameraStream{position:absolute;top:0;left:0;width:100%;height:100%;object-fit:contain}
.camera-offline{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);color:#666;text-align:center;font-size:18px}
.control-section{padding:30px}
.motor-control{margin-bottom:30px}
.slider-container{margin-top:15px}
label{display:block;font-weight:600;margin-bottom:10px;color:#333}
input[type="range"]{width:100%;height:8px;border-radius:5px;background:#ddd;outline:none;-webkit-appearance:none}
input[type="range"]::-webkit-slider-thumb{-webkit-appearance:none;width:24px;height:24px;border-radius:50%;background:#667eea;cursor:pointer;box-shadow:0 2px 5px rgba(0,0,0,0.2)}
input[type="range"]::-moz-range-thumb{width:24px;height:24px;border-radius:50%;background:#667eea;cursor:pointer;border:none;box-shadow:0 2px 5px rgba(0,0,0,0.2)}
.slider-value{text-align:center;font-size:24px;font-weight:bold;color:#667eea;margin-top:10px}
.direction-controls{text-align:center}
.direction-grid{display:inline-grid;grid-template-columns:repeat(3,80px);grid-template-rows:repeat(3,80px);gap:10px;margin-top:15px}
.arrow-btn{background:linear-gradient(145deg,#f0f0f0,#cacaca);border:none;border-radius:15px;font-size:32px;cursor:pointer;transition:all 0.1s;box-shadow:5px 5px 10px #b8b8b8,-5px -5px 10px #fff;display:flex;align-items:center;justify-content:center;user-select:none}
.arrow-btn:hover{background:linear-gradient(145deg,#cacaca,#f0f0f0)}
.arrow-btn.pressed{background:linear-gradient(145deg,#667eea,#764ba2);color:#fff;box-shadow:inset 3px 3px 7px #5568c4,inset -3px -3px 7px #7794ff;transform:scale(0.95)}
.arrow-btn:active{transform:scale(0.95)}
#upBtn{grid-column:2;grid-row:1}
#leftBtn{grid-column:1;grid-row:2}
#stopBtn{grid-column:2;grid-row:2;background:linear-gradient(145deg,#ff6b6b,#ee5555);color:#fff;font-size:20px;font-weight:bold}
#rightBtn{grid-column:3;grid-row:2}
#downBtn{grid-column:2;grid-row:3}
.keyboard-hint{margin-top:20px;text-align:center;color:#666;font-size:14px}
.config-section{margin-top:20px;padding:15px;background:#f5f5f5;border-radius:10px}
.config-input{display:flex;gap:10px;margin-top:10px}
input[type="text"]{flex:1;padding:10px;border:2px solid #ddd;border-radius:8px;font-size:14px}
button{padding:10px 20px;background:#667eea;color:#fff;border:none;border-radius:8px;cursor:pointer;font-weight:600;transition:background 0.3s}
button:hover{background:#5568c4}
</style>
</head>
<body>
<div class="container">
<div class="header">
<h1>🚗 RC Car Control</h1>
<div class="status" id="statusText">Ready</div>
</div>
<div class="camera-section">
<img id="cameraStream" src="" alt="Camera Stream">
<div class="camera-offline" id="cameraOffline">📷 Camera Offline<br><small>Configure ESP32-CAM IP below</small></div>
</div>
<div class="control-section">
<div class="config-section">
<label>ESP32-CAM IP Address:</label>
<div class="config-input">
<input type="text" id="cameraIP" placeholder="e.g., 192.168.1.100" value="">
<button onclick="connectCamera()">Connect</button>
</div>
</div>
<div class="motor-control">
<label>Motor Speed:</label>
<div class="slider-container">
<input type="range" id="motorSpeed" min="0" max="255" value="200" oninput="updateMotorSpeed(this.value)">
<div class="slider-value" id="speedValue">200</div>
</div>
</div>
<div class="direction-controls">
<label>Direction Control:</label>
<div class="direction-grid">
<button class="arrow-btn" id="upBtn" onmousedown="pressKey('up')" onmouseup="releaseKey('up')" ontouchstart="pressKey('up')" ontouchend="releaseKey('up')">▲</button>
<button class="arrow-btn" id="leftBtn" onmousedown="pressKey('left')" onmouseup="releaseKey('left')" ontouchstart="pressKey('left')" ontouchend="releaseKey('left')">◀</button>
<button class="arrow-btn" id="stopBtn" onclick="stopCar()">STOP</button>
<button class="arrow-btn" id="rightBtn" onmousedown="pressKey('right')" onmouseup="releaseKey('right')" ontouchstart="pressKey('right')" ontouchend="releaseKey('right')">▶</button>
<button class="arrow-btn" id="downBtn" onmousedown="pressKey('down')" onmouseup="releaseKey('down')" ontouchstart="pressKey('down')" ontouchend="releaseKey('down')">▼</button>
</div>
<div class="keyboard-hint">💡 Use arrow keys on keyboard for control<br>Hold multiple keys for diagonal movement</div>
</div>
</div>
</div>
<script>
const ARDUINO_IP=window.location.hostname||'192.168.1.10';
let motorSpeed=200;
let keysPressed={up:false,down:false,left:false,right:false};
function connectCamera(){
const ip=document.getElementById('cameraIP').value.trim();
if(!ip){alert('Please enter ESP32-CAM IP address');return;}
const streamUrl=`http://${ip}:81/stream`;
const cameraStream=document.getElementById('cameraStream');
const cameraOffline=document.getElementById('cameraOffline');
cameraStream.src=streamUrl;
cameraStream.style.display='block';
cameraOffline.style.display='none';
cameraStream.onerror=function(){
cameraOffline.style.display='block';
cameraStream.style.display='none';
updateStatus('Camera connection failed','error');
};
cameraStream.onload=function(){updateStatus('Camera connected','success');};
localStorage.setItem('esp32camIP',ip);
}
window.onload=function(){
const savedIP=localStorage.getItem('esp32camIP');
if(savedIP){document.getElementById('cameraIP').value=savedIP;}
};
function updateMotorSpeed(value){
motorSpeed=parseInt(value);
document.getElementById('speedValue').textContent=value;
fetch(`http://${ARDUINO_IP}/setMotorOutput`,{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:`value=${motorSpeed}`
})
.then(response=>response.text())
.then(data=>updateStatus('Speed updated: '+motorSpeed))
.catch(err=>updateStatus('Connection error','error'));
}
function pressKey(key){
keysPressed[key]=true;
updateControl();
}
function releaseKey(key){
keysPressed[key]=false;
updateControl();
}
function updateControl(){
updateUI();
let dir=2;
if (keysPressed.up) dir=0;
else if (keysPressed.down) dir=1;
else dir=2;
sendMotorCommand(dir);
let angle=105;
if (keysPressed.left) angle=90;
else if (keysPressed.right) angle=120;
sendServoCommand(angle);
let status="";
if (dir===0) status="Forward";
else if (dir===1) status="Backward";
else status="Stopped";
if (keysPressed.left) status+=" Left";
else if (keysPressed.right) status+=" Right";
updateStatus(status);
}
function updateUI(){
document.getElementById('upBtn').classList.toggle('pressed',keysPressed.up);
document.getElementById('downBtn').classList.toggle('pressed',keysPressed.down);
document.getElementById('leftBtn').classList.toggle('pressed',keysPressed.left);
document.getElementById('rightBtn').classList.toggle('pressed',keysPressed.right);
}
function stopCar(){
keysPressed={up:false,down:false,left:false,right:false};
updateControl();
}
function sendMotorCommand(dir){
fetch(`http://${ARDUINO_IP}/setMotorDir`,{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:`dir=${dir}`
}).catch(err=>console.error('Motor command failed:',err));
}
function sendServoCommand(angle){
fetch(`http://${ARDUINO_IP}/setServoAngle`,{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:`angle=${angle}`
}).catch(err=>console.error('Servo command failed:',err));
}
const keyMap={'ArrowUp':'up','ArrowDown':'down','ArrowLeft':'left','ArrowRight':'right','w':'up','s':'down','a':'left','d':'right'};
document.addEventListener('keydown',function(e){
const key=keyMap[e.key];
if(key){
e.preventDefault();
if(!keysPressed[key]){
pressKey(key);
}
}
if(e.key===' '){e.preventDefault();stopCar();}
});
document.addEventListener('keyup',function(e){
const key=keyMap[e.key];
if(key){e.preventDefault();releaseKey(key);}
});
function updateStatus(message,type='info'){
const statusText=document.getElementById('statusText');
statusText.textContent=message;
if(type==='error'){statusText.style.color='#ff6b6b';}
else if(type==='success'){statusText.style.color='#51cf66';}
else{statusText.style.color='white';}
setTimeout(()=>{statusText.style.color='white';},2000);
}
</script>
</body>
</html>)rawliteral";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    // Send HTML in chunks to avoid buffer overflow
    const char* ptr = html;
    while (*ptr) {
        client.write(*ptr++);
    }
}

String WebServerManager::urlDecode(String str) {
    String decoded = "";
    char c;
    char code0;
    char code1;
    for (unsigned int i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (c == '+') {
            decoded += ' ';
        } else if (c == '%') {
            i++;
            code0 = str.charAt(i);
            i++;
            code1 = str.charAt(i);
            c = (h2int(code0) << 4) | h2int(code1);
            decoded += c;
        } else {
            decoded += c;
        }
    }
    return decoded;
}


String WebServerManager::getParam(String data, String param) {
    String searchStr = param + "=";
    int start = data.indexOf(searchStr);
    if (start == -1) return "";
    
    start += searchStr.length();
    int end = data.indexOf('&', start);
    if (end == -1) end = data.length();
    
    return urlDecode(data.substring(start, end));
}