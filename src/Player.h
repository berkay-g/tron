#pragma once

#include <vector>
#include <string>
#include "util.h"
#include "Color.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_rect.h"

#include <unordered_set>

struct Vec2Hash
{
    std::size_t operator()(const Vec2s &v) const
    {
        // Combine the hash values of the x and y components
        return (std::hash<short>()(v.x)) ^ (std::hash<short>()(v.y));
    }
};

class Player
{
public:
    Player(const Color &color, const Vec2s &pos, const Vec2s &dir, const SDL_KeyCode keyCodes[4], float size = 5.f)
        : direction(dir), color(color), size(size), dead(false), score(0), iPos(pos), iDir(dir)
    {
        SDL_FRect rect = {pos.x * size, pos.y * size, size, size};
        light.push_back(rect);

        head = pos;

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
    std::vector<SDL_FRect> light;

    std::unordered_set<Vec2s, Vec2Hash> trail;
    Vec2s head;

    Color color;
    Vec2s direction;
    Vec2s iPos;
    Vec2s iDir;
    float size;
    bool dead;
    int score;
    SDL_KeyCode keys[4];
};