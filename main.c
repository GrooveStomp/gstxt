/******************************************************************************
  GrooveStomp's Text Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: main.c
  Created: 2019-11-06
  Updated: 2019-11-07
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/
//! \file main.c
#include <time.h> // nanosleep
#include <stdlib.h> // exit
#include <stdio.h> // fopen, fclose, printf, fprintf
#include <getopt.h> // getopt

#include "graphics.h"
#include "input.h"

//! \brief convert from seconds to nanoseconds
//!
//! Used to make it easier to understand the value being passed to nanosleep()
//!
//! \param x number of seconds
//! \return x as nanoseconds
#define S_TO_NS(x) (x) * 1000000000

void usage() {
        printf("Usage: gstxt text [OPTIONS] ttf_file\n");
        printf("\n");
        printf("Renders text on screen in software.\n");
        printf("\n");
        printf("OPTIONS:\n");
        printf("    --help|-h:   Show this help text\n");
        printf("    --width|-w:  Screen width (defaults to 600)\n");
        printf("    --height|-v: Screen height (defaults to 300)\n");
        printf("    --x:         Text x position (defaults to 20)\n");
        printf("    --y:         Text y position (defaults to 150)\n");
        printf("    --scale|-s:  Scale of text (defaults to 40)\n");
        printf("    --fg|-f:     Foreground color as a 32-bit hexadecimal RGBA value (defaults to 0x000000FF)\n");
        printf("    --bg|-b:     Background color as a 32-bit hexadecimal RGBA value (defaults to 0xFFFFFFFF)\n");
}


static unsigned char ttf_buffer[1<<25]; //!< TrueType font file data
static struct graphics *graphics = NULL; //!< graphics subsystem state
static struct input *input = NULL; //!< input subsystem state
static FILE *ttf_file = NULL; //!< file handle to TrueType font file

//! \brief Gracefully release all resources in use
//!
//! Complex program states are maintained in global variables in main.c so we can
//! uniformly handle them all, regardless of which ones have been properly
//! initialized yet or where.
//!
//! \param code the exit code to terminate with, passed to exit()
void Shutdown(int code) {
        if (NULL != ttf_file)
                fclose(ttf_file);
        if (NULL != input)
                InputDeinit(input);
        if (NULL != graphics)
                GraphicsDeinit(graphics);

        exit(code);
}

int main(int argc, char **argv) {
        unsigned int width = 600;
        unsigned int height = 300;
        unsigned int x = 20;
        unsigned int y = 150;
        unsigned int scale = 40;
        uint32_t fg = 0x000000FF;
        uint32_t bg = 0xFFFFFFFF;

        while (1) {
                static struct option long_options[] = {
                        { "help", no_argument, NULL, 'h' },
                        { "width", required_argument, NULL, 'w' },
                        { "height", required_argument, NULL, 'v' },
                        { "x", required_argument, NULL, 'x' },
                        { "y", required_argument, NULL, 'y' },
                        { "scale", required_argument, NULL, 's' },
                        { "fg", required_argument, NULL, 'f' },
                        { "bg", required_argument, NULL, 'b' },
                };
                int option_index = 0;

                int c = getopt_long(argc, argv, "hw:v:x:y:s:b:f:", long_options, &option_index);
                if (c == -1)
                        break;

                switch (c) {
                        case 'h': {
                                usage();
                                Shutdown(0);
                        }
                        case 'w': {
                                width = atoi(optarg);
                                break;
                        }
                        case 'v': {
                                height = atoi(optarg);
                                break;
                        }
                        case 'x': {
                                x = atoi(optarg);
                                break;
                        }
                        case 'y': {
                                y = atoi(optarg);
                                break;
                        }
                        case 's': {
                                scale = atoi(optarg);
                                break;
                        }
                        case 'b': {
                                bg = strtol(optarg, NULL, 16);
                                break;
                        }
                        case 'f': {
                                fg = strtol(optarg, NULL, 16);
                                break;
                        }
                        default:
                                abort();
                }
        }

        if ((argc - optind) != 2) {
                usage();
                Shutdown(0);
        }

        char *string_to_render = argv[optind++];
        char *ttf_filename = argv[optind++];
        struct timespec sleep = { .tv_sec = 0, .tv_nsec = S_TO_NS(0.0333) };

        graphics = GraphicsInit("GrooveStomp's Text Renderer", width, height);
        if (NULL == graphics) {
                fprintf(stderr, "Couldn't initialize graphics");
                Shutdown(1);
        }

        input = InputInit();
        if (NULL == input) {
                fprintf(stderr, "Couldn't initialize input");
                Shutdown(1);
        }

        ttf_file = fopen(ttf_filename, "rb");
        if (NULL == ttf_file) {
                perror("fopen() failed");
                Shutdown(1);
        }

        int objs_read = fread(ttf_buffer, 1, 1<<25, ttf_file);
        if (objs_read != 1<<25 && ferror(ttf_file)) {
                perror("fread() failed");
                Shutdown(1);
        }
        fclose(ttf_file); ttf_file = NULL;
        GraphicsInitText(graphics, ttf_buffer);

        int running = 1;
        while (running) {
                GraphicsBegin(graphics);
                GraphicsClearScreen(graphics, bg);
                GraphicsDrawText(graphics, x, y, string_to_render, scale, fg);
                GraphicsEnd(graphics);

                InputProcess(input);
                running = !InputIsQuitRequested(input);

                nanosleep(&sleep, NULL);
        }

        Shutdown(0);
        return 0;
}
