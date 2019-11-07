#include <time.h> // nanosleep
#include <stdlib.h> // exit
#include <stdio.h> // fopen, fclose, printf, fprintf
#include <getopt.h> // getopt

#include "stb_truetype.h"

#include "graphics.h"
#include "input.h"

#define S_TO_NS(x) (x) * 1000000000

void usage(char *prog) {
        printf("Usage: %s text [OPTIONS] ttf_file\n", prog);
        printf("\n");
        printf("Renders text on screen in software.\n");
        printf("\n");
        printf("OPTIONS:\n");
        printf("    --help|-h:   Show this help text\n");
        printf("    --width|-w:  Screen width (defaults to 600)\n");
        printf("    --height|-v: Screen height (defaults to 300)p\n");
        printf("    --x:         Text x position (defaults to 20)\n");
        printf("    --y:         Text y position (defaults to 150)\n");
        printf("    --scale|-s:  Scale of text (defaults to 40)\n");
}

static unsigned char ttf_buffer[1<<25];
struct graphics *graphics = NULL;
struct input *input = NULL;
FILE *ttf_file = NULL;

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

        while (1) {
                static struct option long_options[] = {
                        { "help", no_argument, NULL, 'h' },
                        { "width", required_argument, NULL, 'w' },
                        { "height", required_argument, NULL, 'v' },
                        { "x", required_argument, NULL, 'x' },
                        { "y", required_argument, NULL, 'y' },
                        { "scale", required_argument, NULL, 's' }
                };
                int option_index = 0;

                int c = getopt_long(argc, argv, "h,w:v:x:y:s:", long_options, &option_index);
                if (c == -1)
                        break;

                switch (c) {
                        case 'h': {
                                usage(argv[0]);
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
                        default:
                                abort();
                }
        }

        if ((argc - optind) != 2) {
                usage(argv[0]);
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

        SDL_Event event;
        int running = 1;
        while (running) {
                GraphicsBegin(graphics);
                GraphicsClearScreen(graphics, 0xFFFFFFFF);
                GraphicsDrawText(graphics, x, y, string_to_render, scale, 0x0000FFFF);
                GraphicsEnd(graphics);

                while (SDL_PollEvent(&event)) {
                        running = !InputIsQuitPressed(&event);
                }

                nanosleep(&sleep, NULL);
        }

        Shutdown(0);
        return 0;
}
