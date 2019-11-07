/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: graphics.h
  Created: 2019-07-16
  Updated: 2019-11-06
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/
#include <stdint.h>

//! \file graphics.h
//! Drawing interface to the operating system.

#ifndef GRAPHICS_VERSION
#define GRAPHICS_VERSION "0.1.0" //!< include guard

#include "SDL2/SDL.h"

//! \brief Creates and initializes a new graphics object isntance
//!
//! Scale can be specified as a non-negative number. This value is used to
//! multiply both the width and the height and the pixel size of any drawing
//! operations.
//!
//! For example, specifying a scale of 2 would multiply the width by 2, the
//! height by 2, and every pixel would be 2x2; so the total scale factor ends up
//! being scale^2
//!
//! \param[in] title The title displayed in the window titlebar
//! \param[in] width Width of the display area of the window, in pixels
//! \param[in] height Height of the display are of the window, in pixels
//! \param[in] scale Size and rendering scale, natural number multiple
//! \return The initialized graphics object
struct graphics *
GraphicsInit(char *title, int width, int height);

//! \brief De-initializes and frees memory for the given graphics object
//! \param[in,out] graphics The initialized opcode object to be cleaned and reclaimed
void
GraphicsDeinit(struct graphics *graphics);

//! \brief Initializes the graphics subsystem for drawing routines
//!
//! Internally locks streaming texture for direct manipulation.
//!
//! \param[in, out] graphics Graphics state to be manipulated
void
GraphicsBegin(struct graphics *graphics);

//! \brief Prepares the graphics subsystem for presentation, then presents
//!
//! Internally unlocks streaming texture then calls presentation routines.
//!
//! \param[in, out] graphics Graphics state to be manipulated.
void
GraphicsEnd(struct graphics *graphics);

//! \brief Sets all pixels in the screen to the given color
//!
//! \param[in, out] graphics Graphics state to be manipulated
//! \param[in] color 32-bit color with 8-bits per component: (R,G,B,A)
void
GraphicsClearScreen(struct graphics *graphics, uint32_t color);

//! \brief Put a pixel into the graphics buffer
//!
//! \param[in,out] graphics Graphics state to be manipulated
//! \param[in] x horizontal position in display buffer (assuming no scaling)
//! \param[in] y vertical position in display buffer (assuming no scaling)
//! \param[in] color Color to put into display buffer
void
GraphicsPutPixel(struct graphics *graphics, int x, int y, uint32_t color);

uint32_t
GraphicsGetPixel(struct graphics *graphics, int x, int y);

void
GraphicsInitText(struct graphics *graphics, unsigned char *ttfBuffer);

void
GraphicsDrawText(struct graphics *graphics, int x, int y, char *string, int pixHeight, uint32_t color);

#endif // GRAPHICS_VERSION
