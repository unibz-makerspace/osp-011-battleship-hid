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

#include "GameGrid.h"

/// <summary>
/// Attacker grid driver. Each item can be sensed by using the red RGB LED as a
/// light sensor and be colored after a given event has been detected.
/// </summary>
template<
	typename RgbLedMatrix,
	typename RgbLedPhotodiodeArray,
	uint8_t MAX_ROWS = 8, uint8_t MAX_COLUMNS = 8,
	uint8_t FPS = 100
>
class AttackGrid : public GameGrid::Tile {

	enum {
		tDiffMicros = 1000000UL / FPS / MAX_COLUMNS,
		COLUMN_START = 0,
		COLUMN_MAX = MAX_COLUMNS,
	};

	static RgbLedMatrix rgbLedMatrix;
	static RgbLedPhotodiodeArray rgbLedPhotodiodeArray;
	static Tile::Type tiles[MAX_ROWS][MAX_COLUMNS];

	static void displayAndSenseAlgorithm() {
		static uint8_t column = 0;
		// Determine which color of the current column LEDs should be enabled.
		uint8_t colReds = 0x00, colGreens = 0x00, colBlues = 0x00;
		for (uint8_t row = 0; row < MAX_ROWS; row++) {
			Tile::Type tile = tiles[row][column];
			const uint8_t enabledColor = (1 << row);
			switch (tile) {
			case Tile::Type::DESTROYED:
				colReds |= enabledColor;
				break;
			case Tile::Type::HIT:
				colReds |= enabledColor;
				colGreens |= enabledColor;
				break;
			default:
				colGreens |= enabledColor;
				colBlues |= enabledColor;
			}
		}
		// Write the result to the shift registers of the LED matrix.
		rgbLedMatrix.writeColumn(colReds, colGreens, colBlues, column);
		// Wait for the red leds such that they charge up with photons.
		delayMicroseconds(tDiffMicros / 10);
		// Takes 85us per scan @ SCK 2MHz.
		//rgbLedSenseAlgortihm(column);
		// Update column.
		column++;
		if (column >= MAX_COLUMNS) {
			column = 0; // Restart on first column.
		}
	}

	static void rgbLedSenseAlgortihm(uint8_t column) {
		uint8_t redLedPhotodiodesLit[MAX_ROWS] = { 0 };
		rgbLedPhotodiodeArray.read(redLedPhotodiodesLit, MAX_ROWS);
		char str[100];
		snprintf(str, 100, "col=%d "
			"[0:0x%02X 1:0x%02X 2:0x%02X 3:0x%02X "
			"4:0x%02X 5:0x%02X 6:0x%02X 7:0x%02X]",
			column,
			redLedPhotodiodesLit[0],
			redLedPhotodiodesLit[1],
			redLedPhotodiodesLit[2],
			redLedPhotodiodesLit[3],
			redLedPhotodiodesLit[4],
			redLedPhotodiodesLit[5],
			redLedPhotodiodesLit[6],
			redLedPhotodiodesLit[7]
		);
		Serial.println(str);
	}

public:
	static void begin() {
		rgbLedMatrix.begin();
		rgbLedPhotodiodeArray.begin();
	}

	static void run() {
		static unsigned long tStartMicros = micros();
		unsigned long tStopMicros = micros();
		if ((tStopMicros - tStartMicros) >= tDiffMicros) {
			displayAndSenseAlgorithm();
			tStartMicros = tStopMicros;
		}
	}

	static void setTile(uint8_t row, uint8_t column, Tile::Type type) {
		tiles[row][column] = type;
	}
};

template<
	typename RgbLedMatrix,
	typename RgbLedPhotodiodeArray,
	uint8_t MAX_ROWS, uint8_t MAX_COLUMNS,
	uint8_t FPS
>
GameGrid::Tile::Type AttackGrid<
	RgbLedMatrix,
	RgbLedPhotodiodeArray,
	MAX_ROWS, MAX_COLUMNS,
	FPS
>::tiles[MAX_ROWS][MAX_COLUMNS];

#endif // ATTACK_GRID_H
