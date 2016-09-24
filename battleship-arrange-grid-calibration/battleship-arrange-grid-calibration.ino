/*
 * Sources of the human interface devices used by the battleship game.
 *
 * A project in collaboration with makerspace - Faculty of Computer Science 
 * at the Free University of Bozen-Bolzano.
 * 
 *                                                                         
 *    m  a  k  e  r  s  p  a  c  e  .  i  n  f  .  u  n  i  b  z  .  i  t  
 *                                                                         
 *   8888888888888888888888888888888888888888888888888888888888888888888888
 *                                                                         
 *                  8                                                      
 *                  8                                                      
 *   YoYoYo. .oPYo. 8  .o  .oPYo. YoYo. .oPYo. 8oPYo. .oPYo. .oPYo. .oPYo. 
 *   8' 8' 8 .oooo8 8oP'   8oooo8 8  `  Yb..`  8    8 .oooo8 8   `  8oooo8 
 *   8  8  8 8    8 8 `b.  8.  .  8      .'Yb. 8    8 8    8 8   .  8.  .  
 *   8  8  8 `YooP8 8  `o. `Yooo' 8     `YooP' 8YooP' `YooP8 `YooP' `Yooo' 
 *                                             8                           
 *                                             8                           
 *                                                                         
 *   8888888888888888888888888888888888888888888888888888888888888888888888
 *                                                                         
 *    c  o  m  p  u  t  e  r    s  c  i  e  n  c  e    f  a  c  u  l  t  y 
 *                                                                         
 *                                                                         
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Julian Sanin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>

#include <SpiDevice.h>

#include "LaserPhotoresistorArray.h"

enum {
	LASER_ROWS           = 8,
	LASER_COLUMNS        = 8,
	PIN_SS_LASER_ROWS    = 9,       // Digital pin 9 (PORTB.PB1).
	PIN_SS_LASER_COLUMNS = 10,      // Digital pin 10 (PORTB.PB2).
	F_SCK_LASER_ARRAY    = 2000000, // Frequency in Hz.
	PIN_SIG_LED          = 8,       // Digital pin 8.
	SIG_LED              = HIGH,
	SIG_LED_DURATION     = 1000,    // Time between toggle in ms.
	SERIAL_SPEED         = 115200   // Baudrate
};

LaserPhotoresistorArray<
	SpiDevice<PIN_SS_LASER_ROWS, F_SCK_LASER_ARRAY>
> photoresistorRow;

LaserPhotoresistorArray<
	SpiDevice<PIN_SS_LASER_COLUMNS, F_SCK_LASER_ARRAY>
> photoresistorColumn;

uint8_t rowValues[LASER_ROWS] = { 0 };
uint8_t columnValues[LASER_COLUMNS] = { 0 };

void setup() {
	photoresistorRow.begin();
	photoresistorColumn.begin();
	Serial.begin(SERIAL_SPEED);
	pinMode(PIN_SIG_LED, OUTPUT);
}

void loop() {
	digitalWrite(PIN_SIG_LED, SIG_LED);
	Serial.print("   rows: ");
	photoresistorRow.read(rowValues, LASER_ROWS);
	printValues(rowValues, LASER_ROWS);
	Serial.print("columns: ");
	photoresistorColumn.read(columnValues, LASER_COLUMNS);
	printValues(columnValues, LASER_COLUMNS);
	digitalWrite(PIN_SIG_LED, !SIG_LED);
	delay(SIG_LED_DURATION);
}

void printValues(const uint8_t * /*[in]*/ photoresistorValues, uint8_t length) {
	char hexValue[5] = { 0 };
	for (uint8_t i = 0; i < length; i++) {
		sprintf(hexValue, "0x%02X", photoresistorValues[i]);
		Serial.print(hexValue);
		Serial.print(' ');
	}
	Serial.println();
}
