#include "App.h"

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

App::Texture cycleTextures[4], bg;

Button startButton({100.f, 100.f, 120.f, 40.f}, Color(40, 40, 40, 0), Color(8, 170, 249, 155), Color(5, 134, 199), "start", Color(197, 197, 197), Color::White, SDLK_SPACE);
int incDecCount = 8;
Button incDec[8];

Vec2i res[] = {Vec2i(300, 300), Vec2i(600, 600), Vec2i(800, 600), Vec2i(1024, 768), Vec2i(1280, 720), Vec2i(1280, 960), Vec2i(1360, 768), Vec2i(1600, 900), Vec2i(1768, 992)};
int currentRes = 1;
int prevRes = 1;

void updateDims(int w, int h);

void startGame(SDL_Window *window);
void increasePlayerCount();
void decreasePlayerCount();
void increaseSpeed();
void decreaseSpeed();
void increaseTrailSize();
void decreaseTrailSize();
void increaseResolution();
void decreaseResolution();

void App::Setup()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    cycleTextures[0].Bind(bBike, bBikeSize, renderer);
    cycleTextures[1].Bind(oBike, oBikeSize, renderer);
    cycleTextures[2].Bind(yBike, yBikeSize, renderer);
    cycleTextures[3].Bind(rBike, rBikeSize, renderer);

    bg.Bind("bg.jpg", renderer);
    SDL_SetTextureColorMod(bg.texture, 50, 50, 50);

    for (int i = 0; i < incDecCount; i++)
    {
        if (i % 2 == 0)
            incDec[i] = Button({120.f, 150.f, 30.f, 30.f}, Color(40, 40, 40, 0), Color(8, 170, 249, 0), Color(100, 100, 100, 30), ">", Color(197, 197, 197), Color::White, SDLK_0);
        else
            incDec[i] = Button({120.f, 150.f, 30.f, 30.f}, Color(40, 40, 40, 0), Color(8, 170, 249, 0), Color(100, 100, 100, 30), "<", Color(197, 197, 197), Color::White, SDLK_0);
    }

    updateDims(window_width, window_height);
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
            if (startButton.hovering && mainMenu)
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    startGame(window);
                    for (auto &c : cycles)
                        c.score = 0;
                }
                else
                    startGame(window);

            if (incDec[0].hovering && mainMenu)
                increasePlayerCount();
            if (incDec[1].hovering && mainMenu)
                decreasePlayerCount();
            if (incDec[2].hovering && mainMenu)
                increaseSpeed();
            if (incDec[3].hovering && mainMenu)
                decreaseSpeed();

            if (incDec[4].hovering && mainMenu)
                increaseTrailSize();
            if (incDec[5].hovering && mainMenu)
                decreaseTrailSize();
            if (incDec[6].hovering && mainMenu)
                increaseResolution();
            if (incDec[7].hovering && mainMenu)
                decreaseResolution();

            startButton.mousePressed = true;
            for (auto &button : incDec)
                button.mousePressed = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            startButton.mousePressed = false;
            for (auto &button : incDec)
                button.mousePressed = false;
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

            startButton.key = event.key.keysym.sym;
            if (startButton.activateKey == startButton.key && mainMenu)
                startGame(window);

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
            startButton.key = SDLK_UNKNOWN;
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
        startButton.update(mouseX, mouseY);
        for (auto &button : incDec)
            button.update(mouseX, mouseY);
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
        startButton.display(*this);
        for (auto &button : incDec)
            button.display(*this);

        DrawString(std::to_string(playerCount), {-10 + window_width / 2.f, -13 - 20 + window_height / 2.f, 26, 26});
        DrawString(std::to_string((int)speed), {-18 + window_width / 2.f, 30 - 13 + window_height / 2.f, 26, 26});
        DrawString(std::to_string((int)cycles[0].maxTrailLength), {-26 + window_width / 2.f, 80 - 13 + window_height / 2.f, 26, 26});
        DrawString(std::string(std::to_string(res[currentRes].x) + "x" + std::to_string(res[currentRes].y)), {(currentRes <= 2 ? -60 : -68) + window_width / 2.f, 130 - 13 + window_height / 2.f, 26, 26});
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

void updateDims(int w, int h)
{
    short gw = w / (int)cycles[0].size;
    short gh = h / (int)cycles[0].size;

    cycles[0].iPos = Vec2s(gw - 20, gh - 10);
    cycles[0].ResetPosition();
    cycles[1].iPos = Vec2s(20, 10);
    cycles[1].ResetPosition();
    cycles[2].iPos = Vec2s(10, gh - 25);
    cycles[2].ResetPosition();
    cycles[3].iPos = Vec2s(gw - 10, 25);
    cycles[3].ResetPosition();

    startButton.changeDims({w / 2.f, -100 + h / 2.f, 120.f, 40.f});

    incDec[0].changeDims({25 + w / 2.f, -20 + h / 2.f, 30.f, 30.f});
    incDec[1].changeDims({-25 + w / 2.f, -20 + h / 2.f, 30.f, 30.f});
    incDec[2].changeDims({25 + w / 2.f, 30 + h / 2.f, 30.f, 30.f});
    incDec[3].changeDims({-25 + w / 2.f, 30 + h / 2.f, 30.f, 30.f});

    incDec[4].changeDims({32 + w / 2.f, 80 + h / 2.f, 30.f, 30.f});
    incDec[5].changeDims({-32 + w / 2.f, 80 + h / 2.f, 30.f, 30.f});

    incDec[6].changeDims({72 + w / 2.f, 130 + h / 2.f, 30.f, 30.f});
    incDec[7].changeDims({-72 + w / 2.f, 130 + h / 2.f, 30.f, 30.f});
}

void startGame(SDL_Window *window)
{
    mainMenu = false;
    SDL_SetTextureColorMod(bg.texture, 100, 100, 100);

    if (currentRes != prevRes)
    {
        SDL_SetWindowSize(window, res[currentRes].x, res[currentRes].y);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        prevRes = currentRes;
        updateDims(res[currentRes].x, res[currentRes].y);
    }
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

void increaseTrailSize()
{
    int l = cycles[0].maxTrailLength >= 980 ? 980 : cycles[0].maxTrailLength + 20;
    for (auto &c : cycles)
        c.maxTrailLength = l;
}

void decreaseTrailSize()
{
    int l = cycles[0].maxTrailLength <= 100 ? 100 : cycles[0].maxTrailLength - 20;
    for (auto &c : cycles)
        c.maxTrailLength = l;
}

void increaseResolution()
{
    currentRes >= 8 ? currentRes = 0 : currentRes += 1;
}

void decreaseResolution()
{
    currentRes <= 0 ? currentRes = 8 : currentRes -= 1;
}