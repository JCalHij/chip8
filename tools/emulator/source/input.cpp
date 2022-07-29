#include "SDL2/SDL_events.h"

#include <string.h>
#include <map>


static std::map<unsigned int, bool> mapKeyPressed;
static std::map<unsigned int, bool> mapKeyHeld;
static std::map<unsigned int, bool> mapKeyReleased;


void input_new_frame()
{
    for (auto it = mapKeyPressed.begin(); it != mapKeyPressed.end(); ++it)
    {
        mapKeyHeld[it->first] = it->second? it->second : mapKeyHeld[it->first];
    }
    mapKeyPressed.clear();
    mapKeyReleased.clear();
}


void input_update(unsigned int key_action, unsigned int key)
{
    if (key_action == SDL_KEYUP)
    {
        mapKeyPressed[key] = false;
        mapKeyHeld[key] = false;
        mapKeyReleased[key] = true;
    }
    else if (key_action == SDL_KEYDOWN)
    {
        mapKeyPressed[key] = mapKeyHeld[key]? false : true;
        mapKeyReleased[key] = false;
    }
}


bool input_is_key_pressed(unsigned int key)
{
    return mapKeyPressed[key];
}


bool input_is_key_held(unsigned int key)
{
    return mapKeyHeld[key];
}


bool input_is_key_released(unsigned int key)
{
    return mapKeyReleased[key];
}