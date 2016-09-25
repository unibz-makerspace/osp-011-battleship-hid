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

#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include <Arduino.h>
#include <stdint.h>

class Photoresistor {

public:
	struct OnSignalEdgeListener {
		virtual ~OnSignalEdgeListener() { }
		virtual void onRaisingSignalEdge(uint8_t position) = 0;
		virtual void onFallingSignalEdge(uint8_t position) = 0;
	};

private:
	uint8_t negativeTreshold;
	uint8_t positiveTreshold;
	uint8_t treshold;
	const float hysteresis;
	bool logicLevel;
	OnSignalEdgeListener * onSignalEdgeListener;

public:

	Photoresistor() : Photoresistor(0, UINT8_MAX) { }

	Photoresistor(uint8_t min, uint8_t max) : Photoresistor(min, max, 0.1f) { }

	Photoresistor(uint8_t min, uint8_t max,	float hysteresisPercentage) :
			hysteresis(hysteresisPercentage) {
		setTreshold(min, max);
		logicLevel = LOW;
		onSignalEdgeListener = nullptr;
	}

	void setTreshold(uint8_t min, uint8_t max) {
		const uint8_t difference = max - min;
		treshold = (difference / 2) + min;
		negativeTreshold = treshold - (difference * hysteresis);
		positiveTreshold = treshold + (difference * hysteresis);
	}

	void setOnSignalEdgeListener(OnSignalEdgeListener * onSignalEdgeListener) {
		this->onSignalEdgeListener = onSignalEdgeListener;
	}

	/// <summary>
	/// Report the logic level of the laser photoresistor beam. I.e. if the beam
	/// is interrupted the level is high or if not interrupted the level is low.
	/// The model used represents a crude non inverting comparator with
	/// hysteresis to compensate signal noise. It also invokes the
	/// OnSignalEdgeListener.
	/// </summary>
	bool getLogicOutputWithHysteresis(uint8_t position, uint8_t newReading) {
		if ((logicLevel == LOW) && (newReading > positiveTreshold)) {
			logicLevel = HIGH;
			if (onSignalEdgeListener != nullptr) {
				onSignalEdgeListener->onRaisingSignalEdge(position);
			}
		}
		if ((logicLevel == HIGH) && (newReading < negativeTreshold)) {
			logicLevel = LOW;
			if (onSignalEdgeListener != nullptr) {
				onSignalEdgeListener->onFallingSignalEdge(position);
			}
		}
		return logicLevel;
	}

};

#endif
