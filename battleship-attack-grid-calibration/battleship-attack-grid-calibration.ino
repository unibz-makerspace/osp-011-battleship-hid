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

AttackGrid <
	RgbLedMatrix<
	SpiDevicePortB<PIN_SS_LED_MATRIX, F_SCK_LED_MATRIX>
	>,
	RgbLedPhotodiodeArray<
	SpiDevicePortB<PIN_SS_PHOTODIODE_ARRAY, F_SCK_PHOTODIODE_ARRAY>
	>,
	LED_MATRIX_ROWS, LED_MATRIX_COLUMNS
> attackGrid;

//FirmataExt firmataExt;
//FirmataReporting reporting;

void setup() {
	//setupFirmata();
	Serial.begin(250000);
	attackGrid.begin();
	pinMode(PIN_SIG_LED, OUTPUT);
	//setPattern();
	//attackGrid.setTile(7, 1, GameGrid::Tile::Type::HIT);
}

void loop() {
	//loopFirmata();
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

/*void setupFirmata() {
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
}*/

/*void setPattern() {
	GameGrid::Tile::Type type = GameGrid::Tile::Type::WATER;
	static int i = 0;
	static int row = 0;
	static int column = 0;
	for (int i=0; i < LED_MATRIX_ROWS * LED_MATRIX_COLUMNS; i++) {
		attackGrid.setTile(row, column++, type[i]);
		if (column == LED_MATRIX_COLUMNS) {
			column = 0;
			row++;
			if (row == LED_MATRIX_ROWS) {
				row = 0;
			}
		}
	}
}*/

/*
// Proposed colors:
const CRGB colors[] = {
CRGB::Cyan,   // Water.
CRGB::Yellow, // Hit.
CRGB::Red     // Sunk (blinking).
};
*/
