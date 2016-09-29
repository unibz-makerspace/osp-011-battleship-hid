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

#ifndef ARRANGE_GRID_H
#define ARRANGE_GRID_H

#include <Arduino.h>
#include <ConfigurableFirmata.h>
#include <stdint.h>

#include "Photoresistor.h"

template<
	typename LaserPhotoresistorArrayRow,
	typename LaserPhotoresistorArrayColumn,
	byte MAX_ROWS = 8, byte MAX_COLUMNS = 8
>
class ArrangeGrid {

	struct OnSignalEdgeListenerRow :
			public Photoresistor::OnSignalEdgeListener {
		virtual ~OnSignalEdgeListenerRow() { }
		virtual void onRaisingSignalEdge(uint8_t position) {
			sendRowChangeMessage(position);
		}
		virtual void onFallingSignalEdge(uint8_t position) { /* Not used. */ }
	};

	struct OnSignalEdgeListenerColumn :
		public Photoresistor::OnSignalEdgeListener {
		virtual ~OnSignalEdgeListenerColumn() { }
		virtual void onRaisingSignalEdge(uint8_t position) {
			sendColumnChangeMessage(position);
		}
		virtual void onFallingSignalEdge(uint8_t position) { /* Not used. */ }
	};

	static LaserPhotoresistorArrayRow laserPhotoresistorArrayRow;
	static LaserPhotoresistorArrayColumn laserPhotoresistorArrayColumn;
	static Photoresistor photoresistorRow[MAX_ROWS];
	static Photoresistor photoresistorColumn[MAX_COLUMNS];
	static OnSignalEdgeListenerRow onSignalEdgeListenerRow;
	static OnSignalEdgeListenerColumn onSignalEdgeListenerColumn;

	static const byte ROW_CHANGE_MESSAGE    = 0x0D;
	static const byte COLUMN_CHANGE_MESSAGE = 0x0C;

	/// <summary>
	/// Report row change message back to the remote computer.
	/// </summary>
	static void sendRowChangeMessage(byte position) {
		Firmata.write(START_SYSEX);
		Firmata.write(ROW_CHANGE_MESSAGE);
		Firmata.write(position % MAX_ROWS);
		Firmata.write(END_SYSEX);
	}

	/// <summary>
	/// Report column change message back to the remote computer.
	/// </summary>
	static void sendColumnChangeMessage(byte position) {
		Firmata.write(START_SYSEX);
		Firmata.write(COLUMN_CHANGE_MESSAGE);
		Firmata.write(position % MAX_COLUMNS);
		Firmata.write(END_SYSEX);
	}

public:

	static void begin() {
		laserPhotoresistorArrayRow.begin();
		laserPhotoresistorArrayColumn.begin();
		for (uint8_t i = 0; i < MAX_ROWS; i++) {
			photoresistorRow[i]
				.setOnSignalEdgeListener(&onSignalEdgeListenerRow);
		}
		for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
			photoresistorColumn[i]
				.setOnSignalEdgeListener(&onSignalEdgeListenerColumn);
		}
	}

	static void run() {
		uint8_t rowReading[MAX_ROWS];
		uint8_t columnReading[MAX_COLUMNS];
		laserPhotoresistorArrayRow.read(rowReading, MAX_ROWS);
		laserPhotoresistorArrayColumn.read(columnReading, MAX_COLUMNS);
		for (uint8_t i = 0; i < MAX_ROWS; i++) {
			photoresistorRow[i]
				.getLogicOutputWithHysteresis(
					MAX_ROWS - i -1, rowReading[i] // With reverse index order.
				);
		}
		for (uint8_t i = 0; i < MAX_COLUMNS; i++) {
			photoresistorColumn[i]
				.getLogicOutputWithHysteresis(
					i, columnReading[i]
				);
		}
	}
};

// Listeners
template<
	typename LaserPhotoresistorArrayRow,
	typename LaserPhotoresistorArrayColumn,
	byte MAX_ROWS, byte MAX_COLUMNS
>
typename ArrangeGrid<
	LaserPhotoresistorArrayRow,
	LaserPhotoresistorArrayColumn,
	MAX_ROWS, MAX_COLUMNS
>::OnSignalEdgeListenerRow ArrangeGrid<
	LaserPhotoresistorArrayRow,
	LaserPhotoresistorArrayColumn,
	MAX_ROWS, MAX_COLUMNS
>::onSignalEdgeListenerRow;

template<
	typename LaserPhotoresistorArrayRow,
	typename LaserPhotoresistorArrayColumn,
	byte MAX_ROWS, byte MAX_COLUMNS
>
typename ArrangeGrid<
	LaserPhotoresistorArrayRow,
	LaserPhotoresistorArrayColumn,
	MAX_ROWS, MAX_COLUMNS
>::OnSignalEdgeListenerColumn ArrangeGrid<
	LaserPhotoresistorArrayRow,
	LaserPhotoresistorArrayColumn,
	MAX_ROWS, MAX_COLUMNS
>::onSignalEdgeListenerColumn;

#endif // ARRANGE_GRID_H