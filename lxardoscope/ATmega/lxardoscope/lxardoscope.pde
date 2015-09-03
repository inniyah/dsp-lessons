/*
 *
 * This file is part of the LXARDOSCOPE package.
 *
 * LXARDOSCOPE is an Arduino based oscilloscope for Linux, using the Xforms library.
 *
 * Copyright (C) 2011 Oskar Leuthold
 * 
 * LXARDOSCOPE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LXARDOSCOPE is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with LXARDOSCOPE; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

//   This program for Arduino Uno reads two channels and sends the data
//   out through the serial port in 4 bytes.
//   For synchronization purposes, the following scheme was chosen:
//   A0 data:   A09 (MSB) A08 A07 A06 A05 A04 A03 A02 A01 A00 (LSB)
//   A1 data:   A19 (MSB) A18 A17 A16 A15 A14 A13 A12 A11 A10 (LSB)
//   sent as byte 1:   1 1 1 A09 A08 A07 A06 A05
//       and byte 2:   0 1 1 A04 A03 A02 A01 A00
//       and byte 3:   0 1 1 A19 A18 A17 A16 A15
//       and byte 4:   0 1 1 A14 A13 A12 A11 A10
//
//    (This arrangement was chosen for hystorical reasons; there are
//     many other possibilities. 3 bytes would be enough, but this could
//     possibly create a nonsymmetry between the channels.
//
//
int sensorValue = 0;        // value read from the pot
byte lb;
byte hb;

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200); 
}

void loop() {
// read A0:
  sensorValue = analogRead(A0);            
// shift sample by 3 bits, and select higher byte  
  hb=highByte(sensorValue<<3); 
// set 3 most significant bits and send out
  Serial.write(hb|224); 
// select lower byte and clear 3 most significant bits
  lb=(lowByte(sensorValue))&31;
// set bits 5 and 6 and send out
  Serial.write(lb|96);
// read A1
  sensorValue = analogRead(A1);            
// shift sample by 3 bits, and select higher byte 
  hb=highByte(sensorValue<<3); 
// set bits 5 and 6 and send out
  Serial.write(hb|96); 
// select lower byte and clear 3 most significant bits
  lb=(lowByte(sensorValue))&31;
// set bits 5 and 6 and send out
  Serial.write(lb|96);
}
