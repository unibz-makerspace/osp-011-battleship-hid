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

#ifndef GAME_GRID_H
#define GAME_GRID_H

#include <Arduino.h>
#include <ConfigurableFirmata.h>
#include <FirmataFeature.h>
#include <stdio.h>
#include <stdint.h>

struct GameGrid {

	static const byte MAX_ROWS = 8;
	static const byte MAX_COLUMNS = 8;

	/// <summary>
	/// Abstract GameGrid::Tile class.
	/// GameGrid::Tile::onTileTypeMessageReceived must be implemented by the
	/// HID device driver to receive messages from the remote computer.
	/// GameGrid::Tile::sendTileChangeMessage can be used to report tile change
	/// info back to the remote computer.
	/// </summary>
	struct Tile : public FirmataFeature {

		static const byte INVALID_VALUE       = INT8_MAX;
		static const byte TILE_TYPE_MESSAGE   = 0x0F;
		static const byte TILE_CHANGE_MESSAGE = 0x0E;

		enum class Type {
			WATER = 0x00,
			HIT = 0x01,
			DESTROYED = 0x02,
		};

		boolean handlePinMode(byte pin, int mode) { }
		void handleCapability(byte pin) { }
		void reset() { }

		boolean handleSysex(byte command, byte argc, byte *argv) {
			if ((command == TILE_TYPE_MESSAGE) && (argc >= 3)) {
				byte item = argv[0];
				byte row = argv[1];
				byte column = argv[2];
				onTileTypeMessageReceived(
					row, column, static_cast<Tile::Type>(item)
				);
				return true;
			}
			return false;
		}

		/// <summary>
		/// Implemented this method to receive tile type messages from the
		/// remote computer.
		/// </summary>
		virtual void onTileTypeMessageReceived(
			byte row, byte column, Tile::Type type) = 0;

		/// <summary>
		/// Report tile change messages back to the remote computer.
		/// </summary>
		static void sendTileChangeMessage(byte row, byte column) {
			Firmata.write(START_SYSEX);
			Firmata.write(TILE_CHANGE_MESSAGE);
			Firmata.write(row % MAX_ROWS);
			Firmata.write(column % MAX_COLUMNS);
			Firmata.write(END_SYSEX);
		}
	};
};

#endif // GAME_GRID_H