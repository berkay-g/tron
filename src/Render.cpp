#include "App.h"

#include <vector>
#include "Player.h"
#include "Bikes.h"

bool endgame = false;
bool paused = true;
bool navigationLock[2] = {0};

const SDL_Keycode wasd[] = {SDLK_w, SDLK_a, SDLK_s, SDLK_d};
const SDL_Keycode arrows[4] = {SDLK_UP, SDLK_LEFT, SDLK_DOWN, SDLK_RIGHT};

Player blue(Color(0, 155, 255), Vec2s(70, 110), Vec2s(0, -1), arrows);
// Player yellow(Color(255, 255, 0), Vec2s(70, 110), Vec2s(0, -1), arrows);
Player orange(Color(255, 155, 0), Vec2s(50, 10), Vec2s(0, 1), wasd);
// Player red(Color(255, 35, 35), Vec2s(50, 10), Vec2s(0, 1), wasd);

float speed = 65.f;
float timeCounter = 0.f;

App::Texture blueBike, orangeBike, bg;

void App::Setup()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    blue.iPos = Vec2s((short)(window_width / blue.size) - 50, (short)(window_height / blue.size) - 10);
    blue.ResetPosition();

    blueBike.Bind(bBike, bBikeSize, renderer);
    orangeBike.Bind(oBike, oBikeSize, renderer);

    bg.Bind("bg.jpg", renderer);
    SDL_SetTextureColorMod(bg.texture, 75, 75, 75);
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
                orange.ResetPosition();
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
                if (orange.KeyEvents(event))
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
            blue.BorderTeleport(window_width, window_height);
            orange.BorderTeleport(window_width, window_height);

            blue.Update();
            orange.Update();
            navigationLock[0] = false;
            navigationLock[1] = false;

            if (orange.trail.back() == blue.trail.back() && (!blue.dead && !orange.dead))
            {
                blue.dead = true;
                orange.dead = true;
                endgame = true;
            }

            if (orange.CheckCollision() || orange.CheckCollision(blue))
            {
                if (!endgame)
                {
                    blue.score += 1;
                    SDL_SetWindowTitle(window, ">>>>");
                }
                orange.dead = true;
                endgame = true;
            }
            if (blue.CheckCollision() || blue.CheckCollision(orange))
            {
                if (!endgame)
                {
                    orange.score += 1;
                    SDL_SetWindowTitle(window, "<<<<");
                }
                blue.dead = true;
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

    if (bg.texture != NULL)
    {
        SDL_FRect srcrect = {0.f, 0.f, 2000.f, 2000.f};
        SDL_FRect dscrect = {0.f, 0.f, window_width * 1.f, window_height * 1.f};
        SDL_RenderTexture(renderer, bg.texture, &srcrect, &dscrect);
    }

    blue.Draw(renderer);
    orange.Draw(renderer);

    SetStringTextureColorMode({255, 155, 0});
    DrawString(std::to_string(orange.score), {30, 10, 30, 30});

    SetStringTextureColorMode({0, 155, 255});
    DrawString(std::to_string(blue.score), {window_width - 45.f, 10, 30, 30});

    SDL_FRect dscrect = {blue.light.back().x - 12.0f, blue.light.back().y - 5.f, 29.f, 15.f};
    if (!blue.dead)
        SDL_RenderTextureRotated(renderer, blueBike.texture, NULL, &dscrect, blue.GetAngle(), NULL, SDL_FLIP_NONE);
    dscrect = {orange.light.back().x - 12.0f, orange.light.back().y - 5.f, 29.f, 15.f};
    if (!orange.dead)
        SDL_RenderTextureRotated(renderer, orangeBike.texture, NULL, &dscrect, orange.GetAngle(), NULL, SDL_FLIP_NONE);

    // this->ImguiRender();
    SDL_RenderPresent(renderer);
}