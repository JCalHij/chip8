#pragma once

// Initializes the input module for current frame. 
// Updates held keys and resets pressed and released flags
void input_new_frame();

// Pass SDL key_action (Pressed or Released) along with the
// key, and update the corresponding data
void input_update(unsigned int key_action, unsigned int key);

// Returns whether given key has been pressed on this frame
bool input_is_key_pressed(unsigned int key);
// Returns whether given key is being held on this frame
bool input_is_key_held(unsigned int key);
// Returns whether given key has been released on this frame
bool input_is_key_released(unsigned int key);