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
	static uint8_t baseLevel[MAX_ROWS][MAX_COLUMNS];

	static void testPattern(uint8_t column) {
		static int i = 0;
		if (column == 0) {
			//i++;
			//if ((i % 100) == 0) {
				for (uint8_t row = 0; row < MAX_ROWS; row++) {
					for (uint8_t column = 0; column < MAX_COLUMNS; column++) {
						tiles[row][column] = GameGrid::Tile::Type::HIT;
					}
				}
				for (uint8_t row = 0; row < MAX_ROWS; row++) {
					tiles[row][0] = GameGrid::Tile::Type::NONE;
				}
			//}
		}
	}

	static void displayAndSenseAlgorithm() {
		static uint8_t column = 0;
		// Determine which color of the current column LEDs should be enabled.
		uint8_t colReds = 0x00, colGreens = 0x00, colBlues = 0x00;
		//testPattern(column);
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
			case Tile::Type::WATER:
				colBlues |= enabledColor;
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
		rgbLedSenseAlgortihm(column);
		// Update column.
		column++;
		if (column >= MAX_COLUMNS) {
			column = 0; // Restart on first column.
		}
	}

	static void testRead(uint8_t column, uint8_t * diodes) {
		static int i = 0;
		if (column == 0) {
			i++;
			if ((i % 100) == 0) {
				char str[80];
				sprintf(str, "(0,%d)=%d (1,%d)=%d (2,%d)=%d (3,%d)=%d (4,%d)=%d (5,%d)=%d (6,%d)=%d (7,%d)=%d",
					column, diodes[0],
					column, diodes[1],
					column, diodes[2],
					column, diodes[3],
					column, diodes[4],
					column, diodes[5],
					column, diodes[6],
					column, diodes[7]
				);
				Firmata.sendString(str);
			}
		}
	}

	static void rgbLedSenseAlgortihm(uint8_t column) {
		uint8_t redLedPhotodiodesLit[MAX_ROWS] = { 0 };
		rgbLedPhotodiodeArray.read(redLedPhotodiodesLit, MAX_ROWS);
		//testRead(column, redLedPhotodiodesLit);
		for (uint8_t row = 0; row < MAX_ROWS; row++) {
			// Assume nothing is placed.
			if (baseLevel[row][column] == 0) {
				baseLevel[row][column] = redLedPhotodiodesLit[row];

				char str[20];
				sprintf(str, "reset(%d,%d)=%d", row, column, baseLevel[row][column]);
				Firmata.sendString(str);
			} else if ((abs(redLedPhotodiodesLit[row] - baseLevel[row][column])
					> 10/*threshold*/) &&
					(tiles[row][column] == GameGrid::Tile::Type::NONE)) {
				setTile(row, column, GameGrid::Tile::Type::SELECTED);
				// TODO: Write code to transmit sensed positions back to the computer.
				//sendTileChangeMessage(row, column);

				char str[20];
				sprintf(str, "  set(%d,%d)=%d", row, column, abs(redLedPhotodiodesLit[row] - baseLevel[row][column]));
				Firmata.sendString(str);
			}
		}
	}

	static void doReset() {
		for (uint8_t row = 0; row < MAX_ROWS; row++) {
			for (uint8_t column = 0; column < MAX_COLUMNS; column++) {
				setTile(row, column, Tile::Type::NONE);
				baseLevel[row][column] = 0;
			}
		}
	}

public:
	static void begin() {
		rgbLedMatrix.begin();
		rgbLedPhotodiodeArray.begin();
		doReset();
	}

	static void run() {
		if (shouldReset == true) {
			doReset();
			shouldReset = false;
		}
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

	void onTileTypeMessageReceived(byte row, byte column, Tile::Type type) {
		char str[24];
		sprintf(str, "(%d,%d)=%s", row, column,
			(type == Tile::Type::WATER) ? "WATER" :
			(type == Tile::Type::HIT) ? "HIT" :
			(type == Tile::Type::DESTROYED) ? "DESTROYED" : "NONE");
		Firmata.sendString(str);
		// TODO: update ui
		setTile(row, column, type);
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
>::tiles[MAX_ROWS][MAX_COLUMNS] = { GameGrid::Tile::Type::NONE };

/*
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
>::tiles[MAX_ROWS * MAX_COLUMNS] = { GameGrid::Tile::Type::NONE };
*/

template<
	typename RgbLedMatrix,
	typename RgbLedPhotodiodeArray,
	uint8_t MAX_ROWS, uint8_t MAX_COLUMNS,
	uint8_t FPS
>
uint8_t AttackGrid<
	RgbLedMatrix,
	RgbLedPhotodiodeArray,
	MAX_ROWS, MAX_COLUMNS,
	FPS
>::baseLevel[MAX_ROWS][MAX_COLUMNS] = { 0 };

/*
template<
	typename RgbLedMatrix,
	typename RgbLedPhotodiodeArray,
	uint8_t MAX_ROWS, uint8_t MAX_COLUMNS,
	uint8_t FPS
>
uint8_t AttackGrid<
	RgbLedMatrix,
	RgbLedPhotodiodeArray,
	MAX_ROWS, MAX_COLUMNS,
	FPS
>::baseLevel[MAX_ROWS * MAX_COLUMNS] = { 0 };
*/

#endif // ATTACK_GRID_H
