# xmp-curses

libxmp frontend that uses curses.

## ui

```
PLR
00:01:19
```

P - displayed when player is paused

L - displayed when single module loop is on

R - displayed when all module loop is on

## keybinds

q - quit xmp-curses

L - loop module

R - loop all modules

j/k - select module

h/l - rewind/forward

space - play/pause

## backends

pulseaudio - `backend/pulseaudio.h`, link `-lpulse-simple`, tested on linux

sndio - `backend/sndio.h`, link `-lsndio`, tested on openbsd

## cli arguments

-h - show help message

-i [nearest|spline] - hhange default interpolation (linear) to one of these

-s [0-100] - stereo separation in percent

-l - loop over one module

-R - loop over all modules selected

## future plans

- audio backend for freebsd (oss?)

- audio backend using a library (macos and windows)

- playlist files as argument or loaded from stdin (modules dont seem to load from stdin)

- full tui

## license (or rather unlicense)

if for some reason you actually *want* this code, it is public domain under the unlicense. if this changes, it will remain permissively licensed.
