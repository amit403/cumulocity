#include "DHT.h"
#include<Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
DHT dht;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_MOSI   D7
#define OLED_CLK   D5
#define OLED_DC    D2
#define OLED_CS    D8
#define OLED_RESET D3

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//const char* ssid = "Kapil_Pana";  //WIFI
const char* ssid = "IOT"; 
//const char* password =  "Pana@sonic2017";  //WIFI Info
const char* password =  "IOT@12345";
const char* mqttServer = "tcp://mqtt.us.cumulocity.com"; //Cumulocity MQTT Server
const int mqttPort = 1883;
const char* mqttUser = "ninja/Amit.Bhandari1@rsystems.com";  //Cumulocity Tenant Information
//const char* mqttUser = "manufacturing/Richard.Wagner@softwareag.com";  //Cumulocity Tenant Information
const char* mqttPassword = "Rsi@12345";
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0


char message[100];
const char* message1 = "200,ADC0Measurement,A,";
char message2[12];
const char* message3 ="ADC0";

char humi_message[100];
const char* humi_message1 = "200,ADC1Measurement,A,";
char humi_message2[12];
const char* humi_message3 = "ADC1";


int sensorValue = 0;  // value read from the pot
int outputValue = 0;  // value to output to a PWM pin

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
 
  Serial.begin(115200);
  Serial.setTimeout(15000);
  dht.setup(D1);
 if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("s/us", "100,ESP8266Device,c8y_ESP8266");  //create device
  client.publish("s/us", message);  //Send measurement
  
  client.subscribe("s/ds");  //subscribe to messages
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

void reconnect()
{
   Serial.println("Attempting to connect");
  while (!client.connected()) {
     if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {

  delay(dht.getMinimumSamplingPeriod());
  // read the analog in value
  //sensorValue = analogRead(analogInPin);
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  float farenheit = (1.8*temperature)+32;
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(farenheit, 1);
  Serial.print("\t\t");
  
  // map it to the range of the PWM out
  outputValue = map(farenheit, 0, 1024, 0, 255);
  sprintf(message2, "%.2f", farenheit);
  strcat(message, message1);
  strcat(message, message2);

  sprintf(humi_message2, "%.2f", humidity);
  strcat(humi_message, humi_message1);
  strcat(humi_message, humi_message2);
  //strcat(message, message3);

 if(client.connected())
 {
  Serial.println(message);
  client.publish("s/us", message);
  memset(message,0,sizeof message);

  Serial.println(humi_message);
  client.publish("s/us", humi_message);
  memset(humi_message,0,sizeof humi_message);
 }
 else{
      Serial.println("Client not connected");
      memset(message,0,sizeof message);
      memset(humi_message,0,sizeof humi_message);
      reconnect();
     }
 
  client.loop();

  display.clearDisplay();
  
  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(temperature);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
  
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(humidity);
  display.print(" %"); 
  
  display.display();
  delay(14000);
  
  
 

}


