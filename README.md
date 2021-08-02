# Conductor2 (WIP)

## This project is a (re)build of my LEGO train control system.

#

I replaced train's infra-red control system with an ESP32 using Arduino C++.  It makes an internet connection using WiFi (initially configured through a captive portal), and is controlled via pub-sub message (throw away keys right now).

I am currently rebuilding everything from scratch, so things are not all there yet, and are in `debug` and `todo` modes.

More to come...

#

## Train Controller
- [Code](https://github.com/aihrig/Conductor2/tree/master/train-controller/main)
- Future Items:
  - [x] Load configuration values from SPIFFS (ESP32 File System)
  - [ ] Set direction forward/back (in progress)
  - [ ] Set speed (in progress)
  - [x] Headlights on/off
  - [ ] Headlights alternating pattern(s)
- Images
![train_open_cropped](https://user-images.githubusercontent.com/12666421/127811045-a784a203-ba51-4db9-9185-d2d95f510b1f.jpg)
![train_closed_cropped](https://user-images.githubusercontent.com/12666421/127811052-e8a602e8-07c4-462c-a379-3ff2ccb4d792.jpg)

## Track Controller
- Code (coming soon)
- Images (coming soon)

## City Controller
- Code (coming soon)
- Images (coming soon)
