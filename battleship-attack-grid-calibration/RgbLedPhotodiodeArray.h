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

#ifndef RGB_LED_PHOTODIODE_ARRAY_H
#define RGB_LED_PHOTODIODE_ARRAY_H

#include <Arduino.h>
#include <stdint.h>

/// <summary>
/// RGB LED photodiode driver. The red LED will work as a light sensor whereas
/// the green and blue LEDs can function as light emitters.
/// </summary>
template<typename SpiDevice>
class RgbLedPhotodiodeArray {

	static SpiDevice spiDevice;

	enum MCP3008Configuration {
		// Use all 8 channels @ 8-bits. Do not care for 10 bit resolution.
		MCP3008_START_BIT            = (1 << 6),
		MCP3008_SINGLE_NOT_DIFF_CONV = (1 << 5),
		MCP3008_CHANNEL_MAX          = 8,
		MCP3008_CHANNEL_LSHIFT       = 2,
		MCP3008_DUMMY_BYTE           = 0x00
	};

public:
	/// <summary>
	/// Initalize sensor and perform software reset.
	/// </summary>
	static void begin() {
		spiDevice.master();
		uint8_t dummyByte[1] = { 0x00 };
		read(dummyByte, sizeof(dummyByte));
	}

	static void mcp3008Config(uint8_t channel, uint8_t data[2]) {
		const uint8_t confByte = 0x60 | (channel << 2);
		data[0] = confByte;
		data[1] = 0x00;
	}

	/// <summary>
	/// Reads the red LEDs as photodiodes.
	/// </summary>
	/// <param name="diodes">
	/// Array of data bytes to be read. Its content will be overwritten by the
	/// sensed values.
	/// </param>
	/// <param name="length">
	/// The length of the array.
	/// </param>
	/// <returns>
	/// The actual number of read photodiodes.
	/// </returns>
	static uint8_t read(uint8_t * /*[out]*/ diodes, uint8_t length) {
		for (uint8_t i = 0; i < MCP3008_CHANNEL_MAX; i++) {
			static uint8_t data[2];
			mcp3008Config(i, data);
			spiDevice.transferBulk(data, sizeof(data));
			diodes[i] = data[1];
		}
		MCP3008_CHANNEL_MAX;
	}
	/*
	static uint8_t read(uint8_t * diodes, uint8_t length) {
		const uint8_t MAX_ITEMS = length % (MCP3008_CHANNEL_MAX + 1);
		for (uint8_t i = 0; i < MAX_ITEMS; i++) {
			const uint8_t MCP3008_CONFIG_BYTE =
				MCP3008_START_BIT |
				MCP3008_SINGLE_NOT_DIFF_CONV |
				(i << MCP3008_CHANNEL_LSHIFT);
			uint8_t buffer[] = { MCP3008_CONFIG_BYTE, MCP3008_DUMMY_BYTE };
			spiDevice.transferBulk(buffer, sizeof(buffer));
			diodes[i] = buffer[1];
		}
		return MAX_ITEMS;
	}*/
};

#endif // RGB_LED_PHOTODIODE_ARRAY_H
