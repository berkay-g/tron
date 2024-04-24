#include "Button.h"

Button::Button(const SDL_FRect &dimensions, const Color &buttonColor, const Color &hoverColor, const Color &clickColor, const std::string &buttonText, const Color &textColor, const Color &textHoverColor, SDL_Keycode activateKey)
    : dimensions(dimensions), buttonColor(buttonColor), hoverColor(hoverColor), clickColor(clickColor), buttonText(buttonText), textColor(textColor), textHoverColor(textHoverColor), activateKey(activateKey)
{
    this->dimensions.x -= (dimensions.w / 2);
    this->dimensions.y -= (dimensions.h / 2);

    textDimensions = this->dimensions;
    textDimensions.w /= buttonText.length();
    textDimensions.h = textDimensions.w;
    textDimensions.y += (this->dimensions.h / 2.f) - textDimensions.h / 2.f;
    textDimensions.x += (this->dimensions.w / 2.f) - (0.65f * textDimensions.w * buttonText.length() / 2.f);
}

void Button::update(float mouseX, float mouseY)
{
    hovering = overButton(mouseX, mouseY);

    if ((mousePressed && hovering) || ((key == activateKey)))
    {
        clicked = true;
    }
    else
        clicked = false;
}

void Button::display(App &app)
{
    if (clicked)
        app.SetRenderDrawColor(clickColor);
    else if (hovering)
        app.SetRenderDrawColor(hoverColor);
    else
        app.SetRenderDrawColor(buttonColor);

    SDL_RenderFillRect(app.GetSDLRenderer(), &dimensions);

    app.SetStringTextureColorMode((hovering || (key == activateKey)) ? textHoverColor : textColor);
    app.DrawString(buttonText, textDimensions);
    app.SetStringTextureColorMode(Color::White);
}

void Button::changeDims(const SDL_FRect &dimensions)
{
    this->dimensions = dimensions;
    this->dimensions.x -= (dimensions.w / 2);
    this->dimensions.y -= (dimensions.h / 2);

    textDimensions = this->dimensions;
    textDimensions.w /= buttonText.length();
    textDimensions.h = textDimensions.w;
    textDimensions.y += (this->dimensions.h / 2.f) - textDimensions.h / 2.f;
    textDimensions.x += (this->dimensions.w / 2.f) - (0.65f * textDimensions.w * buttonText.length() / 2.f);
}

bool Button::overButton(float mouseX, float mouseY)
{
    return (mouseX >= dimensions.x && mouseX <= dimensions.x + dimensions.w &&
            mouseY >= dimensions.y && mouseY <= dimensions.y + dimensions.h);
}