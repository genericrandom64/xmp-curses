# xmp-curses

libxmp frontend that uses curses.

## ui

```
PL
00:01:19
```

P - displayed when player is paused

L - displayed when loop is on

## keybinds

q - quit xmp-curses

L - loop module

j/k - select module

h/l - rewind/forward

space - play/pause

## backends

pulseaudio - `backend/pulseaudio.h`, link `-lpulse-simple`. tested on linux

sndio - `backend/sndio.h`, link `-lsndio`. tested on openbsd

## cli arguments

-h - Help

-i [nearest|spline] - Change default interpolation (linear) to one of these

-s [0-100] - stereo separation in percent

-l - Loop

## future plans

- audio backend for freebsd (oss?)

- audio backend using a library (macos and windows)

- playlists as argument or loaded from stdin (modules dont seem to load from stdin)

- full tui

## license (or rather unlicense)

if for some reason you actually *want* this code, it is public domain under the unlicense. if this changes, it will remain permissively licensed.
