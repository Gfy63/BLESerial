
/*

 BLESerial was written by Ian Archbell of oddWires. It is based on the BLE implementation
 originally created by Neil Kolban and included in the Espressif esp32 distribution.
 This library makes it simple to send and received data that would normally go to or be sent by
 the serial monitor. The interface is very similar so most usage is identical.

 To use:

  1. Create an instance of BLESerial
  2. Define it as Serial (not necessary as you can use it directly as your named instance 
     but makes it easy to add to existing programs)
  3. Make sure you call begin()   

 There is a connected() method that enables you to find out whether a bluetooth central manager is connected.

 You will need an Ios or Android app on your phone that will connect to the Nordic BLE Serial UART service
 and use its associated characteristics.  

We suggest using basic-chat which is a Bluetooth Low Energy App for iOS using Swift originally written by Adafruit. 
This fork is updated for Swift 4 and to easily interface with BLESerial library for esp32 on github at: 
https://github.com/iot-bus/basic-chat 

*/
#include <Arduino.h>

#include "BLESerial.h"

BLESerial bleSerial;

void BT_EventHandler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

#define LEDPin 4

void setup() {  

  Serial.begin(115200);

  bleSerial.begin("BLE Serial Loop"); 
		
  bleSerial.register_callback(BT_EventHandler);

  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);

  delay(1000);

}

void loop() {

  if (bleSerial.connected())
  { 

    digitalWrite(LEDPin, LOW);

    if ( bleSerial.available() ) { Serial.write( bleSerial.read() ); }

    if ( Serial.available() ) { bleSerial.write( Serial.read() ); }

  }
  else{
    digitalWrite(LEDPin, HIGH);
  }

}

void BT_EventHandler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{

	switch ( event )
	{
		case ESP_SPP_INIT_EVT:
			break;

		case ESP_SPP_START_EVT:
			break;

		case ESP_SPP_SRV_OPEN_EVT:
     Serial.println( "Client connected" );
			break;
		
		case ESP_SPP_CLOSE_EVT:
			Serial.println("Client disconnected");
			break;

		case ESP_SPP_WRITE_EVT:
			break;

		case ESP_SPP_DATA_IND_EVT:
			break;

		default:
			break;
	}
}