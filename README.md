
# Glyph Reactor

[video](https://www.youtube.com/watch?v=GX5vAXJlMV0&feature=youtu.be)

Select a course from the main menu (move the cursor with 'u' or 'd' or the arrow keys, then select with 'Enter'), then type in the words that you see, separated by a space. If you enter any wrong characters, erase them with backspace.

![screenshot](https://github.com/JonArintok/GlyphReactor/blob/master/img/screenshot04.png)

You can exit a course by pressing the 'esc' key. Pressing the 'esc' key from the menu will close the window.

Pressing the 'tab' key from the menu will enter a mode where you can just enter any character to see the effects, then you can exit this mode be pressing 'esc' or 'tab' again.

![screenshot](https://github.com/JonArintok/GlyphReactor/blob/master/img/screenshot06.png)


Toggle full-screen with 'F11'.

Courses can be added by simply placing text files in the "courses" folder.
Only ASCII characters are supported.

To build from source, you will need Scons, a C compiler(like GCC), and the following libs
- SDL2
- GLEW
- libpng

Run `git submodule init` then `git submodule update` for [libVoice](https://github.com/JonArintok/libVoice), then run `scons` to build. The output will be named "GlyphReactor".

