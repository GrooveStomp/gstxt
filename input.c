/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: input.c
  Created: 2019-06-21
  Updated: 2019-11-06
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file input.c

#include "SDL2/SDL.h"

#include "math.h"

//! \brief Keypress state. Unexported.
struct input {
        const unsigned char *sdlKeyStates;
};

struct input *InputInit() {
        struct input *i = (struct input *)malloc(sizeof(struct input));
        memset(i, 0, sizeof(struct input));

        i->sdlKeyStates = SDL_GetKeyboardState(NULL);

        return i;
}

void InputDeinit(struct input *i) {
        if (NULL == i)
                return;

        free(i);
}

int InputIsQuitPressed(SDL_Event *event) {
        switch (event->type) {
                case SDL_QUIT:
                        return 1;
                        break;

                case SDL_KEYUP:
                        break;

                case SDL_KEYDOWN:
                        if (event->key.keysym.sym == SDLK_ESCAPE) {
                                return 1;
                                break;
                        }
                        break;
        }

        return 0;
}
