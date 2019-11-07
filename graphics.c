/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: graphics.c
  Created: 2019-06-25
  Updated: 2019-11-06
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file graphics.c

#include <string.h> // memset
#include <stdio.h> // fprintf

#include "SDL2/SDL.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "math.h"
#include "graphics.h"

//! \brief A mostly generic implementation of swap
//!
//! Both v1 and v2 must point to data that is the same size, as specified in the
//! size parameter.
//!
//! \param[in,out] v1 first value
//! \param[in,out] v2 second value
//! \param[in] size v1 and v2 must each be this size
//!
void swap_generic(void *v1, void *v2, size_t size) {
        char temp[size];
        memmove(temp, v1, size);
        memmove(v1, v2, size);
        memmove(v2, temp, size);
}

//! \brief Graphics state
struct graphics {
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        unsigned int width;
        unsigned int height;

        uint8_t *pixels;
        int bytesPerRow;
        stbtt_fontinfo fontInfo;
};

struct graphics *GraphicsInit(char *title, int width, int height) {
        struct graphics *g = (struct graphics *)malloc(sizeof(struct graphics));
        memset(g, 0, sizeof(struct graphics));

        g->width = width;
        g->height = height;

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);

        g->window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                g->width, g->height,
                SDL_WINDOW_SHOWN
        );
        if (NULL == g->window) {
                fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
                GraphicsDeinit(g);
                return NULL;
        }

        g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_SOFTWARE);
        if (NULL == g->renderer) {
                fprintf(stderr, "Couldn't create renderer: %s\n", SDL_GetError());
                GraphicsDeinit(g);
                return NULL;
        }

        g->texture = SDL_CreateTexture(g->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (NULL == g->texture) {
                fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
                GraphicsDeinit(g);
                return NULL;
        }

        return g;
}

void GraphicsDeinit(struct graphics *g) {
        if (NULL == g) {
                return;
        }

        if (NULL != g->texture) {
                SDL_DestroyTexture(g->texture);
        }

        if (NULL != g->renderer) {
                SDL_DestroyRenderer(g->renderer);
        }

        if (NULL != g->window) {
                SDL_DestroyWindow(g->window);
        }

        SDL_Quit();
        free(g);
}

void GraphicsBegin(struct graphics *graphics) {
        SDL_LockTexture(graphics->texture, NULL, (void **)&graphics->pixels, &graphics->bytesPerRow);
}

void GraphicsEnd(struct graphics *graphics) {
        SDL_UnlockTexture(graphics->texture);
        SDL_RenderClear(graphics->renderer);
        SDL_RenderCopy(graphics->renderer, graphics->texture, 0, 0);
        SDL_RenderPresent(graphics->renderer);
}

void GraphicsClearScreen(struct graphics *graphics, unsigned int color) {
        for (int i = 0; i < graphics->bytesPerRow * graphics->height; i+=4) {
                unsigned int *pixel = (unsigned int *)&graphics->pixels[i];
                *pixel = color;
        }
}

float ByteToFloat(uint8_t value) {
        return (float)value / 255.0;
}

float AlphaBlendComponent(float c1, float a1, float c2, float a2) {
        float dividend = (c1 * a1) + ((c2 * a2) * (1.0f - a1));
        float divisor = a1 + (a2 * (1.0f - a1));
        return dividend / divisor;
}

uint32_t AlphaBlendPixels(uint32_t top, uint32_t bottom) {
        float bottomRed   = ByteToFloat((bottom >> 24) & 0xFF);
        float bottomGreen = ByteToFloat((bottom >> 16) & 0xFF);
        float bottomBlue  = ByteToFloat((bottom >>  8) & 0xFF);
        float bottomAlpha = ByteToFloat((bottom >>  0) & 0xFF);

        float topRed   = ByteToFloat((top >> 24) & 0xFF);
        float topGreen = ByteToFloat((top >> 16) & 0xFF);
        float topBlue  = ByteToFloat((top >>  8) & 0xFF);
        float topAlpha = ByteToFloat((top >>  0) & 0xFF);

        float newRed   = AlphaBlendComponent(topRed,   topAlpha, bottomRed,   bottomAlpha);
        float newGreen = AlphaBlendComponent(topGreen, topAlpha, bottomGreen, bottomAlpha);
        float newBlue  = AlphaBlendComponent(topBlue,  topAlpha, bottomBlue,  bottomAlpha);

        uint32_t newColor = (
                ((uint8_t)(newRed   * 255.0f) << 24) |
                ((uint8_t)(newGreen * 255.0f) << 16) |
                ((uint8_t)(newBlue  * 255.0f) <<  8) |
                0xFF);

        return newColor;
}

