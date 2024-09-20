/**
 * @file epaper_arduino.ino
 * @brief Meant to be run on the Arduino in the Auto Dick 3.
 * 
 * This code reads the state of the hardware, encodes it, 
 * and sends it to the Epaper for processing over an I2C connection.
 * It receives data from the Epaper about the hardware actions to take. 
 */
#include <Arduino.h>
#include "lib/GPS.h"
#include <Wire.h>

#define PINMP3 8

#define PINAfbreken 9
#define PINOvertreding 10
#define PINToeter 11
#define PINFinish 12
#define PINStart 13

#define PINSingle A0
#define PINSequenceA A1
#define PINSequenceB A2
#define PINFlag A3
#define PINModeA 5
#define PINModeB 6

const int BUTTONS[] {PINStart, PINOvertreding, PINToeter, PINFinish, PINAfbreken};
const int SWITCHES[] {PINModeB, PINModeB, PINFlag, PINSequenceB, PINSequenceA, PINSingle};

SoftwareSerial ss(4,3);
TinyGPSPlus tinyGPS;
GPS gps(tinyGPS, ss); 

/**
 * @brief Sends hardware state data over I2C.
 *
 * Sends an 11 byte message:
 * | 0 : buttons | 1 : switches | 2 : encoder | 3 : SD-card | 4-10 : GPS |
 */
void sendData() {
	byte switches = B00000000;
	byte buttons = B00000000;

	for (size_t i = 0; i < (sizeof(SWITCHES) / sizeof(SWITCHES[0])); i++)
		switches |= (digitalRead(SWITCHES[i]) << i);

	for (size_t i = 0; i < (sizeof(BUTTONS) / sizeof(BUTTONS[0])); i++)
		buttons |= (digitalRead(BUTTONS[i]) << i);

	static struct timeStruct t;
	gps.getTime(t);

	Wire.write(switches);
	Wire.write(buttons);
	Wire.write(0); // TODO: implement encoder
	Wire.write(1); // TODO: implement SD-card
	Wire.write(t.Day);
	Wire.write(t.Month);	
	Wire.write(t.Year);
	Wire.write(t.Hour);
	Wire.write(t.Minute);
	Wire.write(t.Second);
	Wire.write(1); // TODO: determen if GPS has fix
}

/**
 * @brief Receives hardware state data over I2C.
 *
 * This function has not been implemented yet.
 * It receives 12 bytes:
 * | 0-5 : button & switches LED state | 6 : audio frament | 7 : settings state | 8-10 : time | 11 : flag | 
 */
void receiveData(int dataC){
	Serial.print("Epaper data: ");

	while (Wire.available() > 0)
		Serial.print(Wire.read());

	Serial.println();
}

/**
 * @brief Plays next audio on the MP3
 */
void playNext(){
	digitalWrite(PINMP3, 1);
	delay(100);
	digitalWrite(PINMP3, 0);
	delay(100);
	digitalWrite(PINMP3, 1);
}

void setup() {
	Serial.begin(115200);
	ss.begin(9600);

	Wire.begin(8);
  	Wire.onRequest(sendData);
	Wire.onReceive(receiveData);

	pinMode(PINMP3, OUTPUT);
	for (size_t i = 0; i < (sizeof(BUTTONS) / sizeof(BUTTONS[0])); i++)
		pinMode(BUTTONS[i], INPUT);
}

void loop() {
	gps.updateGPS();

	if (digitalRead(PINToeter)){
		Serial.println("PLAY");
		playNext();
	}
}