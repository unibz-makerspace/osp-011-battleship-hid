/*
 * 2016-07-26 Julian Sanin
 */

/*
ISDN flat band cable with RJ45 8P8C plugs: 
ORG  SCK --> SCK     [EDGE]
BRN  GND --- GND
WHT MISO <-- DOUT    [LEVL]
BLU MOSI --> SI/DIN  [LEVL]
YLW  VCC --- VCC
GRN  SS1 --> RCLK    [EDGE]
RED( GND --- GND )
BLK  SS2 --> CS/SHDN [EDGE]

3ns/m

Series      AC
Resist. --> Termination?
Zribbon = 110 - 130 (ohms)

https://forum.pjrc.com/threads/28138-Long-SPI-cables-and-100-ohm-resistors?p=68642&viewfull=1#post68642

1) commonest SPI problems are the quality (rise/fall time, noise) on SCLK and CSB. If
   you are doing software CSB control, then you likely will have plenty of time delay
   between CSB falling and the 1st SCLK transition -- that's good.

2) SCLK glitches (noise, reflections) can cause problems -- this can be worse with
   more loads on the line (modules) -- if it works with (say) 3, but not 7, does it
   work with 3, and additional capacitance (10-30 pF ?) ? -- if you don't have a
   'scope, this can help distinguish between reflections or just capacitive loading
   causing problems. If it is capacitive loading, perhaps you can just use additional
   HC125 buffers to drive subgroups of SCLK lines ?

3) Does it work when you only have small loads on the SPI peripherals (e.g. low LED
   current, small numbers of LEDs etc. ?) ? Can you confirm if the problems are
   sending or in receiving data ?

4) You could use 0.1 uF and 10 uF in parallel on the boards -- but the board may
   already have these.

5) Ensure your ground wire is as good as the SPI signal wires --this includes the
   GND to the buffers as well as to the peripherals. It might be best to 'star'
   connect these -- individual wires back to a single GND point on the SPI master.
   Connect the GND of the +5 V power supply to here also. I realize this amount of
   wire might be difficult, so maybe group them -- e.g. separate wires for all the
   left peripheral group, the right group, the HC125s etc.

6) I didn't search much, but it's better if the 3-state buffers have hysteresis 
   --maybe some versions do ? What about LS244 ?

7) try reduce the 5 V power supply to the buffers and the peripherals -- 4.5 V
   (If you don't have a variable one, a diode in series will work e.g. IN4001..7)
   may reduce noise enough to make progress and understand where the problems are
   coming from.
 */

#include <SPI.h>

enum PinConfiguration {
  PIN_SCK  = 13,
  PIN_MISO = 12,
  PIN_MOSI = 11,
  PIN_CS_LED = 10,
  PIN_CS_ADC = 9,
  SIG_CS = LOW,
};

enum SpiConfiguration {
  SPI_SCK_FREQ = 2000000, //Hz
  SPI_BIT_ORDER = MSBFIRST,
  SPI_MODE = SPI_MODE0,
};

enum LedConfiguration{
  LED_DURATION = 1000, //ms
};

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(PIN_CS_LED, OUTPUT);
  pinMode(PIN_CS_ADC, OUTPUT);
  digitalWrite(PIN_CS_LED, !SIG_CS);
  digitalWrite(PIN_CS_ADC, !SIG_CS);
}

void loop() {
  testLedLights();
  testExternalAdc();
}

void testLedLights() {
  static const SPISettings spiSettings(SPI_SCK_FREQ, SPI_BIT_ORDER, SPI_MODE);
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS_LED, SIG_CS);
  SPI.transfer(0x01); // COM
  SPI.transfer(~0x00); // ~B
  SPI.transfer(~0x00); // ~G
  SPI.transfer(~0x01); // ~R
  digitalWrite(PIN_CS_LED, !SIG_CS);
  SPI.endTransaction();
  delay(LED_DURATION);
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS_LED, SIG_CS);
  SPI.transfer(0x01); // COM
  SPI.transfer(~0x00); // ~B
  SPI.transfer(~0x01); // ~G
  SPI.transfer(~0x00); // ~R
  digitalWrite(PIN_CS_LED, !SIG_CS);
  SPI.endTransaction();
  delay(LED_DURATION);
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS_LED, SIG_CS);
  SPI.transfer(0x01); // COM
  SPI.transfer(~0x01); // ~B
  SPI.transfer(~0x00); // ~G
  SPI.transfer(~0x00); // ~R
  digitalWrite(PIN_CS_LED, !SIG_CS);
  SPI.endTransaction();
  delay(LED_DURATION);
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS_LED, SIG_CS);
  SPI.transfer(0x01); // COM
  SPI.transfer(~0x00); // ~B
  SPI.transfer(~0x01); // ~G
  SPI.transfer(~0x01); // ~R
  digitalWrite(PIN_CS_LED, !SIG_CS);
  SPI.endTransaction();
  delay(LED_DURATION);
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS_LED, SIG_CS);
  SPI.transfer(0x01); // COM
  SPI.transfer(~0x01); // ~B
  SPI.transfer(~0x01); // ~G
  SPI.transfer(~0x00); // ~R
  digitalWrite(PIN_CS_LED, !SIG_CS);
  SPI.endTransaction();
  delay(LED_DURATION);
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS_LED, SIG_CS);
  SPI.transfer(0x01); // COM
  SPI.transfer(~0x01); // ~B
  SPI.transfer(~0x00); // ~G
  SPI.transfer(~0x01); // ~R
  digitalWrite(PIN_CS_LED, !SIG_CS);
  SPI.endTransaction();
  delay(LED_DURATION);
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS_LED, SIG_CS);
  SPI.transfer(0x01); // COM
  SPI.transfer(~0x01); // ~B
  SPI.transfer(~0x01); // ~G
  SPI.transfer(~0x01); // ~R
  digitalWrite(PIN_CS_LED, !SIG_CS);
  SPI.endTransaction();
  delay(LED_DURATION);
}

void testExternalAdc() {
  static const SPISettings spiSettings(4000000, SPI_BIT_ORDER, SPI_MODE);
  static const uint8_t ADC_SINGLE_SIDED_CHANNEL[] = {
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    // [SINGLE/DIFF] [CH2] [CH1] [CH0]
  };
  for (uint8_t i = 0; i < 8; i++) {
    SPI.beginTransaction(spiSettings);
    digitalWrite(PIN_CS_ADC, SIG_CS);
    SPI.transfer(0x01);
    uint16_t data = SPI.transfer(ADC_SINGLE_SIDED_CHANNEL[i] << 4);
    data &= 0x0003;
    data <<= 8;
    data |= SPI.transfer(0x00);
    digitalWrite(PIN_CS_ADC, !SIG_CS);
    SPI.endTransaction();
    Serial.print("0x");
    Serial.print(data);
    Serial.print(' ');
  }
  Serial.println();
}