//! \brief Put a pixel into the graphics buffer
//!
//! \param[in,out] graphics Graphics state to be manipulated
//! \param[in] x horizontal position in display buffer (assuming no scaling)
//! \param[in] y vertical position in display buffer (assuming no scaling)
//! \param[in] color Color to put into display buffer
void GraphicsPutPixel(struct graphics *graphics, int x, int y, unsigned int color) {
        if (x >= 0 && x < graphics->width && y >= 0 && y < graphics->height) {
                uint8_t opacity = color & 0xFF;
                if (0 == opacity) {
                        return;
                }

                uint32_t pixel = GraphicsGetPixel(graphics, x, y);
                uint32_t newPixel = AlphaBlendPixels(color, pixel);

                int y_flipped = (graphics->height - y - 1);
                uint32_t *screen = (uint32_t *)&graphics->pixels[y_flipped * graphics->bytesPerRow + x * 4];
                *screen = newPixel;
        }
}

uint32_t GraphicsGetPixel(struct graphics *graphics, int x, int y) {
        if (x >= 0 && x < graphics->width && y >= 0 && y < graphics->height) {
                return *(uint32_t *)&graphics->pixels[y * graphics->bytesPerRow + x * 4];
        }

        return 0x00000000;
}

//! \brief Draws a line from (x1,y1) to (x2,y2)
//!
//! Used by GraphicsTriangleWireframe() and GraphicsTriangleSolid()
//!
//! \param[in,out] graphics Graphics state to be manipulated
//! \param[in] x1 horizontal position of the line start.
//! \param[in] y1 vertical position of the line start.
//! \param[in] x2 horizontal position of the line end.
//! \param[in] y2 vertical position of the line end.
//! \param[in] color color to render the line with.
void GraphicsDrawLine(struct graphics *graphics, int x1, int y1, int x2, int y2, unsigned int color) {
        int dx = x2 - x1;
        int dy = y2 - y1;

        int dx1 = abs(dx);
        int dy1 = abs(dy);

        int px = 2 * dy1 - dx1;
        int py = 2 * dx1 - dy1;

        int x, y, xe, ye;

        if (dy1 <= dx1) {
                // Line is horizontal.
                if (dx >= 0) {
                        x = x1;
                        y = y1;
                        xe = x2;
                } else {
                        x = x2;
                        y = y2;
                        xe = x1;
                }

                GraphicsPutPixel(graphics, x, y, color);

                for (int i = 0; x < xe; i++) {
                        x = x + 1;
                        if (px < 0) {
                                px = px + 2 * dy1;
                        } else {
                                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                                        y = y + 1;
                                } else {
                                        y = y - 1;
                                }
                                px = px + 2 * (dy1 - dx1);
                        }
                        GraphicsPutPixel(graphics, x, y, color);
                }
        } else {
                // Line is vertical.
                if (dy >= 0) {
                        x = x1;
                        y = y1;
                        ye = y2;
                } else {
                        x = x2;
                        y = y2;
                        ye = y1;
                }

                GraphicsPutPixel(graphics, x, y, color);

                for (int i = 0; y < ye; i++) {
                        y = y + 1;
                        if (py <= 0) {
                                py = py + 2 * dx1;
                        } else {
                                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                                        x = x + 1;
                                } else {
                                        x = x - 1;
                                }
                                py = py + 2 * (dx1 - dy1);
                        }
                        GraphicsPutPixel(graphics, x, y, color);
                }
        }
}

//! Used internally by GraphicsTriangleSolid()
void TriangleSolidDrawLine(struct graphics *graphics, int xmin, int xmax, int y, unsigned int color) {
        for (int i = xmin; i <= xmax; i++) {
                GraphicsPutPixel(graphics, i, y, color);
        }
}

void GraphicsInitText(struct graphics *graphics, unsigned char *ttfBuffer) {
        if (NULL == ttfBuffer) {
                // TODO: error handling
                return;
        }

        stbtt_InitFont(&graphics->fontInfo, (const unsigned char *)ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));
}

void GraphicsDrawText(struct graphics *graphics, int x, int y, char *string, int pixHeight, uint32_t color) {
        int len = strlen(string);
        int advanceWidths[len + 1];
        advanceWidths[0] = 0;

        float scale = stbtt_ScaleForPixelHeight(&graphics->fontInfo, pixHeight);

        // Render the text.
        int xOffset = 0;
        for (int c = 0; c < len; c++) {
                stbtt_GetCodepointHMetrics(&graphics->fontInfo, string[c], &advanceWidths[c+1], NULL);

                int x0, x1, y0, y1;
                stbtt_GetCodepointBitmapBox(&graphics->fontInfo, string[c], scale, scale, &x0, &y0, &x1, &y1);

                int width, height;
                uint8_t *bitmap = stbtt_GetCodepointBitmap(&graphics->fontInfo, scale, scale, string[c], &width, &height, 0, 0);

                xOffset += (int)((float)(advanceWidths[c]) * scale);

                // Draw the character.
                for (int h = 0; h < height; h++) {
                        for (int w = 0; w < width; w++) {
                                uint8_t opacity = bitmap[h * width + w];
                                color = (color & 0xFFFFFF00) | opacity;
                                GraphicsPutPixel(graphics, x + xOffset + (w + x0), y - (h + y0), color);
                        }
                }
        }
}
