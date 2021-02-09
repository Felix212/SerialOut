
/*
uint32_t laser_left_timer_start;
uint32_t laser_right_timer_start;
uint32_t laser_left_time_update;
uint32_t laser_right_time_update;

init_lasers();

void init_lasers()
{
    leftLaser.strip_part_index = 1;
    rightLaser.strip_part_index = 5;
    laser_left_timer_start = 0;
    laser_right_timer_start = 0;
    laser_left_time_update = 151 / leftLaser.laserSpeed;
    laser_right_time_update = 151 / rightLaser.laserSpeed;
}

void ledwalkleft(struct Laser *laser)
{
    current_handler = &stripeControl[laser->strip_part_index];

    if (current_handler->status.on == 0)
    {
        return;
    }

    if (current_handler->from + laser->laserIndex == current_handler->to)
    {
        laser->laserIndex = 0;
        laser->toggle = !laser->toggle;
    }

    if (current_mills_cached - laser_left_timer_start > laser_left_time_update)
    {
        if (laser->toggle)
        {
            support_array[current_handler->from + laser->laserIndex] = CRGB::Black;
        }
        else
        {
            support_array[current_handler->from + laser->laserIndex] = stripeControl[laser->strip_part_index].actual_color;
        }

        laser->laserIndex++;
        laser_left_timer_start = current_mills_cached;
    }
}

void ledwalkright(struct Laser *laser)
{
    current_handler = &stripeControl[laser->strip_part_index];

    if (current_handler->status.ON == 0)
    {
        return;
    }

    if (current_handler->to - laser->laserIndex - 1 < current_handler->from)
    {
        laser->laserIndex = 0;
        laser->toggle = !laser->toggle;
    }

    if (current_mills_cached - laser_right_timer_start > laser_right_time_update)
    {
        if (laser->toggle)
        {
            support_array[current_handler->to - laser->laserIndex - 1] = CRGB::Black;
        }
        else
        {
            support_array[current_handler->to - laser->laserIndex - 1] = stripeControl[laser->strip_part_index].actual_color;
        }

        laser->laserIndex++;
        laser_right_timer_start = current_mills_cached;
    }
}

*/
