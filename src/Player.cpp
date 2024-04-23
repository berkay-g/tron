#include "Player.h"

void Player::Update()
{
    if (!dead)
    {
        if ((light.back().x != trail.back().x * size) || (light.back().y != trail.back().y * size))
        {
            light.back().x = trail.back().x * size;
            light.back().y = trail.back().y * size;
        }

        trail.push_back(trail.back() + direction);
        SDL_FRect rect = {trail.back().x * size, trail.back().y * size, size, size};
        light.push_back(rect);

        if (light.size() >= maxTrailLength)
        {
            light.erase(light.begin());
            trail.erase(trail.begin());
        }
    }
}

void Player::Draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, dead ? 50 : color.a);
    SDL_RenderFillRects(renderer, light.data(), (int)light.size());
}

const bool Player::CheckCollision(Player &other) const
{
    return other.dead ? false : std::find(other.trail.begin(), other.trail.end(), trail.back()) != other.trail.end();
}

const bool Player::CheckCollision() const
{
    return std::find(std::next(trail.rbegin()), trail.rend(), trail.back()) != trail.rend();
}

bool Player::KeyEvents(SDL_Event &event)
{
    bool pressed = false;
    if (event.key.keysym.sym == keys[0] && direction.y != 1)
    {
        direction = {0, -1};
        pressed = true;
    }

    else if (event.key.keysym.sym == keys[1] && direction.x != 1)
    {
        direction = {-1, 0};
        pressed = true;
    }

    else if (event.key.keysym.sym == keys[2] && direction.y != -1)
    {
        direction = {0, 1};
        pressed = true;
    }

    else if (event.key.keysym.sym == keys[3] && direction.x != -1)
    {
        direction = {1, 0};
        pressed = true;
    }
    return pressed;
}

void Player::BorderTeleport(int borderWidth, int borderHeight)
{
    if (trail.back().x < 0)
        trail.back().x = -1 + short(borderWidth / size);
    if (trail.back().x > -1 + borderWidth / size)
        trail.back().x = 0;
    if (trail.back().y < 0)
        trail.back().y = -1 + short(borderHeight / size);
    if (trail.back().y > -1 + borderHeight / size)
        trail.back().y = 0;
}

const double Player::GetAngle() const
{
    if (direction == Vec2s(0, -1)) // Up
        return 90.0;
    if (direction == Vec2s(0, 1)) // Down
        return 270.0;
    if (direction == Vec2s(-1, 0)) // Left
        return 0.0;
    return 180.0; // Right
}

void Player::ResetPosition()
{
    light.clear();
    SDL_FRect rect = {iPos.x * size, iPos.y * size, size, size};
    light.push_back(rect);

    trail.clear();
    trail.push_back(iPos);

    direction = iDir;

    dead = false;
}