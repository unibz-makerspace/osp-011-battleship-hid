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

#ifndef SPI_DEVICE_PORT_B_H
#define SPI_DEVICE_PORT_B_H

#include <Arduino.h>
#include <stdint.h>

#include <SpiDevice.h>

/// <summary>
/// SPI driver with faster slave select pin access for Arduino Uno.
/// </summary>
template<
	uint8_t PORTB_PIN,
	uint32_t F_SCK = 4000000/*Hz*/,
	SpiBitOrder BIT_ORDER = SpiBitOrderMsbFirst,
	SpiMode MODE = SpiMode0
>
struct SpiDevicePortB {

	/// <summary>
	/// Initalize the SPI port as bus master.
	/// </summary>
	static void master(void) {
		PORTB |= (1 << PORTB_PIN);
		DDRB |= (1 << PORTB_PIN);
		SPI.begin();
	}

	/// <summary>
	/// Transfer bytes on the SPI bus.
	/// </summary>
	/// <param name="data">
	/// Array of data bytes to be transfered. The content will be sent in order
	/// of the array. Its content will be overwritten by the received bytes.
	/// </param>
	/// <param name="length">
	/// The length of the array.
	/// </param>
	static void transferBulk(uint8_t * /*[in,out]*/ data, uint8_t length) {
		SPI.beginTransaction(SPISettings(F_SCK, BIT_ORDER, MODE));
		PORTB &= ~(1 << PORTB_PIN);
		for (uint8_t i = 0; i < length; i++) {
			data[i] = SPI.transfer(data[i]);
		}
		PORTB |= (1 << PORTB_PIN);
		SPI.endTransaction();
	}
};

#endif // SPI_DEVICE_PORT_B_H