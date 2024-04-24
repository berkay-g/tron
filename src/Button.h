#pragma once

#include <string>
#include "App.h"

class Button
{
public:
    SDL_FRect dimensions;
    SDL_FRect textDimensions;
    Color buttonColor, hoverColor, clickColor;
    Color textColor, textHoverColor;
    bool hovering = false;
    bool clicked = false;
    bool mousePressed = false;
    std::string buttonText;
    SDL_Keycode activateKey;
    SDL_Keycode key;

    Button(const SDL_FRect &dimensions, const Color &buttonColor, const Color &hoverColor, const Color &clickColor, const std::string &buttonText, const Color &textColor, const Color &textHoverColor, SDL_Keycode activateKey = SDLK_UNKNOWN);
    Button() {};

    void update(float mouseX, float mouseY);

    void display(App &app);

    void changeDims(const SDL_FRect &dimensions);
    
    bool overButton(float mouseX, float mouseY);
};