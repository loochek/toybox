#include <stdexcept>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "SDLGraphics.hpp"
#include <SDL2/SDL_ttf.h>

static const float ARROW_WIDTH  = 7.0f;
static const float ARROW_HEIGHT = 10.0f;

const char *FONT_FILE_NAME = "Roboto-Light.ttf";

Graphics::Graphics(const Vec2i &resolution, const char *title)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              resolution.x, resolution.y, SDL_WINDOW_VULKAN);
    if (window == nullptr)
        throw std::runtime_error("Unable to init SDL");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (window == nullptr)
        throw std::runtime_error("SDL: unable to create renderer");

    sdlTicks = SDL_GetTicks();
}

Graphics::~Graphics()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::drawTriangle(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Color &color)
{
    const Sint16 vx[] = { (Sint16)p1.x, (Sint16)p2.x, (Sint16)p3.x };
    const Sint16 vy[] = { (Sint16)p1.y, (Sint16)p2.y, (Sint16)p3.y };

    filledPolygonRGBA(renderer, vx, vy, 3,
                      color.x * 255, color.y * 255, color.z * 255, 255);
}

void Graphics::drawQuad(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4, const Color &color)
{
    const Sint16 vx[] = { (Sint16)p1.x, (Sint16)p2.x, (Sint16)p3.x, (Sint16)p4.x };
    const Sint16 vy[] = { (Sint16)p1.y, (Sint16)p2.y, (Sint16)p3.y, (Sint16)p4.y };

    filledPolygonRGBA(renderer, vx, vy, 4,
                      color.x * 255, color.y * 255, color.z * 255, 255);
}

void Graphics::drawRect(const Rect &rect, const Color &color)
{
    Vec2f p1 = rect.position;
    Vec2f p2 = rect.position + Vec2f(rect.size.x, 0);
    Vec2f p3 = rect.position + rect.size;
    Vec2f p4 = rect.position + Vec2f(0, rect.size.y);

    drawQuad(p1, p2, p3, p4, color);
}

void Graphics::drawCircle(const Vec2f &position, float radius, const Color &color)
{
    filledCircleRGBA(renderer, position.x, position.y, radius,
                     color.x * 255, color.y * 255, color.z * 255, 255);
}

void Graphics::drawLine(const Vec2f &p1, const Vec2f &p2, float thickness, const Color &color)
{
    Vec2f line_vec = p2 - p1;
    Vec2f normal = Vec2f(-line_vec.y, line_vec.x) / line_vec.length();

    Vec2f vert1 = p1 - normal * thickness;
    Vec2f vert2 = p1 + normal * thickness;
    Vec2f vert3 = p2 + normal * thickness;
    Vec2f vert4 = p2 - normal * thickness;
    
    const Sint16 vx[] = { (Sint16)vert1.x, (Sint16)vert2.x, (Sint16)vert3.x, (Sint16)vert4.x };
    const Sint16 vy[] = { (Sint16)vert1.y, (Sint16)vert2.y, (Sint16)vert3.y, (Sint16)vert4.y };

    filledPolygonRGBA(renderer, vx, vy, 4,
                      color.x * 255, color.y * 255, color.z * 255, 255);
}

void Graphics::drawArrow(const Vec2f &start, const Vec2f &end, float thickness, const Color &color)
{
    drawLine(start, end, thickness, color);

    Vec2f line_vec_norm = (end - start).normalized();
    Vec2f normal = Vec2f(-line_vec_norm.y, line_vec_norm.x);

    Vec2f vert1 = end - line_vec_norm * ARROW_HEIGHT - normal * ARROW_WIDTH;
    Vec2f vert2 = end - line_vec_norm * ARROW_HEIGHT + normal * ARROW_WIDTH;

    const Sint16 vx[] = { (Sint16)end.x, (Sint16)vert1.x, (Sint16)vert2.x };
    const Sint16 vy[] = { (Sint16)end.y, (Sint16)vert1.y, (Sint16)vert2.y };

    filledPolygonRGBA(renderer, vx, vy, 3,
                      color.x * 255, color.y * 255, color.z * 255, 255);
}

void Graphics::drawText(const Vec2f &position, const char *text, TextOrigin textOrigin,
                        const Color &color, int size)
{
    TTF_Font *font = TTF_OpenFont(FONT_FILE_NAME, size);
    if (font == NULL)
        throw std::runtime_error("Unable to load font");

    SDL_Color sdlColor = {0};
    sdlColor.r = color.x * 255;
    sdlColor.g = color.y * 255;
    sdlColor.b = color.z * 255;
    sdlColor.a = 255;

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, sdlColor);
    if (font == NULL)
    {
        TTF_CloseFont(font);
        throw std::runtime_error("Unable to render text");
    }
        
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (texture == NULL)
    {
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        throw std::runtime_error("Unable to create texture");
    }

    int textWidth = 0, textHeight = 0;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

    SDL_Rect srcRect = {0};
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = textWidth;
    srcRect.h = textHeight;

    SDL_Rect dstRect = {0};
    dstRect.x = position.x;
    dstRect.y = position.y;
    dstRect.w = textWidth;
    dstRect.h = textHeight;

    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

void Graphics::clear(const Color &clearColor)
{
    SDL_SetRenderDrawColor(renderer, clearColor.x * 255, clearColor.y * 255, clearColor.z * 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void Graphics::display()
{
    SDL_RenderPresent(renderer);
}

bool Graphics::shouldClose()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            return true;
    }

    return false;
}

float Graphics::timerReset()
{
    int currTicks = SDL_GetTicks();;
    int elapsedTicks = currTicks - sdlTicks;
    sdlTicks = currTicks;

    return elapsedTicks / 1000.0f;;
}

bool Graphics::isLeftMouseButtonPressed()
{
    int buttonState = SDL_GetMouseState(nullptr, nullptr);
    return buttonState && SDL_BUTTON_LMASK;
}

bool Graphics::isRightMouseButtonPressed()
{
    int buttonState = SDL_GetMouseState(nullptr, nullptr);
    return buttonState && SDL_BUTTON_RMASK;
}

Vec2f Graphics::getWindowMousePosition()
{
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    return Vec2f(x, y);
}