#include "Player.h"

void Player::Update()
{
    if (!dead)
    {
        trail.insert(head);
        head = head + direction;

        SDL_FRect rect = {head.x * size, head.y * size, size, size};
        light.push_back(rect);
    }
}

void Player::Draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, dead ? 50 : color.a);
    SDL_RenderFillRects(renderer, light.data(), (int)light.size());

    SDL_FRect rect = {head.x * size, head.y * size, size, size};
    SDL_RenderFillRect(renderer, &rect);
}

const bool Player::CheckCollision(Player &other) const
{
    return other.dead ? false : other.trail.find(head) != other.trail.end();
}

const bool Player::CheckCollision() const
{
    return trail.find(head) != trail.end();
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
    if (head.x < 0)
        head.x = -1 + short(borderWidth / size);
    if (head.x > -1 + borderWidth / size)
        head.x = 0;
    if (head.y < 0)
        head.y = -1 + short(borderHeight / size);
    if (head.y > -1 + borderHeight / size)
        head.y = 0;
}

void Player::ResetPosition()
{
    light.clear();
    SDL_FRect rect = {iPos.x * size, iPos.y * size, size, size};
    light.push_back(rect);

    trail.clear();

    head = iPos;
    direction = iDir;
    dead = false;
}