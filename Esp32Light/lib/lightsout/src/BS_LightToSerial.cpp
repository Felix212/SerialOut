#include "BS_LightToSerial.h"
#include "Arduino.h"
#define byte uint8_t

BS_LightToSerial::BS_LightToSerial()
{
	left = (color) {255, 0, 0};
	right = (color) {0, 125, 255};
}

BS_LightEvent BS_LightToSerial::ParseMessage(byte msg[])
{
	BS_LightEvent message;
	//= (BS_LightEvent){msg[0], msg[1], msg[2]};
	if (msg[0] < 252)
	{
		message.type = (msg[0] >> 4);
		message.value = (msg[0] & 15);
		if (msg[1] != 0)
		{
			message.color.r = (msg[1] >> 5) * 32;
			message.color.g = ((msg[1] & 28) >> 2) * 32;
			message.color.b = (msg[1] & 3) * 64;
		}
		else
		{
			message.color.r = 0;
			message.color.b = 0;
			message.color.g = 0;
		}
	}
	else
	{
		if (msg[0] == 255)
		{
			message.type = msg[0];
			message.value = msg[1];
		}
		if (msg[0] == 254 || msg[0] == 253)
		{
			message.type = msg[0];
			message.color.r = (msg[1] >> 5) * 32;
			message.color.g = ((msg[1] & 28) >> 2) * 32;
			message.color.b = (msg[1] & 3) * 64;
		}
	}

	return message;
}