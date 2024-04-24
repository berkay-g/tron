#include "App.h"

#include <vector>

#include "Player.h"
#include "Bikes.h"
#include "Button.h"

bool mainMenu = true;
bool endgame = false;
bool paused = true;

const SDL_Keycode wasd[] = {SDLK_w, SDLK_a, SDLK_s, SDLK_d};
const SDL_Keycode arrows[4] = {SDLK_UP, SDLK_LEFT, SDLK_DOWN, SDLK_RIGHT};
const SDL_Keycode uhjk[] = {SDLK_u, SDLK_h, SDLK_j, SDLK_k};
const SDL_Keycode kp[] = {SDLK_KP_5, SDLK_KP_7, SDLK_KP_8, SDLK_KP_9};

const Color blueColor = Color(0, 155, 255);
const Color orangeColor = Color(255, 155, 0);
const Color yellowColor = Color(255, 255, 0);
const Color redColor = Color(255, 35, 35);

Player cycles[] = {
    Player(blueColor, Vec2s(150, 110), Vec2s(0, -1), arrows),
    Player(orangeColor, Vec2s(50, 10), Vec2s(0, 1), wasd),
    Player(yellowColor, Vec2s(20, 90), Vec2s(1, 0), uhjk),
    Player(redColor, Vec2s(180, 20), Vec2s(-1, 0), kp)};

bool navigationLock[4] = {0};

int playerCount = 2;

float speed = 65.f;
float timeCounter = 0.f;

App::Texture blueBike, orangeBike, bg;

App::Texture cycleTextures[4];

Button test({100.f, 100.f, 120.f, 40.f}, Color(40, 40, 40, 0), Color(8, 170, 249, 155), Color(5, 134, 199), "start", Color(197, 197, 197), Color::White, SDLK_SPACE);
Button incDec[4];

void testButton();
void increasePlayerCount();
void decreasePlayerCount();
void increaseSpeed();
void decreaseSpeed();

