/*
 *  IOdataParser.cpp
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

#include "xbeeIOdataParser.h"

//constructer
xbeeIOdataParser::xbeeIOdataParser(){
for (int i=0;i<4; i++){anaValues[i]=0;}             // clear values 4 ana values max
for (int i=0;i<16; i++){Digitals[i]=0;}             // 16 digital max
}

void xbeeIOdataParser::getSensorReadings (byte* _data){

  // get num of digital io lines
  byte temp;
  byte mask= 0x01;                                  // 00000001 is on
  howManyDigital = 0;                                // num of io channels
  howManyAna=0;                                     // num of ana channels
  //GET NUMBER OF DIGITAL CHANNELS
  for (int i = 1; i<3; i++){                        // looks at the first two bytes (as bit fields) for which digital I/Os are activated
    temp = _data[i];
    for (int j =0; j <8; j++){
      howManyDigital += temp && mask;                // bit field: shift one, if it is there = 1, add it up
      temp = (temp >> 1);
    }
  }

  //Get NUMBER OF ANALOGUE CHANNELS
  howManyAna = 0;                                   // num of io channels
  temp = _data[3];                                  // analogue byte
  for (int i = 0; i < 8; i++){
    howManyAna += (temp & mask);
    temp = (temp >> 1);
  }

  //GET DIGITAL CHANNELS
  if (howManyDigital !=0){
    for (int i = 0; i<2; i++){                      // GO for BYTES 4 and 5
      temp = _data[5-i];                            // It sends high bit first. So for array to be sequential, reverse data bit order.

      for (int j = 8*i; j< 8*(i+1); j++){           // look at each bit in the bytes and them and store
        Digitals[j]= temp & mask;
        temp = (temp >> 1);
      }
    }
  }



  //look at ANA
  unsigned long inTemp = 0;

  if(howManyAna !=0 && howManyDigital !=0){         // if there was digital bytes add them in
    for (int i =0; i< howManyAna; i++){
      inTemp = 0;
	  inTemp = _data[6+(i*2)]<< 8;                  //GET ANA BYTES AND ADD TOGETHER high byte first shift it
      inTemp += (_data[6+(i*2)+1]);
      anaValues[i] = long((inTemp * 1200)/1024) ;   //(A/D reading * 1200mV) / 1024 it only goes to 1.2v GIVES YOU mv
	}
  }
  else if(howManyAna !=0){
    for (int i =0; i< howManyAna; i++){             // else leave them out
      inTemp = 0;
	  inTemp = (_data[4+(i*2)]<< 8);                //NO DIGITAL BYTES high byte first shift it
      inTemp += _data[4+(i*2)+1];
      anaValues[i]= long((inTemp* 1200)/1024);      //(A/D reading * 1200mV) / 1024 it only goes to 1.2v GIVES YOU mv
		}
  }



}



