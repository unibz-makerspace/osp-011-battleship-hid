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
