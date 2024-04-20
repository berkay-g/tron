#include "App.h"

#include <vector>
#include "Player.h"

bool endgame = false;
bool paused = true;
bool navigationLock[2] = {0};

const SDL_KeyCode wasd[4] = {SDLK_w, SDLK_a, SDLK_s, SDLK_d};
const SDL_KeyCode arrows[4] = {SDLK_UP, SDLK_LEFT, SDLK_DOWN, SDLK_RIGHT};

Player blue(Color(0, 155, 255), Vec2s(70, 110), Vec2s(0, -1), arrows);
Player red(Color(255, 155, 0), Vec2s(50, 10), Vec2s(0, 1), wasd);

float speed = 70.f;
float timeCounter = 0.f;

void App::Setup()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    blue.iPos = Vec2s((short)(window_width / blue.size) - 50, (short)(window_height / blue.size) - 10);
    blue.ResetPosition();
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

            if (!navigationLock[0])
            {
                if (blue.KeyEvents(event))
                    navigationLock[0] = true;
            }

            if (!navigationLock[1])
            {
                if (red.KeyEvents(event))
                    navigationLock[1] = true;
            }

            break;
        }
    }

    if (!paused)
    {
        timeCounter += deltaTime;
        while (timeCounter >= 1 / speed)
        {
            timeCounter -= 1 / speed;
            blue.Update();
            red.Update();
            navigationLock[0] = false;
            navigationLock[1] = false;

            blue.BorderTeleport(window_width, window_height);
            red.BorderTeleport(window_width, window_height);

            if (red.CheckCollision() || red.CheckCollision(blue))
            {
                if (!endgame)
                {
                    blue.score += 1;
                    SDL_SetWindowTitle(window, ">>>>");
                }
                red.dead = true;
                endgame = true;
            }
            if (blue.CheckCollision() || blue.CheckCollision(red))
            {
                if (!endgame)
                {
                    red.score += 1;
                    SDL_SetWindowTitle(window, "<<<<");
                }
                blue.dead = true;
                endgame = true;
            }

            if (!endgame && red.head == blue.head)
            {
                blue.dead = true;
                red.dead = true;
                endgame = true;
            }
            
        }
    }
    else
        timeCounter = 0.f;
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