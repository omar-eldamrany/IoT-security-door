#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

Servo servo;

const char *ssid = "HalaMadrid";      // Enter your WiFi name
const char *password = "+Ramos1854";  // Enter WiFi password

const char *mqtt_broker = "m9fea71f.eu-central-1.emqx.cloud";
const char *topic_pub = "door/buzzer";
const char *topic_sub_1 = "door/buzzeroff";
const char *topic_sub_2 = "door/isopen";
const char *mqtt_username = "nodeMCU";
const char *mqtt_password = "enternode";
const int mqtt_port = 15167;

WiFiClient espClient;
PubSubClient client(espClient);

// int door = 11;
int buzzer = 12;
int pir = 13;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "NodeMCU Client";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the our mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to our private broker");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  servo.attach(4);
  servo.write(0);

  client.subscribe(topic_sub_1);
  delay(2000);
  client.subscribe(topic_sub_2);
  pinMode(pir, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  // pinMode(door,OUTPUT);
}
int buzzerFlag = 0;
String msg = "";

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg += (char)payload[i];
  }
  if (msg == "turnoff")
    buzzerFlag = 0;
  else {
    controldoor(msg);
  }
  Serial.println();
  Serial.println("-----------------------");
  msg = "";
}
void controldoor(String msg) {
  if (msg == "open") {
    digitalWrite(BUILTIN_LED, LOW);
    servo.write(90);
  }

  else if (msg == "close") {
    digitalWrite(BUILTIN_LED, HIGH);
    servo.write(0);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  int state = digitalRead(pir);
  if (state == HIGH) {
    buzzerFlag = 1;
    while (buzzerFlag == 1) {
      digitalWrite(buzzer, HIGH);
      Serial.println("MOTION DETECTED");
      client.publish(topic_pub, "SOMEONE IS AT THE DOOR!");
      delay(2000);
      client.loop();
    }
  }
  digitalWrite(buzzer, LOW);
  Serial.println("Quiet");
  client.publish(topic_pub, "it's quiet");
  delay(2000);
  client.loop();
}
