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

#ifndef RGB_LED_MATRIX_H
#define RGB_LED_MATRIX_H

#include <Arduino.h>
#include <stdint.h>

/// <summary>
/// RGB LED matrix driver for common catode LEDs. The matrix can be implemented
/// with daisy chained shift registers such as 74HC595. The first 3 registers in
/// the chain are responsible for red, green, and blue colors of each row. The
/// 4th register is responsible for selecting the current active column.
/// </summary>
template<typename SpiDevice>
class RgbLedMatrix {

	static SpiDevice spiDevice;

public:
	/// <summary>
	/// Initalize matrix.
	/// </summary>
	static void begin() {
		spiDevice.master();
	}

	/// <summary>
	/// Write the row colors to the selected column. The enabled color for each
	/// row is encoded as a bitfield.
	/// </summary>
	static void writeColumn(
			uint8_t rowReds, uint8_t rowGreens, uint8_t rowBlues,
			uint8_t column) {
		// Calculate register contents for a common cathode RGB LED matrix.
		rowReds   = ~rowReds;   // Flip bits to activate PMOS transistors.
		rowGreens = ~rowGreens; // Flip bits to activate PMOS transistors.
		rowBlues  = ~rowBlues;  // Flip bits to activate PMOS transistors.
		column = (uint8_t)(1 << column); // Activate given row NMOS transistor.
		// The order of the bytes depends on the positioning of the daisy
		// chained shift registers. In this case the first shift register is
		// responsable for the red LEDs, the 2nd for the green LEDs, and the 3rd
		// for the blue LEDs. Finally the is the last register for the selection
		// of the active column. Since the column register is the last one in
		// the chain it must be transmitted as the first byte following the
		// other bytes until it propagates through all the shift registers.
		uint8_t data[] = { column, rowBlues, rowGreens, rowReds };
		spiDevice.transferBulk(data, sizeof(data));
	}
};

#endif // RGB_LED_MATRIX_H
