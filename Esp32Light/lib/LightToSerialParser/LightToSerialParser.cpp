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

void LightToSerialParser::byteToRGB(CRGB *color, byte data_color)
{
	color->r = (data_color >> 5) * 32;
	color->g = ((data_color & 28) >> 2) * 32;
	color->b = (data_color & 3) * 64;
}

// Read buffer_size bytes from Serial and stores them in parser buffer
void LightToSerialParser::readData()
{
	Serial.readBytes(buffer, buffer_size);
}

LightGroup valToLightGroup(byte val)
{
	switch (val)
	{
	case BACKTOPLASER:
		return LightGroup::BackTopLaser;
	case RINGLASER:
		return LightGroup::RingLaser;
	case LEFTLASER:
		return LightGroup::LeftLaser;
	case RIGHTLASER:
		return LightGroup::RightLaser;
	case CENTERLIGHT:
		return LightGroup::CenterLight;
	default:
		return LightGroup::Error;
	}
}

SetupEvents nameToSetupEvent(byte name)
{
	switch (name)
	{
	case SETUPEVENTS:
		return SetupEvents::First_Song_Event;
	case TURNOFFLIGHTS:
		return SetupEvents::Turn_Off_Lights;
	case LEFTCOLOR:
		return SetupEvents::Left_Color;
	case RIGHTCOLOR:
		return SetupEvents::Right_Color;
	case CHROMAEVENT:
		return SetupEvents::Chroma_Event;
	default:
		return SetupEvents::Error;
	}
}

ShowEvents nameToShowEvent(byte name)
{
	switch (name)
	{
	case LIGHT_OFF:
		return ShowEvents::Light_Off;
	case RIGHT_COLOR_ON:
		return ShowEvents::Right_Color_On;
	case RIGHT_COLOR_FADE:
		return ShowEvents::Right_Color_Fade;
	case RIGHT_COLOR_FLASH:
		return ShowEvents::Right_Color_Flash;
	case LEFT_COLOR_ON:
		return ShowEvents::Left_Color_On;
	case LEFT_COLOR_FADE:
		return ShowEvents::Left_Color_Fade;
	case LEFT_COLOR_FLASH:
		return ShowEvents::Left_Color_Flash;
	case LEFTLASERSPEED:
		return ShowEvents::Left_Laser_Speed;
	case RIGHTLASERSPEED:
		return ShowEvents::Right_Laser_Speed;
	default:
		return ShowEvents::Error;
	}
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
		message->event_name = nameToSetupEvent(ev_0);
		if (message->event_name == SetupEvents::Left_Color ||
			message->event_name == SetupEvents::Right_Color)
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
		message->event_name = nameToShowEvent(ev_0 >> 4);
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
		message->event_name = nameToShowEvent(ev_0 & 15);
		byteToRGB(&message->color, ev_1);
	}

	return message;
}
