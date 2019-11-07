# GrooveStomp's Text Renderer
[![GPLv3 license](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://raw.githubusercontent.com/GrooveStomp/gstxt/master/LICENSE) [![Reference Documentation](https://img.shields.io/static/v1?label=c&message=documentation&color=brightgreen)](https://groovestomp.github.io/gstxt)

This is a small demonstration program I put together to understand how to use [stb_truetype.h](https://github.com/nothings/stb).
It creates a window and renders the `text` parameter to the window based on the parameters specified.

# Development
## Requirements
- make
- gcc
- sdl2
- doxygen (For documentation generation)

This is developed for Linux and no effort has been made to support it elsewhere.
That said, with the exception of `main.c` using [getopt.h](https://www.gnu.org/software/libc/manual/html_node/Getopt.html#Getopt) for arg parsing and `time.h` for `nanosleep()`, I think the rest of the code should compile with little to no issue on other platforms.
I would expect this to build on OSX with little issue.

## Building
There are four targets in the `Makefile`:
- `clean`
- `debug`
- `release`
- `docs`

The default target is `release`.
`release` builds `gstxt` at `release/gstxt`.
`debug` builds `gstxt` at `debug/gsxtxt`.
`docs` builds the documentation with Doxygen.

# Usage
There is help built into the program. Invoke the program with `-h` or `--help`:
```
debug/gstxt -h
Usage: gstxt text [OPTIONS] ttf_file

Renders text on screen in software.

OPTIONS:
    --help|-h:   Show this help text
    --width|-w:  Screen width (defaults to 600)
    --height|-v: Screen height (defaults to 300)p
    --x:         Text x position (defaults to 20)
    --y:         Text y position (defaults to 150)
    --scale|-s:  Scale of text (defaults to 40)
    --fg|-f:     Foreground color as a 32-bit hexadecimal RGBA value (defaults to 0x000000FF)
    --bg|-b:     Background color as a 32-bit hexadecimal RGBA value (defaults to 0xFFFFFFFF)
```

Here's a sample invocation that changes several attributes of the resulting image:
```
debug/gstxt "a quick brown fox jumped" /usr/share/fonts/truetype/noto/NotoMono-Regular.ttf -f 0xFF00FFFF -b 0x000000ff -v 150 -y 50 -s 80 -w 1000
```
`ESC` quits the program, as does closing the window.