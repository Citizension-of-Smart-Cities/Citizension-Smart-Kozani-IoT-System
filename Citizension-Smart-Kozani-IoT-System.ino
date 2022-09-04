/********************************
 * Libraries included
 *******************************/
#include <Ethernet.h>
#include <SPI.h>
#include <UbidotsEthernet.h>
#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;
/********************************
 * Constants and objects
 *******************************/
/* Assigns the Ubidots parameters */
char const * TOKEN = "Ubidots token"; // Assign your Ubidots TOKEN
char const * VARIABLE_LABEL_1 = "temperature"; // Assign the unique variable label to send the data
char const * VARIABLE_LABEL_2 = "CO2"; // Assign the unique variable label to send the data
char const * VARIABLE_LABEL_3 = "TVOC"; // Assign the unique variable label to send the data
char const * VARIABLE_LABEL_4 = "Decibel"; // Assign the unique variable label to send the data

const int MIC = 0; //the microphone amplifier output is connected to pin A0
int adc;
int dB, PdB; //the variable that will hold the value read from the microphone each time

/* Enter a MAC address for your controller below */
/* Newer Ethernet shields have a MAC address printed on a sticker on the shield */
byte mac[] = { 0xAE, 0xAD, 0xBE, 0xEF, 0xBE, 0xED };

/* initialize the instance */
Ubidots client(TOKEN);

/********************************
 * Main Functions
 *******************************/
void setup() {
  Serial.begin(9600);
  //client.setDebug(true);// uncomment this line to visualize the debug message
  /* start the Ethernet connection */
  Serial.print(F("Starting ethernet..."));
  if (!Ethernet.begin(mac)) {
    Serial.println(F("failed"));
  } else {
    Serial.println(Ethernet.localIP());
  }
  /* Give the Ethernet shield a second to initialize */
  delay(2000);
  Serial.println(F("Ready"));
  pinMode(3, OUTPUT);
  Serial.println("CCS811 test");

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }

  // Wait for the sensor to be ready
  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
}

void loop() {
  Ethernet.maintain();
  PdB = dB; //Store the previous of dB here
  adc= analogRead(MIC); //Read the ADC value from amplifer 
  //Serial.println (adc);//Print ADC for initial calculation 
  dB = (adc+83.2073) / 11.003; //Convert ADC value to dB using Regression values
  if (PdB!=dB)
    
  if(ccs.available()){
    if(!ccs.readData()){
      /* Sensors readings */
      float temp= ccs.calculateTemperature();
      float co2 = ccs.geteCO2();
      float TVOC = ccs.getTVOC();
      float Temp =temp;
      float Decibel = dB;
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.print(ccs.getTVOC());
      Serial.print("ppb, Temp: ");
      Serial.print(temp);
      Serial.print(" C, Decibel: ");
      Serial.print (dB);
      Serial.println("dB");
      /* Sending values to Ubidots */
      client.add(VARIABLE_LABEL_1, temp);
      client.add(VARIABLE_LABEL_2, co2);
      client.add(VARIABLE_LABEL_3, TVOC);
      client.add(VARIABLE_LABEL_4, Decibel);
      client.sendAll();
    }
    else{
      Serial.println("ERROR!");
      while(1);
    }
  }
  delay(1000);
}