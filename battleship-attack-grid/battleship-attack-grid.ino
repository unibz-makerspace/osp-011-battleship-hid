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

// http://firmatabuilder.com
#include <ConfigurableFirmata.h>
#include <FirmataExt.h>
#include <FirmataReporting.h>

#include "AttackGrid.h"
#include "RgbLedMatrix.h"
#include "RgbLedPhotodiodeArray.h"
#include "SpiDevicePortB.h"
#include "States.h"

enum {
	LED_MATRIX_ROWS         = 8,
	LED_MATRIX_COLUMNS      = 8,
	PIN_SS_LED_MATRIX       = PB2,     // Digital pin 10 (PORTB).
	F_SCK_LED_MATRIX        = 8000000, // Frequency in Hz.
	PIN_SS_PHOTODIODE_ARRAY = PB1,     // Digital pin 9 (PORTB).
	F_SCK_PHOTODIODE_ARRAY  = 2000000, // Frequency in Hz.
	PIN_SIG_LED             = 8,       // Digital pin 8.
	SIG_LED                 = HIGH,
	SIG_LED_DURATION        = 1000,    // Time between toggle in ms.
};

// Change photodiode min/max values if calibration is needed.
template<
	typename RgbLedMatrix,
	typename RgbLedPhotodiodeArray,
	uint8_t MAX_ROWS, uint8_t MAX_COLUMNS,
	uint8_t FPS
>
Photodiode AttackGrid<
	RgbLedMatrix,
	RgbLedPhotodiodeArray,
	MAX_ROWS, MAX_COLUMNS,
	FPS
>::photodiodes[MAX_ROWS][MAX_COLUMNS] = {
	{ { 0x2F,0x67 },{ 0x34,0x66 },{ 0x41,0x63 },{ 0x4B,0x67 },{ 0x48,0x75 },{ 0x3D,0x66 },{ 0x45,0x64 },{ 0x46,0x69 } }, // Row 0
	{ { 0x41,0x5B },{ 0x3E,0x68 },{ 0x42,0x67 },{ 0x3B,0x6A },{ 0x37,0x69 },{ 0x47,0x62 },{ 0x39,0x66 },{ 0x36,0x66 } }, // Row 1
	{ { 0x3D,0x66 },{ 0x3A,0x61 },{ 0x39,0x66 },{ 0x39,0x65 },{ 0x38,0x65 },{ 0x37,0x64 },{ 0x48,0x66 },{ 0x4B,0x69 } }, // Row 2
	{ { 0x48,0x68 },{ 0x38,0x67 },{ 0x34,0x68 },{ 0x34,0x65 },{ 0x43,0x68 },{ 0x45,0x63 },{ 0x34,0x67 },{ 0x3C,0x64 } }, // Row 3
	{ { 0x48,0x64 },{ 0x37,0x61 },{ 0x43,0x6A },{ 0x3D,0x64 },{ 0x41,0x62 },{ 0x4D,0x63 },{ 0x56,0x66 },{ 0x39,0x68 } }, // Row 4
	{ { 0x40,0x5D },{ 0x3B,0x60 },{ 0x39,0x6A },{ 0x41,0x6C },{ 0x42,0x6A },{ 0x3D,0x65 },{ 0x43,0x6A },{ 0x43,0x5A } }, // Row 5
	{ { 0x32,0x5F },{ 0x42,0x66 },{ 0x52,0x6B },{ 0x35,0x6A },{ 0x48,0x68 },{ 0x39,0x69 },{ 0x3F,0x67 },{ 0x42,0x6B } }, // Row 6
	{ { 0x32,0x62 },{ 0x46,0x5A },{ 0x3B,0x66 },{ 0x39,0x68 },{ 0x2C,0x62 },{ 0x2B,0x66 },{ 0x4A,0x66 },{ 0x39,0x62 } }, // Row 7
};

AttackGrid <
	RgbLedMatrix<
	SpiDevicePortB<PIN_SS_LED_MATRIX, F_SCK_LED_MATRIX>
	>,
	RgbLedPhotodiodeArray<
	SpiDevicePortB<PIN_SS_PHOTODIODE_ARRAY, F_SCK_PHOTODIODE_ARRAY>
	>,
	LED_MATRIX_ROWS, LED_MATRIX_COLUMNS
> attackGrid;

FirmataExt firmataExt;
FirmataReporting reporting;

void setup() {
	setupFirmata();
	attackGrid.begin();
	pinMode(PIN_SIG_LED, OUTPUT);
}

void loop() {
	loopFirmata();
	enum { SIG_LED_ON, SIG_LED_OFF, SIG_LED_RESET };
	USING_STATES;
	STATE(SIG_LED_ON) {
		digitalWrite(PIN_SIG_LED, SIG_LED);
		DELAY(SIG_LED_DURATION);
	}
	STATE(SIG_LED_OFF) {
		digitalWrite(PIN_SIG_LED, !SIG_LED);
		DELAY(SIG_LED_DURATION);
	}
	STATE(SIG_LED_RESET) {
		//Firmata.sendString("test");
		GOTO(SIG_LED_ON);
	}
	attackGrid.run();
}

void setupFirmata() {
	Firmata.setFirmwareVersion(FIRMWARE_MAJOR_VERSION, FIRMWARE_MINOR_VERSION);
	Firmata.disableBlinkVersion();
	firmataExt.addFeature(attackGrid);
	Firmata.attach(SYSTEM_RESET, systemResetCallback);
	Firmata.begin();
	systemResetCallback();
}

void loopFirmata() {
	while (Firmata.available()) {
		Firmata.processInput();
	}
	// TODO: Add code to be processed by firmata.
}

void systemResetCallback() {
	// TODO: Add code to be processed by firmata.
	firmataExt.reset();
}

/*
// Proposed colors:
const CRGB colors[] = {
CRGB::Cyan,   // Water.
CRGB::Yellow, // Hit.
CRGB::Red     // Sunk (blinking).
};
*/
