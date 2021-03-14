/*
  This file is part of the ArduinoDMX library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/



#include "_DMX.h"

DMXClass::DMXClass() :
  rs485(SerialUART3,PB10, PB4, -1),
  _universeSize(0),
  _transmissionBegin(false)
{
}

int DMXClass::begin(int universeSize)
{
  if (universeSize < 1 || universeSize > DMX_MAX_CHANNELS) {
    return 0;
  }

  _universeSize = universeSize;
  _transmissionBegin = false;
  memset(_values, 0x00, _universeSize);

  rs485.begin(250000);

  return 1;
}

void DMXClass::end()
{
  rs485.end();
}

int DMXClass::beginTransmission()
{
  _transmissionBegin = true;

  return 1;
}

int DMXClass::write(int channel, byte value)
{
  if (!_transmissionBegin || channel < 1 || channel > _universeSize) {
    return 0;
  }

  _values[channel] = value;

  return 1;
}

int DMXClass::writeAll(byte value)
{
  memset(_values + 1, value, _universeSize);

  return 1;
}

int DMXClass::endTransmission()
{
  rs485.beginTransmission();
  rs485.sendBreakMicroseconds(88);
  delayMicroseconds(12);

  rs485.write(_values, _universeSize + 1);
  rs485.endTransmission();

  _transmissionBegin = false;

  return 1;
}

DMXClass DMX;
