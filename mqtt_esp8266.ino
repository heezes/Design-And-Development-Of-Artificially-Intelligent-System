#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// Update these with values suitable for your network.

const char* ssid = "CRYSTAL BROADBAND 52";
const char* password = "8127743728";
const char* mqtt_server = "m11.cloudmqtt.com";
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
  //wifiManager.autoConnect("AP-NAME", "hello@123");
  setup_wifi();
  client.setServer(mqtt_server, 19845);
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
  if ((char)payload[0] == 'O' && (char)payload[1]=='f') {
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
    if (client.connect("Altamash","munnvxsn","KYuAfwQxGVpY")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello");
      // ... and resubscribe
      client.subscribe("hello/world");
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
