let mqtt;
let reconnectTimeout = 2000;
let host = ""; /mqtt host
let port = 8084;

let dooropen = new Audio("dooropened.mp3");
let doorclosed = new Audio("doorclosed.mp3");
let connected = new Audio("connected.mp3");
let failed = new Audio("failed.mp3");
let buzzer = new Audio("buzzeroff.mp3");

function onFail() {
  failed.play();
  document.getElementById("connectionStatus").innerHTML =
    "Wrong user or password";
}
// subscribe to topic door/buzzer after connection
function onConnect() {
  document.getElementById("connectionStatus").innerHTML = "";
  connected.play();
  document.getElementById("control").style.display = "block";
  let elem = document.getElementById("loginform");
  elem.parentNode.removeChild(elem);
  // console.log("Connected ");
  mqtt.subscribe("door/buzzer");
}
//subscribed topics msgs handle
function onMessageArrived(r_message) {
  if (r_message.payloadString == "SOMEONE IS AT THE DOOR!") {
    document.getElementById("detector").innerHTML = "SOMEONE IS AT THE DOOR!";
  } else {
    document.getElementById("detector").innerHTML = "";
  }
  out_msg = "Message received: " + r_message.payloadString;
  // console.log(out_msg);
}
function MQTTconnect() {
  user = document.getElementById("user").value;
  passwd = document.getElementById("passwd").value;

  //console.log(user + " " + passwd);

  //console.log("connecting to " + host + " " + port);

  let cname = "webcontroller";
  mqtt = new Paho.MQTT.Client(host, port, cname);
  var options = {
    useSSL: true,
    timeout: 3,
    userName: user,
    password: passwd,
    onSuccess: onConnect,
    onFailure: onFail,
  };
  mqtt.onMessageArrived = onMessageArrived;
  mqtt.connect(options);
}

// send open to door/isopen topic
function openDoor() {
  message = new Paho.MQTT.Message("open");
  message.destinationName = "door/isopen";
  mqtt.send(message);
  dooropen.play();
}
// send close to door/isopen topic
function closeDoor() {
  message = new Paho.MQTT.Message("close");
  message.destinationName = "door/isopen";
  mqtt.send(message);
  doorclosed.play();
}
// send turnoff to door/buzzeroff topic
function stopBuzzer() {
  message = new Paho.MQTT.Message("turnoff");
  message.destinationName = "door/buzzeroff";
  mqtt.send(message);
  buzzer.play();
}