void App::Setup()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    cycleTextures[0].Bind(bBike, bBikeSize, renderer);
    cycleTextures[1].Bind(oBike, oBikeSize, renderer);
    cycleTextures[2].Bind(yBike, yBikeSize, renderer);
    cycleTextures[3].Bind(rBike, rBikeSize, renderer);

    bg.Bind("bg.jpg", renderer);
    SDL_SetTextureColorMod(bg.texture, 50, 50, 50);

    test.changeDims({window_width / 2.f, -100 + window_height / 2.f, 120.f, 40.f});

    for (int i = 0; i < 4; i++)
    {
        if (i % 2 == 0)
            incDec[i] = Button({120.f, 150.f, 30.f, 30.f}, Color(40, 40, 40, 0), Color(8, 170, 249, 0), Color(100, 100, 100, 30), ">", Color(197, 197, 197), Color::White, SDLK_0);
        else
            incDec[i] = Button({120.f, 150.f, 30.f, 30.f}, Color(40, 40, 40, 0), Color(8, 170, 249, 0), Color(100, 100, 100, 30), "<", Color(197, 197, 197), Color::White, SDLK_0);
    }

    incDec[0].changeDims({25 + window_width / 2.f, window_height / 2.f, 30.f, 30.f});
    incDec[1].changeDims({-25 + window_width / 2.f, window_height / 2.f, 30.f, 30.f});
    incDec[2].changeDims({25 + window_width / 2.f, 75 + window_height / 2.f, 30.f, 30.f});
    incDec[3].changeDims({-25 + window_width / 2.f, 75 + window_height / 2.f, 30.f, 30.f});
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
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (test.hovering && mainMenu)
                testButton();

            if (incDec[0].hovering && mainMenu)
                increasePlayerCount();
            if (incDec[1].hovering && mainMenu)
                decreasePlayerCount();
            if (incDec[2].hovering && mainMenu)
                increaseSpeed();
            if (incDec[3].hovering && mainMenu)
                decreaseSpeed();

            test.mousePressed = true;
            for (int i = 0; i < 4; i++)
                incDec[i].mousePressed = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            test.mousePressed = false;
            for (int i = 0; i < 4; i++)
                incDec[i].mousePressed = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                if (!mainMenu)
                {
                    mainMenu = true;
                    SDL_SetTextureColorMod(bg.texture, 50, 50, 50);

                    for (auto &c : cycles)
                        c.ResetPosition();
                    paused = true;
                    endgame = false;
                }
                else
                    quit = true;
            }

            else if (event.key.keysym.sym == SDLK_k)
            {
                SDL_Log("fps = %f", 1 / deltaTime);
            }

            else if (event.key.keysym.sym == SDLK_0)
                increasePlayerCount();
            else if (event.key.keysym.sym == SDLK_9)
                decreasePlayerCount();

            if (event.key.keysym.sym == SDLK_SPACE && (paused || endgame) && !mainMenu)
            {
                for (auto &c : cycles)
                    c.ResetPosition();
                paused = false;
                if (endgame)
                    paused = true;

                endgame = false;
            }

            test.key = event.key.keysym.sym;
            if (test.activateKey == test.key && mainMenu)
                testButton();

            if (paused || mainMenu)
                break;

            for (size_t i = 0; i < playerCount; i++)
            {
                if (!navigationLock[i])
                {
                    if (cycles[i].KeyEvents(event))
                        navigationLock[i] = true;
                }
            }

            break;
        case SDL_EVENT_KEY_UP:
            test.key = SDLK_UNKNOWN;
            break;
        }
    }

    if (!paused)
    {
        timeCounter += deltaTime;
        while (timeCounter >= 1 / speed)
        {
            timeCounter -= 1 / speed;

            for (size_t i = 0; i < playerCount; i++)
            {
                cycles[i].Update();
                navigationLock[i] = false;
                cycles[i].BorderTeleport(window_width, window_height);

                for (size_t j = 0; j < playerCount; j++)
                {
                    if (j == i)
                        continue;
                    if (cycles[i].trail.back() == cycles[j].trail.back() && (!cycles[i].dead && !cycles[j].dead))
                    {
                        cycles[i].dead = true;
                        cycles[j].dead = true;
                    }
                }

                for (size_t j = 0; j < playerCount; j++)
                {
                    if (j == i)
                        continue;

                    if ((cycles[i].CheckCollision() || cycles[i].CheckCollision(cycles[j])) && !cycles[i].dead)
                        cycles[i].dead = true;
                }
            }

            int deadCount = 0;
            for (size_t i = 0; i < playerCount; i++)
            {
                if (cycles[i].dead)
                    deadCount++;
            }
            if (deadCount >= playerCount - 1)
            {
                size_t index = -1;
                for (size_t i = 0; i < playerCount; i++)
                {
                    if (!cycles[i].dead)
                    {
                        index = i;
                        break;
                    }
                }
                if (!endgame && index != -1)
                    cycles[index].score += 1;
                endgame = true;
            }
        }
    }
    else
        timeCounter = 0.f;

    if (mainMenu)
    {
        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        test.update(mouseX, mouseY);
        for (int i = 0; i < 4; i++)
            incDec[i].update(mouseX, mouseY);
    }
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

    if (mainMenu)
    {
        test.display(*this);
        for (int i = 0; i < 4; i++)
            incDec[i].display(*this);

        DrawString(std::to_string(playerCount), {-10 + window_width / 2.f, -13 + window_height / 2.f, 26, 26});

        DrawString(std::to_string((int)speed), {-18 + window_width / 2.f, 75 - 13 + window_height / 2.f, 26, 26});
    }
    else
    {
        for (size_t i = 0; i < playerCount; i++)
            cycles[i].Draw(renderer);

        SetStringTextureColorMode(blueColor);
        DrawString(std::to_string(cycles[0].score), {30, 10, 18, 18});
        SetStringTextureColorMode(orangeColor);
        DrawString(std::to_string(cycles[1].score), {70, 10, 18, 18});

        if (playerCount > 2)
        {
            SetStringTextureColorMode(yellowColor);
            DrawString(std::to_string(cycles[2].score), {110, 10, 18, 18});
            if (playerCount > 3)
            {
                SetStringTextureColorMode(redColor);
                DrawString(std::to_string(cycles[3].score), {150, 10, 18, 18});
            }
        }

        for (size_t i = 0; i < playerCount; i++)
        {
            SDL_FRect dscrect = {cycles[i].light.back().x - 12.0f, cycles[i].light.back().y - 5.f, 29.f, 15.f};
            if (!cycles[i].dead)
                SDL_RenderTextureRotated(renderer, cycleTextures[i].texture, NULL, &dscrect, cycles[i].GetAngle(), NULL, SDL_FLIP_NONE);
        }
    }

    // this->ImguiRender();
    SDL_RenderPresent(renderer);
}

void testButton()
{
    mainMenu = false;
    SDL_Log("woo hoo");
    SDL_SetTextureColorMod(bg.texture, 100, 100, 100);
}

void increasePlayerCount()
{
    playerCount >= 4 ? playerCount = 4 : playerCount++;
}

void decreasePlayerCount()
{
    playerCount <= 2 ? playerCount = 2 : playerCount--;
}

void increaseSpeed()
{
    speed >= 95 ? speed = 95 : speed += 5;
}

void decreaseSpeed()
{
    speed <= 10 ? speed = 10 : speed -= 5;
}