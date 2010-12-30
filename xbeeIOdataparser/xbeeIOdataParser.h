/*
 *  IOdataParser.h
 *  xbeeLibrary

  Copyright (c) Justin Downs 12/9/08 All right reserved.
  for more info/tutorials visit
  www.johnHenrysHammer.com/WOW/
  or
  www.GRNDLAB.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#ifndef _IOPARSER
#define _IOPARSER
#include <WProgram.h>
#include <inttypes.h>
class xbeeIOdataParser {

public:

    xbeeIOdataParser();
    // get Data
    void getSensorReadings (byte* _data);                  //pass in reply data parses into below vars.
	//Return stuff
	int gethowManyAna() const {return howManyAna;}         //gives number of analog channels  ( if !0 then read with-> getanaValues())
	int gethowManyDigital() const {return howManyDigital;} //gives number of digital channels ( if !0 then read with-> getDigitals())
	const boolean* getDigitals() const {return Digitals;}  //bitfeild of digital channels D0 is lowest bit
	const long* getanaValues() const {return anaValues;}   //bitfeild of analog channels ANA0 is lowest bit

/*
2 Digital Channel Mask
âbit 0 = AD0/DIO0
âbit 1 = AD1/DIO1
âbit 2 = AD2/DIO2
âbit 3 = AD3/DIO3
âbit 4 = DIO4
âbit 5 = ASSOC/DIO5
âbit 6 = RTS/DIO6
âbit 7 = CTS/GPIO7
âbit 8 = N/A
âbit 9 = N/A
âbit 10 = RSSI/DIO10
âbit 11 = PWM/DIO11
âbit 12 = CD/DIO12

read by saying:

if (gethowManyDigital){
boolean *digitalsChannel = getDigitals();
boolean myReading = digitalsChannel[0];   //would read DIO0
}




1 Analog Channel Mask
âbit 0 = AD0/DIO0
âbit 1 = AD1/DIO1
âbit 2 = AD2/DIO2
âbit 3 = AD3/DIO3
âbit 7 = Supply Voltage

Analog samples are returned as 10-bit values.  The analog reading is scaled such that 0x0000
represents 0V, and 0x3FF = 1.2V.  (The analog inputs on the module cannot read more than
1.2V.)  Analog samples are returned in order starting with AIN0 and finishing with AIN3, and the
supply voltage.  Only enabled analog input channels return data as shown in the figure below.
To convert the A/D reading to mV, do the following:

AD(mV) = (A/D reading * 1200mV) / 1024

read by saying:

if (gethowManyAna()){
long *analogChannel  = getanaValues();
long myReading = analogChannel[0]      //would get first enabled analog channel
}


*/
private:

	//IO data
	int howManyAna;         // number of ana chanels
	int howManyDigital;     // number of Digital channels
	boolean Digitals[16];   // digital array
	long anaValues[4];      // ana array

};

#endif
