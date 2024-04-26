#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "util.h"
#include "Color.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_rect.h"

class Player
{
public:
    Player(const Color &color, const Vec2s &pos, const Vec2s &dir, const SDL_Keycode keyCodes[4], int maxTrailLength = 360, float size = 5.f)
        : direction(dir), color(color), size(size), dead(false), score(0), iPos(pos), iDir(dir), maxTrailLength(maxTrailLength)
    {
        SDL_FRect rect = {pos.x * size, pos.y * size, size, size};
        light.push_back(rect);

        trail.push_back(pos);

        for (size_t i = 0; i < 4; ++i)
        {
            keys[i] = keyCodes[i];
        }
    }

    void Update();

    bool KeyEvents(SDL_Event &event);

    void BorderTeleport(int borderWidth, int borderHeight);

    const bool CheckCollision(Player &other) const;
    const bool CheckCollision() const;

    void Draw(SDL_Renderer *renderer);

    const double GetAngle() const;

    void ResetPosition();

public:
    std::vector<Vec2s> trail;
    std::vector<SDL_FRect> light;
    int maxTrailLength;

    Color color;
    Vec2s direction;
    Vec2s iPos;
    Vec2s iDir;
    float size;
    bool dead;
    int score;
    SDL_Keycode keys[4];
};
