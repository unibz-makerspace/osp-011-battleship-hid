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

#ifndef ATTACK_GRID_H
#define ATTACK_GRID_H

#include <Arduino.h>
#include <stdint.h>

#include <TimerOne.h>
#include <FastLED.h>

/// <summary>
/// Attacker grid driver. Each item can be sensed by using the red RGB LED as a
/// light sensor and be colored after a given event has been detected.
/// </summary>
template<
	typename RgbLedMatrix,
	typename RgbLedPhotodiodeArray,
	uint8_t MAX_ROWS = 8, uint8_t MAX_COLUMNS = 8,
	uint8_t FPS = 100, uint8_t MAX_BITS_PER_COLOR = 2
>
class AttackGrid {

	static RgbLedMatrix rgbLedMatrix;
	static RgbLedPhotodiodeArray rgbLedPhotodiodeArray;
	static CRGB rgbLeds[MAX_ROWS * MAX_COLUMNS];

	enum BcmConstants {
		BCM_TIME_BASE =
				1000000UL /*us*/ / 255U /*max. 8-bit*/ / FPS / MAX_COLUMNS,
		BCM_MSB_BITMASK = 0x80,
		BCM_LSB_BITMASK = (BCM_MSB_BITMASK >> MAX_BITS_PER_COLOR),
		BCM_COLUMN_START = 0,
		BCM_COLUMN_MAX = MAX_COLUMNS,
	};

	static void binaryCodeModulationAlgorithm() {
		static uint8_t bcmBitmask = BCM_MSB_BITMASK;
		static uint8_t column = 0;
		// Determine which color of the current column LEDs should be enabled.
		uint8_t rowReds = 0x00, rowGreens = 0x00, rowBlues = 0x00;
		for (uint8_t row = 0; row < MAX_ROWS; row++) {
			CRGB rgbLed = rgbLeds[row + column];
			if (rgbLed.r & bcmBitmask) {
				// Enable red LED of given row and column.
				rowReds |= (1 << row);
			}
			if (rgbLed.g & bcmBitmask) {
				// Enable green LED of given row and column.
				rowGreens |= (1 << row);
			}
			if (rgbLed.b & bcmBitmask) {
				// Enable blue LED of given row and column.
				rowBlues |= (1 << row);
			}
		}
		// Write the result to the shift registers of the LED matrix.
		rgbLedMatrix.writeColumn(rowReds, rowGreens, rowBlues, column);
		// Prepare for the next BCM interval.
		bcmBitmask >>= 1;
		if (bcmBitmask == BCM_LSB_BITMASK) {
			bcmBitmask = BCM_MSB_BITMASK;
			column++;
			if (column >= BCM_COLUMN_MAX) {
				column = BCM_COLUMN_START;
				for (uint8_t col = 0; col < MAX_COLUMNS; col++) {
					// Takes 85us per scan @ SCK 2MHz.
					rgbLedSenseAlgortihm(col);
				}
			}
		}
		Timer1.initialize((uint32_t)BCM_TIME_BASE * bcmBitmask);
	}

	static void rgbLedSenseAlgortihm(uint8_t column) {
		uint8_t rLedPhotodiodesLit[MAX_ROWS] = { 0 };
		uint8_t rLedPhotodiodesUnlit[MAX_ROWS] = { 0 };
		// Enable only blue LEDs and wait for 1us to settle before sensing.
		uint8_t rowReds = 0x00, rowGreens = 0xFF, rowBlues = 0xFF;
		rgbLedMatrix.writeColumn(rowReds, rowGreens, rowBlues, column);
		//delayNanoseconds();
		rgbLedPhotodiodeArray.read(rLedPhotodiodesLit, MAX_ROWS);
		// Disable blue LEDs and wait for 1us to settle before sensing.
		rowGreens = 0xFF; rowBlues = 0x00;
		rgbLedMatrix.writeColumn(rowReds, rowGreens, rowBlues, column);
		//delayNanoseconds();
		rgbLedPhotodiodeArray.read(rLedPhotodiodesUnlit, MAX_ROWS);
		
		// TODO: Write code to transmit sensed positions back to the computer.

		/*
		//Serial.print(column); Serial.print(":");
		for (uint8_t i = 0; i < 1; i++) {
			//Serial.print(" 0x");
			//Serial.print(rLedPhotodiodesLit[i], HEX);
			Serial.print(rLedPhotodiodesLit[i] - rLedPhotodiodesUnlit[i]);
		}
		Serial.println();
		*/
	}

public:
	static void begin() {
		rgbLedMatrix.begin();
		rgbLedPhotodiodeArray.begin();
		Timer1.initialize((uint32_t)BCM_TIME_BASE * BCM_MSB_BITMASK);
		Timer1.attachInterrupt(binaryCodeModulationAlgorithm);
	}
};

template<
	typename RgbLedMatrix,
	typename RgbLedPhotodiodeArray,
	uint8_t MAX_ROWS, uint8_t MAX_COLUMNS,
	uint8_t FPS, uint8_t MAX_BITS_PER_COLOR
>
CRGB AttackGrid<
	RgbLedMatrix,
	RgbLedPhotodiodeArray,
	MAX_ROWS, MAX_COLUMNS,
	FPS, MAX_BITS_PER_COLOR
>::rgbLeds[MAX_ROWS * MAX_COLUMNS] = { CRGB::Black };

#endif // ATTACK_GRID_H
