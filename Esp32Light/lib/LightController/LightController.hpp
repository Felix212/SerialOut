#include "Globals.hpp"
#include "SupportStructures.hpp"

class LightController
{
private:
    int x;
    CRGBSet *support_array;
    // in which status is the light
    t_status status;

    // begin led index
    size_t from;
    // to -1 is last led index of this light controller
    size_t to;

    CRGB color;
    CRGB color_flash;
    CRGB actual_color;
    bool color_changed;
    
    // time of when light was turn on
    uint32_t status_start_time;

    // flag to turn of light after minimum time passed
    bool have_to_turn_off;
    void flashLight();
    void fadeLight();

public:
    LightController(CRGBSet *, size_t, size_t);
    ~LightController();
    void update(uint32_t);
    void update_color(CRGB);
    void reset_colors();
    void reset_status();
    void update_status(t_status, uint32_t);
};
