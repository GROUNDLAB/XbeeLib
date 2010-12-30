/*
*  xbeeLib.cpp
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

#include "xbeeLib.h"
//#define XBEEDEBUG


//constructor
xbeeST::xbeeST(){

packetLength = 0;
highAddress = 0;
lowAddress = 0;

for (int i=0;i<4; i++){anaValues[i]=0;}                         // clear values 4 ana values max
for (int i=0;i<16; i++){Digitals[i]=0;}                         // 16 digital max
for (int i=0;i<2; i++){ATcommand[i]='*';}                       // 2 AT max


}

//pass in array
boolean xbeeST::getData(byte * _receivedData,int _timeOut) {    // call when there is serial data
unsigned int timeout = _timeOut;                                // bail out time
long startTime;


byte inByte=0x00;
startTime = millis();

//***************
//LOOKING FOR START BYTE AND PACKET LENGTH
while (inByte != 0x7E) {                                        // clears out the buffer of junk
  if(Serial.available() < 1) return 0;                          // no more and didn't find it
  inByte = Serial.read();                                       // read a byte from the buffer
  if((millis() - startTime) > timeout)return 0;                 // timed out bad message
}


//***************
//look for start byte 0x7E(~)
if (inByte == 0x7E) {              // if we got the API start byte
startTime = millis();
while (Serial.available() < 2){ // block for length byte two bytes to be available
  if((millis() - startTime) > timeout) return 0;// timed out bad message
}

if (Serial.available() > 1){     // if we got 2 bytes
  int lengthMSB = Serial.read(); // read the most significant length byte
  int lengthLSB = Serial.read(); // read the least significant length byte
  packetLength = (lengthMSB << 8) + lengthLSB; // put the two bytes together
}
}

//***************
// if we have a packet continue get APIDENTIFIER
if (packetLength > 0) {
apiIdentifier = 'Z';                  // set apiIdentifier to an impossible value
startTime = millis();
while (Serial.available() < 1){       // wait for a byte or timeout
  if((millis() - startTime) > timeout) return 0;// timed out bad message
}

if (Serial.available() > 0) {         // if a byte is waiting in the buffer set it to the API
  apiIdentifier = byte(Serial.read());
} else return 0; //else bad message


//***************
//GET LONG ADDRESS
long addrByte=0;
highAddress=0; // clear out address
lowAddress=0;
startTime = millis();
while(Serial.available() < 8){          // stop for bytes
  if((millis() - startTime) > timeout) return 0;  // timed out bad message
}
if (Serial.available() > 7) {
  byte address[8]; // array for address
  for (int i = 0; i< 8; i++){
	addrByte = byte(Serial.read());
	address[i] = addrByte;

	//global last send
	if (i < 4){  // get first half of address
	highAddress += (addrByte << ((3-i) * 8)); // get the first 4 bytes shift into place 4321
	}else {lowAddress += (addrByte << ((7-i) * 8)); // get second shift them int place 4321
		}
  }

}else  return 0;// timed out bad message



//***************
//GET LOCAL ADDRESS
startTime = millis();
while (Serial.available() < 2); // wait for at least two bytes to be available

if((millis() - startTime) > timeout) return 0;// timed out bad message

int localAd = Serial.read(); // read the most significant length byte
int localAd2 = Serial.read(); // read the least significant length byte
localAddress = (localAd << 8) + localAd2; // put the two bytes together



//***************
//GET OPTION
startTime = millis();
while (Serial.available() < 1){
if((millis() - startTime) > timeout) return 0;// timed out bad message

}
  options = byte(Serial.read()); // get the options 1 packet ack returned 2 brodcast



//***************
//GET DATA
 startTime = millis();
 dataLength = (packetLength - 12);						//record the lenth of incoming data
 while (Serial.available() < (packetLength - 12)){
 if((millis() - startTime) > timeout) return 0;
 }

for (int i= 0; i < (packetLength - 12); i++) {			// take out other info in packet + checksum
 _receivedData [i] = byte(Serial.read());				// get a byte of data
 }


//***************
//GET CHECKSUM
startTime = millis();
while (Serial.available() < 1){
if((millis() - startTime) > timeout) return 0;// timed out bad message
}
checksum = byte(Serial.read());
}

return 1;                                // if everything went well return a true
}

//gets data puts it in Malloc array
boolean xbeeST::getDataAuto(int _timeOut) {// call when there is serial data
unsigned int timeout= _timeOut;// bail out time
long startTime;


byte inByte=0x00;
startTime = millis();
//***************

//LOOKING FOR START BYTE AND PACKET LENGTH
while (inByte != 0x7E) { // clears out the buffer of junk
  if(Serial.available() < 1) return 0;       // no more and didn't find it
  inByte = Serial.read();                          // read a byte from the buffer
  if((millis() - startTime) > timeout)return 0; // timed out bad message
}


//***************
//look for start byte 0x7E(~)
if (inByte == 0x7E) {              // if we got the API start byte
startTime = millis();
while (Serial.available() < 2){ // block for length byte two bytes to be available
  if((millis() - startTime) > timeout)return 0; // timed out bad message
}
if (Serial.available() > 1){     // if we got 2 bytes
  int lengthMSB = Serial.read(); // read the most significant length byte
  int lengthLSB = Serial.read(); // read the least significant length byte
  packetLength = (lengthMSB << 8) + lengthLSB; // put the two bytes together
}
}

//***************
// APIDENTIFIER
if (packetLength > 0) {
apiIdentifier = 'Z';                  // set apiIdentifier to an impossible value
startTime = millis();
while (Serial.available() < 1){       // wait for a byte or timeout
  if((millis() - startTime) > timeout) return 0;// timed out bad message
}
if (Serial.available() > 0) {         // if a byte is waiting in the buffer set it to the API
  apiIdentifier = byte(Serial.read());
} else return 0; //else bad message


//***************
//GET LONG ADDRESS
long addrByte=0;
highAddress=0; // clear out address
lowAddress=0;
startTime = millis();
while(Serial.available() < 8){          // stop for bytes
  if((millis() - startTime) > timeout) return 0;    // timed out bad message

}
if (Serial.available() > 7) {
  byte address[8]; // array for address
  for (int i = 0; i< 8; i++){
	addrByte = byte(Serial.read());
	address[i] = addrByte;

	//global last send
	if (i < 4){  // get first half of address
	highAddress += (addrByte << ((3-i) * 8)); // get the first 4 bytes shift into place 4321
	}else {lowAddress += (addrByte << ((7-i) * 8)); // get second shift them int place 4321
		}
  }

}else return 0;// timed out bad message


//***************
//GET LOCAL ADDRESS
startTime = millis();
while (Serial.available() < 2); // wait for at least two bytes to be available

if((millis() - startTime) > timeout) return 0;// timed out bad message

int localAd = Serial.read(); // read the most significant length byte
int localAd2 = Serial.read(); // read the least significant length byte
localAddress = (localAd << 8) + localAd2; // put the two bytes together


//***************
//GET OPTION
startTime = millis();
while (Serial.available() < 1){
if((millis() - startTime) > timeout) return 0;// timed out bad message
}
  options = byte(Serial.read()); // get the options 1 packet ack returned 2 brodcast



//***************
//GET DATA
 startTime = millis();
 dataLength = (packetLength - 12);						//record the lenth of incoming data
 while (Serial.available() < (packetLength - 12)){
 if((millis() - startTime) > timeout) return 0;
 }

 static boolean goneOnce = false;						// flag to not free the first time
 if (goneOnce)free(receivedData);						// if it has been allocated Free the memory
 else goneOnce = true;
  receivedData = (byte*) malloc(packetLength - 12);     // get memory, be sure to call free in sendData or it blows real quick
  if (receivedData == NULL) return 0;	                // if we couldn't get memory

 for (int i= 0; i < (packetLength - 12); i++) {			// take out other info in packet + checksum
 receivedData [i] = byte(Serial.read());				// get a byte of data
 }


//***************
//GET CHECKSUM
startTime = millis();
while (Serial.available() < 1){
if((millis() - startTime) > timeout) return 0;// timed out bad message
}
checksum = byte(Serial.read());
}

return 1;                                // if everything went well return a true
}

// function that puts together a data packet and transmits it, using 64-bit addressing
boolean xbeeST::sendData(byte* data, int _dataLength,long* _destination,int _localAddress) {
int dataLength = _dataLength;
//int dataLength = sizeof(data);
//strlen dosen't work with num conversion because of null use only with letter strings
// dataLength =(strlen(data)); // length of data when using CHAR ARRAYS ONLY
static byte frameID = 0xA1;
frameID++; // add one to the frame ID each time, okay to overflow
if (frameID == 0) frameID = 1; //skip zero because this disables status response packets
byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
// calculate length of the packet
// API identifier + frameId + 8bytes 64-bit addr
// + 2 bytes 16-bit addr + broadcast radius
// + options + ***TAGBYTE***
int length = (dataLength + 14);  // last one is tagByte
// send the packet
Serial.print(0x7E, BYTE); // send start delimiter
Serial.print(0x0, BYTE);  // send length MSB (always zero because packets can't be more than 100 bytes)
Serial.print(length, BYTE); // send length LSB
Serial.print(0x10, BYTE); // send API command identifier
checksum = checksum - 0x10;  // minus from checksum FF
Serial.print(frameID, BYTE); // send frame ID (set to 0 if noresponse is required)
checksum = checksum - frameID;

// set  the 64 bit factory address long high byte then low
for(int i=0;i < 2; i++){ // address never bigger than 2
for (int j = 4; j > 0; j--) {
  byte destinationByte = _destination[i] >> 8 * (j-1); // shift over one byte at a time, MSB first 4 for long
  Serial.print(destinationByte, BYTE);  // send destination address
  checksum = checksum - destinationByte;
}
}


// break up local address and send it
byte localLow = _localAddress;
byte localHigh = _localAddress >> 8;
//send 16-bit address as unkown FUNCTION
Serial.print(localHigh,BYTE);   //send unknown for 16-bit address FF
checksum = checksum-localHigh;
Serial.print(localLow,BYTE);   //send unknown for 16-bit address  FE
checksum = checksum - localLow;
Serial.print(0x0,BYTE); //send broadcast radius
checksum = checksum - 0x0;
Serial.print(0x0,BYTE); //send option byte about multicasting
checksum = checksum - 0x0;
//////******start data *****\\\\\\\\\\

//send all data in array
byte thisbyte;
for (int i = 0; i < dataLength ; i++){
 thisbyte = data[i];
Serial.print(thisbyte,BYTE);
checksum = checksum - thisbyte;
}


Serial.print(checksum,BYTE); //  send checksum
return true;

}


boolean xbeeST::sendLocalAtCommandWithOptions (char* _ATcommand, char* _AToptions,int _optionsize) {

  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++; // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1; //skip zero because this disables status response packets

  int CommandLength = (strlen(_ATcommand)); // length of ATcommand
  int AToptionLength =  _optionsize; // if there are options count them
  CommandLength += AToptionLength;

  //******************************
  //calculate length of the packet
  //  Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  // (API identifier + frameId) = 2 "plus +" (ATcommand + command options) = CommandLength
  int messageLength = (CommandLength + 2);

  // A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //  To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial.print(0x7E, BYTE);             // send start delimiter
  Serial.print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial.print(messageLength, BYTE);    // send length LSB
  Serial.print(0x08, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x08;           // minus command identifier from checksum FF
  Serial.print(frameID, BYTE);          // send frame ID (set to 0 if noresponse is required)
  checksum = checksum - frameID;

  //******************************
  //command options
  Serial.print(_ATcommand[0], BYTE); // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial.print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  //******************************
  //send options
  if(AToptionLength != 0){
    for (int i = 0; i < AToptionLength; i++){
      Serial.print(_AToptions[i] ,BYTE);
      checksum = checksum - _AToptions[i];
    }
  }

  Serial.print(checksum,BYTE);       //  send checksum
  return true;
}


boolean xbeeST::sendLocalAtCommand(char* _ATcommand) {
  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++;                                   // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1;               //skip zero because this disables status response packets

  int CommandLength = (strlen(_ATcommand));     // length of ATcommand

  //******************************
  //calculate length of the packet
  //  Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  // (API identifier + frameId) = 2 "plus +" (ATcommand) = CommandLength
  int messageLength = (CommandLength + 2);

  // A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //  To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial.print(0x7E, BYTE);             // send start delimiter
  Serial.print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial.print(messageLength, BYTE);    // send length LSB
  Serial.print(0x08, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x08;           // minus command identifier from checksum FF
  Serial.print(frameID, BYTE);          // send frame ID (set to 0 if noresponse is required)
  checksum = checksum - frameID;

  //******************************
  //AT command
  Serial.print(_ATcommand[0], BYTE); // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial.print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  Serial.print(checksum,BYTE);       //  send checksum
  return true;
}


boolean xbeeST::getLocalATCommandData(int _timeOut){

  unsigned int timeout = _timeOut;// bail out time
  long startTime;

  byte inByte=0x00;
  startTime = millis();
  //***************
//LOOKING FOR START BYTE AND PACKET LENGTH
while (inByte != 0x7E) { // clears out the buffer of junk
  if(Serial.available() < 1) return 0;       // no more and didn't find it
  inByte = Serial.read();                          // read a byte from the buffer
  if((millis() - startTime) > timeout)return 0; // timed out bad message
}

  //***************
  //look for start byte 0x7E(~)
  if (inByte == 0x7E) {              // if we got the API start byte
    startTime = millis();
    while (Serial.available() < 2){ // block for length byte two bytes to be available
      if((millis() - startTime) > timeout)return 0;// timed out bad message
	}
    if (Serial.available() > 1){     // if we got 2 bytes
      int lengthMSB = Serial.read(); // read the most significant length byte
      int lengthLSB = Serial.read(); // read the least significant length byte
      packetLength = (lengthMSB << 8) + lengthLSB; // put the two bytes together
    }
  }

  //***************
  //  APIDENTIFIER
  if (packetLength > 0) {               //IF WE HAVE A FULL PACKET
    apiIdentifier = 'Z';                  // set apiIdentifier to an impossible value
    startTime = millis();
    while (Serial.available() < 1){       // wait for a byte or timeout
      if((millis() - startTime) > timeout) return 0; // timed out bad message
    }

    if (Serial.available() > 0) {         // if a byte is waiting in the buffer set it to the API
      apiIdentifier = byte(Serial.read());
    }
    else return 0; //else bad message


    //***************
    //FRAME ID
    startTime = millis();
    while (Serial.available() < 1){       // wait for a byte or timeout
      if((millis() - startTime) > timeout)return 0;// timed out bad message
    }
    if (Serial.available() > 0) {         // if a byte is waiting in the buffer set it to the API
      frameID = byte(Serial.read());
    }
    else return 0; //else bad message


    //***************
    //GET AT COMMAND BYTE
    startTime = millis();
    char ATcommandByte = '?'; //what is it?
    while( Serial.available() < 3 ){ // stop for bits
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }
    if (Serial.available() > 1) { // if a byte is waiting in the buffer
      for (int i = 0; i<2; i++){ // two command bytes
        ATcommandByte =byte(Serial.read());
        ATcommand[i]=ATcommandByte;
      }
    }
    else return 0; // timed out bad message


    //***************
    //GET STATUS/options
    startTime = millis();
    while (Serial.available() < 1){
      if((millis() - startTime) > timeout) return 0;// timed out bad message

    }
    status = byte(Serial.read()); // get the options/status 0 = OK 1 = Error 2 = Invalid Command 3 = Invalid Parameter


    //***************
    //GET DATA
    startTime = millis();
    dataLength = (packetLength - 5);						//record the lenth of incoming data
    while (Serial.available() < (packetLength - 5)){
      if((millis() - startTime) > timeout) return 0;
    }
    static boolean goneOnce = false;						// flag to not free the first time
    if (goneOnce)free(receivedData);						// if it has been allocated Free the memory
    else goneOnce = true;
    receivedData = (byte*) malloc(packetLength - 5);                           // get memory, be sure to call free in sendData or it blows real quick
    if (receivedData == NULL){                                                  // if we couldn't get memory

      return 0; // couldnt allocate
    }

    for (int i= 0; i < (packetLength - 5); i++) {			// take out other info in packet + checksu

      receivedData [i] = byte(Serial.read());				// get a byte of data
    }

    //***************
    //GET CHECKSUM
    startTime = millis();
    while (Serial.available() < 1){
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }
    checksum = byte(Serial.read());


    //***************
    //PRINTALL
#ifdef XBEEDEBUG
    Serial.println("packetLength");
    Serial.println(packetLength,HEX);
    Serial.println("apiIdentifier");
    Serial.println(apiIdentifier,HEX);
    Serial.println("frameID");
    Serial.println(frameID,HEX);
    Serial.println("Atcommand");
    for (int i= 0; i < 2; i++) { // take out other info in packet + checksum
      Serial.println(ATcommand[i],HEX);
    }
    Serial.println("status");
    Serial.println(status,HEX);
    Serial.println("info");
    for (int i= 0; i < (packetLength - 5); i++) { // take out other info in packet + checksum
      Serial.println(receivedData[i],HEX);
    }
    Serial.println("checksum  ");
    Serial.println(checksum,HEX);
    Serial.println("END");
#endif

    return 1;                                // if everything went well return a true
  }
  return 0;                               // somthing went wrong
}


boolean xbeeST::sendAtCommandWithOptions (long* _destination, char* _ATcommand, char* _AToptions,int _optionsize, int _localAddress ) {

  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++; // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1; //skip zero because this disables status response packets

  int CommandLength = strlen(_ATcommand); // length of ATcommand
  int AToptionLength = _optionsize; // if there are options count them

  //int AToptionLength = int(sizeof(_AToptions)); // if there are options count them
  CommandLength += AToptionLength;

  //******************************
  //calculate length of the packet
  //  Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  // (API identifier + frameId) = 2 "plus +" (ATcommand + command options) = CommandLength
  int messageLength = (CommandLength + 13);

  // A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //  To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial.print(0x7E, BYTE);             // send start delimiter
  Serial.print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial.print(messageLength, BYTE);    // send length LSB
  Serial.print(0x17, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x17;           // minus command identifier from checksum FF
  Serial.print(frameID, BYTE);          // send frame ID (set to 0 if noresponse is required)
  checksum = checksum - frameID;

  //******************************
  //SEND hardware Adress
  // set  the 64 bit factory address long high byte then low
  for(int i=0;i < 2; i++){                                 // address never bigger than 2
    for (int j = 4; j > 0; j--) {
      byte destinationByte = _destination[i] >> 8 * (j-1); // shift over one byte at a time, MSB first 4 for long
      Serial.print(destinationByte, BYTE);                 // send destination address
      checksum = checksum - destinationByte;
    }
  }

  //******************************
  // break up local address and send it
  byte localLow = _localAddress;
  byte localHigh = _localAddress >> 8;
  //send 16-bit address as unkown FUNCTION
  Serial.print(localHigh,BYTE);  //send unknown for 16-bit address FF
  checksum = checksum - localHigh;
  Serial.print(localLow,BYTE);   //send unknown for 16-bit address  FE
  checksum = checksum - localLow;
  //******************************
  // sets when to run command
  Serial.print(0x02,BYTE);   //send 0x02 to run command when recieved. IF not set the AT command "AC" must be sent to set and run.
  checksum = checksum - 0x02;
  //******************************
  //command options
  Serial.print(_ATcommand[0], BYTE); // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial.print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  //******************************
  //send options
  if(AToptionLength != 0){
    for (int i = 0; i < AToptionLength; i++){
      Serial.print(_AToptions[i] ,BYTE);
      checksum = checksum - _AToptions[i];

    }
  }

  Serial.print(checksum,BYTE);       //  send checksum
  return true;
}


boolean xbeeST::sendAtCommand(long* _destination, char* _ATcommand, int _localAddress ) {

  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++; // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1; //skip zero because this disables status response packets

  int CommandLength = strlen(_ATcommand); // length of ATcommand

  //******************************
  //calculate length of the packet
  //  Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  // (API identifier + frameId) = 2 "plus +" (ATcommand + command options) = CommandLength
  int messageLength = (CommandLength + 13);

  // A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //  To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial.print(0x7E, BYTE);             // send start delimiter
  Serial.print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial.print(messageLength, BYTE);    // send length LSB
  Serial.print(0x17, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x17;           // minus command identifier from checksum FF
  Serial.print(frameID, BYTE);          // send frame ID (set to 0 if noresponse is required)
  checksum = checksum - frameID;

  //******************************
  //SEND hardware Adress
  // set  the 64 bit factory address long high byte then low
  for(int i=0;i < 2; i++){                                 // address never bigger than 2
    for (int j = 4; j > 0; j--) {
      byte destinationByte = _destination[i] >> 8 * (j-1); // shift over one byte at a time, MSB first 4 for long
      Serial.print(destinationByte, BYTE);                 // send destination address
      checksum = checksum - destinationByte;
    }
  }

  //******************************
  // break up local address and send it
  byte localLow = _localAddress;
  byte localHigh = _localAddress >> 8;
  //send 16-bit address as unkown FUNCTION
  Serial.print(localHigh,BYTE);  //send unknown for 16-bit address FF
  checksum = checksum - localHigh;
  Serial.print(localLow,BYTE);   //send unknown for 16-bit address  FE
  checksum = checksum - localLow;
  //******************************
  // sets when to run command
  Serial.print(0x02,BYTE);   //send 0x02 to run command when recieved. IF not set the AT command "AC" must be sent to set and run.
  checksum = checksum - 0x02;
  //******************************
  //command options
  Serial.print(_ATcommand[0], BYTE); // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial.print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  //******************************
  Serial.print(checksum,BYTE);       //  send checksum
  return true;

}


boolean xbeeST::getATCommandData(int _timeOut){

  unsigned int timeout = _timeOut;// bail out time
  long startTime;

  byte inByte=0x00;
  startTime = millis();
  //***************
  //LOOKING FOR START BYTE AND PACKET LENGTH
  while (inByte != 0x7E) { // clears out the buffer of junk
     if(Serial.available() < 1) return 0;       // no more and didn't find it
	inByte = Serial.read();                            // read byte from the buffer
#ifdef XBEEDEBUG									   // Debug
    Serial.println(inByte, HEX);
#endif

    if((millis() - startTime) > timeout){             //time out for program flow
#ifdef XBEEDEBUG
      Serial.println((millis() - startTime));
      Serial.println("inbyte fucked");
#endif
      return 0; // timed out bad message
    }



  }
  //***************
  //look for start byte 0x7E(~)
  if (inByte == 0x7E) {              // if we got the API start byte
    startTime = millis();
    while (Serial.available() < 2){ // block for length byte two bytes to be available
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }
    if (Serial.available() > 1){     // if we got 2 bytes
      int lengthMSB = Serial.read(); // read the most significant length byte
      int lengthLSB = Serial.read(); // read the least significant length byte
      packetLength = (lengthMSB << 8) + lengthLSB; // put the two bytes together
    }
  }


  //***************
  //  APIDENTIFIER
  if (packetLength > 0) {               //IF WE HAVE A FULL PACKET
    apiIdentifier = 'Z';                  // set apiIdentifier to an impossible value
    startTime = millis();
    while (Serial.available() < 1){       // wait for a byte or timeout
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }

    if (Serial.available() > 0) {         // if a byte is waiting in the buffer set it to the API
      apiIdentifier = byte(Serial.read());
    }
    else return 0; //else bad message



    //***************
    //FRAME ID
    startTime = millis();
    while (Serial.available() < 1){       // wait for a byte or timeout
      if((millis() - startTime) > timeout) return 0;// timed out bad message

    }

    if (Serial.available() > 0) {         // if a byte is waiting in the buffer set it to the API
      frameID = byte(Serial.read());

    }
    else return 0; //else bad message




    //***************
    //GET LONG ADDRESS
    long addrByte = 0;
    highAddress = 0; // clear out address
    lowAddress = 0;
    startTime = millis();
    while(Serial.available() < 8){          // stop for bytes
      if((millis() - startTime) > timeout) return 0;  // timed out bad message
    }
    if (Serial.available() > 7) {
      byte address[8]; // array for address
      for (int i = 0; i< 8; i++){
        addrByte = byte(Serial.read());
        address[i] = addrByte;

        if (i < 4){  // get first half of address
          highAddress += (addrByte << ((3-i) * 8)); // get the first 4 bytes shift into place 4321
        }
        else {
          lowAddress += (addrByte << ((7-i) * 8)); // get second shift them int place 4321
        }

      }

    }
    else return 0;// timed out bad message




    //***************
    //GET LOCAL ADDRESS
    startTime = millis();
    while (Serial.available() < 2); // wait for at least two bytes to be available

    if((millis() - startTime) > timeout) return 0;// timed out bad message


    int localAd = Serial.read(); // read the most significant length byte
    int localAd2 = Serial.read(); // read the least significant length byte
    localAddress = (localAd << 8) + localAd2; // put the two bytes together



    //***************
    //GET AT COMMAND BYTE
    startTime = millis();
    char ATcommandByte = '?'; //what is it?
    while( Serial.available() < 3 ){ // stop for bits
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }

    if (Serial.available() > 1) { // if a byte is waiting in the buffer
      for (int i = 0; i<2; i++){ // two command bytes
        ATcommandByte =byte(Serial.read());
        ATcommand[i]=ATcommandByte;
      }
    }
    else return 0; // timed out bad message


    //***************
    //GET STATUS/options
    startTime = millis();
    while (Serial.available() < 1){
      if((millis() - startTime) > timeout) return 0;// timed out bad message

    }
    status = byte(Serial.read()); // get the options/status 0 = OK 1 = Error 2 = Invalid Command 3 = Invalid Parameter


    //***************
    //GET DATA
    startTime = millis();
    dataLength = (packetLength - 15);						//record the lenth of incoming data
    while (Serial.available() < (packetLength - 15)){
      if((millis() - startTime) > timeout) return 0;
    }

    static boolean goneOnce = false;						// flag to not free the first time
    if (goneOnce)free(receivedData);						// if it has been allocated Free the memory
    else goneOnce = true;
    receivedData = (byte*) malloc(packetLength - 15);       // get memory, be sure to call free in sendData or it blows real quick
    if (receivedData == NULL) return 0;                     // if we couldn't get memory

    for (int i= 0; i < (packetLength - 15); i++) {			// take out other info in packet + checksu
      receivedData [i] = byte(Serial.read());				// get a byte of data
    }


    //***************
    //GET CHECKSUM
    startTime = millis();
    while (Serial.available() < 1){
      if((millis() - startTime) > timeout) return 0;// timed out bad message
    }
    checksum = byte(Serial.read());



    //***************
    //PRINTALL
#ifdef XBEEDEBUG
    Serial.println("packetLength");
    Serial.println(packetLength,HEX);
    Serial.println("apiIdentifier");
    Serial.println(apiIdentifier,HEX);
    Serial.println("frameID");
    Serial.println(frameID,HEX);
    Serial.println("localAddress");
    Serial.println(localAddress,HEX);
    Serial.println("Address");
    Serial.println(highAddress,HEX);
    Serial.println(lowAddress,HEX);
    Serial.println("Atcommand");
    for (int i= 0; i < 2; i++) { // take out other info in packet + checksum
      Serial.println(ATcommand[i],HEX);
    }
    Serial.println("status");
    Serial.println(status,HEX);
    Serial.println("info");
    for (int i= 0; i < (packetLength - 15); i++) { // take out other info in packet + checksum
      Serial.println(receivedData[i],HEX);
    }
    Serial.println("checksum  ");
    Serial.println(checksum,HEX);
    Serial.println("END");
#endif

    return 1;                                // if everything went well return a true
  }
  return 0;                               // somthing went wrong
}


void xbeeST::getSensorReadings (byte* _data){

  // _data[0]; // # of Sample Sets, always set to 1
  // get num of digital io lines
  byte temp;
  byte mask= 0x01; // 00000001 is on
  howManyDigital = 0;//num of io channels
  howManyAna=0; //num of ana channels
  //GET NUMBER OF DIGITAL CHANNELS
  for (int i = 1; i<3; i++){   // looks at the first two bytes (as bit fields) for which digital I/Os are activated
    temp =_data[i];
    for (int j =0; j <8; j++){
      howManyDigital += temp && mask;  // bit field: shift one, if it is there = 1, add it up
      temp = (temp >> 1);
    }
  }

  //Get NUMBER OF ANALOUGE CHANNELS
  howManyAna = 0;//num of io channels
  temp = _data[3]; //analouge byte
  for (int i = 0; i < 8; i++){
    howManyAna += temp & mask;
    temp = (temp >> 1);
  }

  //GET DIGITAL CHANNELS
  if (howManyDigital !=0){
    for (int i = 0; i<2; i++){ // GO for BYTES 4 and 5
      temp = _data[5-i];       // It sends high bit first. So for array to be sequential, reverse data bit order.

      for (int j = 8*i; j< 8*(i+1); j++){ //look at each bit in the bytes and them and store
        Digitals[j]= temp & mask;
        temp = (temp >> 1);
      }
    }
  }
//DEBUG
 for(int i=0; i < 16; i++){
      Serial.println("digital ON");
      Serial.println(Digitals[i],HEX);
    }



  //look at ANA
  unsigned long inTemp = 0;

  if(howManyAna !=0 && howManyDigital !=0){         // if there was digital bytes add them in
    for (int i =0; i< howManyAna; i++){
      inTemp = 0;
	  inTemp = _data[6+(i*2)]<< 8;
	  Serial.println("inTemp1");
	  Serial.println(inTemp);                      //GET ANA BYTES AND ADD TOGETHER igh byt first shift it
      inTemp += (_data[6+(i*2)+1]);
	   Serial.println("inTemp2");
	 Serial.println(inTemp);
      anaValues[i] = long((inTemp * 1200)/1024) ;   //(A/D reading * 1200mV) / 1024 it only goes to 1.2v
     Serial.println("inTemp3");
	 Serial.println(inTemp);
	 Serial.println("value");
	 Serial.println(anaValues[i]);
	}
  }
  else if(howManyAna !=0){
    for (int i =0; i< howManyAna; i++){             // else leave them out
      inTemp = 0;
	  inTemp = (_data[4+(i*2)]<< 8);
	  Serial.println("inTemp1");
	  Serial.println(inTemp);                     //NO DIGITAL BYTES high byte first shift it
      inTemp += _data[4+(i*2)+1];
	   Serial.println("inTemp2");
	   Serial.println(inTemp);
      anaValues[i]= long((inTemp* 1200)/1024);      //(A/D reading * 1200mV) / 1024 it only goes to 1.2v
	 Serial.println("inTemp3");
	 Serial.println(inTemp);
	 Serial.println("value");
	 Serial.println(anaValues[i]);
		}
  }



}



