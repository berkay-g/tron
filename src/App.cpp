#include "App.h"
#include "Font.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

App::App(const char *window_title, int window_width, int window_height, Uint32 sdl_init_flags, Uint32 window_flags, const char *renderer_name)
    : window_title(window_title), initial_window_width(window_width), initial_window_height(window_height), window_width(window_width), window_height(window_height), isMouseHidden(false)
{
    if (SDL_Init(sdl_init_flags) != 0)
    {
        SDL_Log("Failed to initialize SDL: %s\n", SDL_GetError());
        quit = true;
        return;
    }

    window = SDL_CreateWindow(window_title, initial_window_width, initial_window_height, window_flags);
    if (!window)
    {
        SDL_Log("Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        quit = true;
        return;
    }

    renderer = SDL_CreateRenderer(window, renderer_name, 0);
    if (!renderer)
    {
        SDL_Log("Failed to create SDL renderer: %s\n", SDL_GetError());
        renderer = SDL_CreateRenderer(window, NULL, 0);
        if (!renderer)
        {
            SDL_Log("Failed to create SDL renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            quit = true;
            return;
        }
    }
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    SDL_Log("Current SDL_Renderer: %s", info.name);

    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(display);
    if (displayMode)
    {
        SDL_Log("%s %dx%d", SDL_GetDisplayName(display), displayMode->w, displayMode->h);
        SDL_Rect rect = {0, 0, displayMode->w, displayMode->h};
        SDL_SetRenderViewport(renderer, &rect);
    }
    else
        SDL_SetRenderViewport(renderer, NULL);

    scale_factor_x = static_cast<float>(window_width) / initial_window_width;
    scale_factor_y = static_cast<float>(window_height) / initial_window_height;
    scale_factor = std::min(scale_factor_x, scale_factor_y);
    quit = false;

    SDL_Surface *surface;
    int32_t width, height, bytesPerPixel;
    void *data = stbi_load_from_memory(my_font, my_font_size, &width, &height, &bytesPerPixel, 0);
    if (!data)
        return;

    int pitch;
    pitch = width * bytesPerPixel;
    pitch = (pitch + 3) & ~3;

    surface = SDL_CreateSurfaceFrom(data, width, height, pitch, SDL_PIXELFORMAT_ARGB8888);
    font = SDL_CreateTextureFromSurface(renderer, surface);

    stbi_image_free(data);
    SDL_DestroySurface(surface);
}

App::~App()
{

#ifdef IMGUI
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif
    SDL_DestroyTexture(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#ifdef IMGUI

ImGuiIO &App::ImguiInit()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = NULL;
    io.LogFilename = NULL;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    return io;
}

ImGuiIO &App::ImguiNewFrame()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGuiIO &io = ImGui::GetIO();

    SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    return io;
}

void App::ImguiRender()
{
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
}

#endif

int App::SetWindowMinimumSize(int min_width, int min_height)
{
    return SDL_SetWindowMinimumSize(window, min_width, min_height);
}

void App::SetWindowWidthHeight(int w, int h)
{
    window_width = w;
    window_height = h;
    scale_factor_x = static_cast<float>(window_width) / initial_window_width;
    scale_factor_y = static_cast<float>(window_height) / initial_window_height;
    scale_factor = std::min(scale_factor_x, scale_factor_y);
}

void App::GetScaleFactors(float &x, float &y, float &factor)
{
    x = scale_factor_x;
    y = scale_factor_y;
    factor = scale_factor;
}

void App::HideMouseCursor()
{
    if (!isMouseHidden)
    {
        if (!SDL_HideCursor())
        {
            SDL_Log("%s", SDL_GetError());
            return;
        }
        isMouseHidden = true;
    }
}

void App::ShowMouseCursor()
{
    if (isMouseHidden)
    {
        if (SDL_ShowCursor())
        {
            SDL_Log("%s", SDL_GetError());
            return;
        }
        isMouseHidden = false;
    }
}

#ifdef IMGUI

void App::DrawString(const std::string &str, const ImVec2 &position, float size, const ImVec4 &color, int discriminator)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;

    ImGui::SetNextWindowPos(position);
    ImGui::Begin(std::string("String " + std::to_string(discriminator)).c_str(), nullptr, window_flags);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    float old_size = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= size / 100;
    ImGui::PushFont(ImGui::GetFont());
    ImGui::Text(str.c_str());
    ImGui::GetFont()->Scale = old_size;
    ImGui::PopFont();
    ImGui::PopStyleColor();
    ImGui::End();
}

#endif

void App::DrawString(const std::string &str, const SDL_FRect &dimensions)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == ' ')
            continue;
        auto idx = get2DIndex(str[i], 16);
        SDL_FRect rect = dimensions;
        rect.x += (rect.w * 0.65f) * i;
        SDL_FRect srcrect = {16.f * idx.second, 16.1f * idx.first, 15.f, 15.f};
        SDL_RenderTexture(renderer, font, &srcrect, &rect);
    }
}

