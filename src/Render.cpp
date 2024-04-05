#include "App.h"

#include "Delay.h"
#include <vector>

class Player
{
public:
    std::vector<Vec2s> light;
    Color color;
    Vec2s direction;
    Vec2s iPos;
    Vec2s iDir;
    float size;
    bool dead;
    int score;

    Player(const Color& color, const Vec2s& pos, const Vec2s& dir, float size = 5.f)
        : direction(dir), color(color), size(size), dead(false), score(0), iPos(pos), iDir(dir)
    {
        light.push_back(pos);
    }

    void Update()
    {
        if (!dead)
            light.push_back(light.back() + direction);
    }

    void Draw(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, dead ? 50 : color.a);
        SDL_FRect rect;
        for (auto &l : light)
        {
            rect = {l.x * size, l.y * size, size, size};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    void ResetPosition()
    {
        light.clear();
        light.push_back(iPos);
        direction = iDir;
        dead = false;
    }
};

bool endgame = false;
bool paused = true;

Player blue({0, 155, 255}, Vec2s{70, 110}, Vec2s{0, -1});
Player red({255, 155, 0}, Vec2s{50, 10}, Vec2s{0, 1});

Delay delay(0.02f);

void App::Setup()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    delay.start = true;
}

void App::Update(SDL_Event &event, float deltaTime)
{
    while (SDL_PollEvent(&event))
    {
        // ImGui_ImplSDL3_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            quit = true;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                quit = true;

            else if (event.key.keysym.sym == SDLK_k)
            {
                SDL_Log("fps = %f", 1 / deltaTime);
            }

            else if (event.key.keysym.sym == SDLK_SPACE && (paused || endgame))
            {   
                blue.ResetPosition();
                red.ResetPosition();
                paused = false;
                if (endgame)
                    paused = true;

                endgame = false;
            }

            if (paused)
                break;

            if (event.key.keysym.sym == SDLK_UP && blue.direction.y != 1)
            {
                blue.direction = {0, -1};
                paused = false;
            }

            if (event.key.keysym.sym == SDLK_DOWN && blue.direction.y != -1)
            {
                blue.direction = {0, 1};
                paused = false;
            }

            if (event.key.keysym.sym == SDLK_LEFT && blue.direction.x != 1)
            {
                blue.direction = {-1, 0};
                paused = false;
            }

            if (event.key.keysym.sym == SDLK_RIGHT && blue.direction.x != -1)
            {
                blue.direction = {1, 0};
                paused = false;
            }


            if (event.key.keysym.sym == SDLK_w && red.direction.y != 1)
            {
                red.direction = {0, -1};
                paused = false;
            }

            if (event.key.keysym.sym == SDLK_s && red.direction.y != -1)
            {
                red.direction = {0, 1};
                paused = false;
            }

            if (event.key.keysym.sym == SDLK_a && red.direction.x != 1)
            {
                red.direction = {-1, 0};
                paused = false;
            }

            if (event.key.keysym.sym == SDLK_d && red.direction.x != -1)
            {
                red.direction = {1, 0};
                paused = false;
            }

            break;
        }
    }

    if (delay.Update(deltaTime))
    {
        if (!paused)
        {
            blue.Update();
            red.Update();
        }
        delay.start = true;
    }

   
    if (blue.light.back().x < 0 || blue.light.back().x > -1 + window_width / blue.size || blue.light.back().y < 0 || blue.light.back().y > -1 + window_height / blue.size)
    {
        if (!endgame) 
        {
            red.score += 1;
            SDL_SetWindowTitle(window, "<<<<");
        }
        blue.dead = true;
        endgame = true;
    }

    if (red.light.back().x < 0 || red.light.back().x > -1 + window_width / red.size || red.light.back().y < 0 || red.light.back().y > -1 + window_height / red.size)
    {
        if (!endgame) 
        {
            blue.score += 1;
            SDL_SetWindowTitle(window, ">>>>");
        }
        red.dead = true;
        endgame = true;
    }
        
    for (size_t i = 0; i < red.light.size(); i++)
    {
        if (red.light[i] == blue.light.back() && !red.dead)
        {
            if (!endgame) 
            {
                red.score += 1;
                SDL_SetWindowTitle(window, "<<<<");
            }
            blue.dead = true;
            endgame = true;
        }

        if (i != 0 && i != red.light.size() -1 && red.light.back() == red.light[i])
        {
            if (!endgame) 
            {
                blue.score += 1;
                SDL_SetWindowTitle(window, ">>>>");
            }
            red.dead = true;
            endgame = true;
        }
    }

    for (size_t i = 0; i < blue.light.size(); i++)
    {
        if (blue.light[i] == red.light.back() && !blue.dead)
        {
            if (!endgame) 
            {
                blue.score += 1;
                SDL_SetWindowTitle(window, ">>>>");
            }
            red.dead = true;
            endgame = true;
        }

        if (i != 0 && i != blue.light.size() -1 && blue.light.back() == blue.light[i])
        {
            if (!endgame) 
            { 
                red.score += 1;
                SDL_SetWindowTitle(window, "<<<<");
            }
            blue.dead = true;
            endgame = true;
        }
    }
    
}

void App::Draw()
{
    // ImGuiIO &io = this->ImguiNewFrame();
    this->SetRenderDrawColor({22, 22, 22});
    SDL_RenderClear(renderer);

    blue.Draw(renderer);
    red.Draw(renderer);

    SetStringTextureColorMode({255, 155, 0});
    DrawString(std::to_string(red.score), {30, 10, 30, 30});

    SetStringTextureColorMode({0, 155, 255});
    DrawString(std::to_string(blue.score), {window_width - 45.f, 10, 30, 30});

    // this->ImguiRender();
    SDL_RenderPresent(renderer);
}