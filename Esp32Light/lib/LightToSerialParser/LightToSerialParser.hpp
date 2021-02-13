#pragma once

#include "Utils.hpp"

class LightToSerialParser
{

private:
    uint8_t buffer_size = 2;
    uint8_t buffer[2];
    // first part of event to handle
    uint8_t ev_0;
    // second part of event to handle
    uint8_t ev_1;

public:
    LightToSerialParser();
    void readData();
    bool moreEvents();
    t_lightEvent *parseMessage();
};
