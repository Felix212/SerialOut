#include "LightToSerialParser.hpp"

LightToSerialParser::LightToSerialParser()
{
	for (size_t i = 0; i < buffer_size; ++i)
	{
		buffer[i] = 0;
	}
}

// Checks if there are events to handle
bool LightToSerialParser::moreEvents()
{
	return Serial.available();
}

// Read buffer_size bytes from Serial and stores them in parser buffer
void LightToSerialParser::readData()
{
	Serial.readBytes(buffer, buffer_size);
}

// Decode data in buffer as defined in https://github.com/Felix212/SerialOut/wiki/Serial-Protocol
t_lightEvent * LightToSerialParser::parseMessage()
{
	t_lightEvent *message = (t_lightEvent *)malloc(sizeof(t_lightEvent));
    message->event_value = 0;

	ev_0 = buffer[0];
    ev_1 = buffer[1];

	// Ev_0 has bits are something like 1111xxxx
    if (ev_0 > 239)
    {
		message->event_type = SETUP_EVENTS;
		message->setup_name = nameToSetupEvent(ev_0);
		if (message->setup_name == SetupEvents::Left_Color ||
			message->setup_name == SetupEvents::Right_Color)
		{
			byteToRGB(&message->color, ev_1);
		} 
		else
		{
			message->event_value = ev_1;
		}
    }
	// Ev_0 has bits are something like 11xxyyyy
	// So just 1100yyyy, 1101yyyy, 1110yyyy since 1111xxxx has already been handled
    else if(ev_0 > 191)
	{
		message->event_type = SHOW_EVENTS;
		message->show_name = nameToShowEvent(ev_0 >> 4);
		message->event_value = ev_0 & 15;
	}
	// Ev_0 bits are something like xxxxyyyy
	// xxxx will be light group, yyyy light event to apply to that group
	// 0000, ..., 1011 group
	// 0000, ..., 1011 event
	else
	{
		message->event_type = SHOW_EVENTS;
		message->light_group = valToLightGroup(ev_0 >> 4);
		message->show_name = nameToShowEvent(ev_0 & 15);
		byteToRGB(&message->color, ev_1);
	}

	return message;
}