void App::SetStringTextureColorMode(const Color &color)
{
    SDL_SetTextureColorMod(font, color.r, color.g, color.b);
}

const bool App::IsMouseInsideRect(float mouseX, float mouseY, const SDL_FRect &rect)
{
    return (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h);
}

int App::SetRenderDrawColor(Color color)
{
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

App::Texture::Texture(const char *filename, SDL_Renderer *renderer)
{
    SDL_Surface *surface = NULL;
    texture = NULL;
    int32_t width, height, bytesPerPixel;
    void *data = stbi_load(filename, &width, &height, &bytesPerPixel, 0);
    if (!data)
        return;

    int pitch;
    pitch = width * bytesPerPixel;
    pitch = (pitch + 3) & ~3;

    int32_t Rmask, Gmask, Bmask, Amask;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Rmask = 0x000000FF;
    Gmask = 0x0000FF00;
    Bmask = 0x00FF0000;
    Amask = (bytesPerPixel == 4) ? 0xFF000000 : 0;
#else
    int s = (bytesPerPixel == 4) ? 0 : 8;
    Rmask = 0xFF000000 >> s;
    Gmask = 0x00FF0000 >> s;
    Bmask = 0x0000FF00 >> s;
    Amask = 0x000000FF >> s;
#endif

    surface = SDL_CreateSurfaceFrom(data, width, height, pitch, SDL_GetPixelFormatEnumForMasks(bytesPerPixel * 8, Rmask, Gmask, Bmask, Amask));
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture)
        SDL_Log("Texture Failed to Load");

    stbi_image_free(data);
    SDL_DestroySurface(surface);
}

App::Texture::Texture(const unsigned char *imageData, int imageDataSize, SDL_Renderer *renderer)
{
    SDL_Surface *surface = NULL;
    texture = NULL;
    int32_t width, height, bytesPerPixel;
    void *data = stbi_load_from_memory(imageData, imageDataSize, &width, &height, &bytesPerPixel, 0);
    if (!data)
        return;

    int pitch;
    pitch = width * bytesPerPixel;
    pitch = (pitch + 3) & ~3;

    int32_t Rmask, Gmask, Bmask, Amask;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Rmask = 0x000000FF;
    Gmask = 0x0000FF00;
    Bmask = 0x00FF0000;
    Amask = (bytesPerPixel == 4) ? 0xFF000000 : 0;
#else
    int s = (bytesPerPixel == 4) ? 0 : 8;
    Rmask = 0xFF000000 >> s;
    Gmask = 0x00FF0000 >> s;
    Bmask = 0x0000FF00 >> s;
    Amask = 0x000000FF >> s;
#endif

    surface = SDL_CreateSurfaceFrom(data, width, height, pitch, SDL_GetPixelFormatEnumForMasks(bytesPerPixel * 8, Rmask, Gmask, Bmask, Amask));
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture)
        SDL_Log("Texture Failed to Load");

    stbi_image_free(data);
    SDL_DestroySurface(surface);
}

App::Texture::~Texture()
{
    SDL_DestroyTexture(texture);
    SDL_Log("Texture Destroyed");
}