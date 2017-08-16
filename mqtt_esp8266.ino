#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>
/* -----------------------------------------
library below is used for saving router wifi and pass. Also one can change ssid and pass if router is changed via mobile.
You can look into this library in github for more info. I have not used this library.
-------------------------------------------- */
#include <WiFiManager.h> 

// Update these with values suitable for your network.

const char* ssid = "*********"; // Your wifi SSID
const char* password = "***********"; // Your wifi Password
const char* mqtt_server = "m11.cloudmqtt.com"; //You can input the name or IP of your mqtt broker.
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int LED = 2;
WiFiManager wifiManager;

void setup() {
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(LED,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
  Serial.begin(115200);
  //wifiManager.autoConnect("SSID", "Password"); just used for demonstration.
  setup_wifi();
  client.setServer(mqtt_server, 19845); // here 19845 is the port No.
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == 'O' && (char)payload[1]=='f') {   // I used Off for switcing On the light (led on node mcu is negative bias)
    digitalWrite(LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else { 
    digitalWrite(LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ID","(username","password")) { // input these values if your mqtt broker provide these info they make connection secure.
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello"); // hello is the publishing topic
      // ... and resubscribe
      client.subscribe("hello/world"); // hello/world subscription topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  //Serial.println("Hello");
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello #%ld", value);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    //client.publish("outTopic", msg);
  }
}
