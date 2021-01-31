#include "LightToSerialParser.hpp"

LightToSerialParser::LightToSerialParser()
{
	for (size_t i = 0; i < buffer_size; ++i)
	{
		buffer[i] = 0;
	}

	total_read_byte = 0;
	current_pos_buffer = 0;
}

int LightToSerialParser::dataAvailable()
{
	return Serial.available();
}

void LightToSerialParser::byteToRGB(CRGB *color, byte data_color)
{
	color->r = (data_color >> 5) * 32;
	color->g = ((data_color & 28) >> 2) * 32;
	color->b = (data_color & 3) * 64;
}

void LightToSerialParser::readData()
{
	total_read_byte = Serial.readBytes(buffer, buffer_size);
	current_pos_buffer = 0;
}

bool LightToSerialParser::moreEvents()
{
	return current_pos_buffer < total_read_byte;
}

t_lightEvent *LightToSerialParser::parseMessage()
{
	t_lightEvent *message = (t_lightEvent *)malloc(sizeof(t_lightEvent));
	//= (BS_LightEvent){msg[0], msg[1], msg[2]};
	if (buffer[current_pos_buffer] < 252)
	{
		message->type = (buffer[current_pos_buffer] >> 4);
		message->value = (buffer[current_pos_buffer] & 15);

		// if buffer[current_pos_buffer + 1] is 0 then 0 will be applied as color -> black
		byteToRGB(&message->color, buffer[current_pos_buffer + 1]);
	}
	else
	{
		switch (buffer[current_pos_buffer])
		{
		case TURNOFFLIGHTS: // 252
			message->type = TURNOFFLIGHTS;
			break;
		case LEFTCOLOR:	 // 253
		case RIGHTCOLOR: // 254
			message->type = buffer[current_pos_buffer];
			byteToRGB(&message->color, buffer[current_pos_buffer + 1]);
			break;

		case CHROMAEVENT: // 255
			message->type = CHROMAEVENT;
			message->value = buffer[current_pos_buffer + 1];
		default:
			break;
		}
	}

	current_pos_buffer += 2;
	return message;
}
