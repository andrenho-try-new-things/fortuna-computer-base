## Version 0.1

- [x] VGA simple framebuffer (640x480)
  - [x] Remove excess code
  - [x] Draw single pixel
- [x] Font generator
  - [x] Application to generate font to .h
- [x] Text framebuffer + color
  - [x] Draw characters
  - [x] Framebuffer
  - [x] Use callbacks to blink cursor
  - [x] Many chars
  - [x] Deal with ENTER
  - [x] Line feed
- [x] Better font (deal with multiple fonts)
- [x] Schematic

## Version 0.2

- [x] USB keyboard
- [x] Add version number
- [x] Add copyright
- [x] Test application
  - [x] cls
  - [x] bug with set_cursor
  - [x] longtext
  - [x] font
- [x] Finalize font
  - [x] boxes

## Version 0.3

- [x] SD Card
- [x] UART
- [x] Extra button / LED
- [x] Real-time clock + battery

## Version 0.4

- [x] Transform in library
- [x] Extra SPI port
- [x] Create examples
  - [x] Demo

## Version 1.0

- [x] Audio (?)
  - [x] All notes
- [x] WiFi/Bluetooth
- [x] Write documentation / rename

## Version 2.0

- [x] Configurable initialization
- [x] General code review
- [x] Print total RAM usage
- [x] User panel
  - [x] Detect changes to dipswitch
- [x] VGA improvements
  - [x] Text mode
    - [x] Don't use text backend (less memory)
    - [x] 4 different fonts

## Version 2.1

- [x] VGA mode 2 at lower resolution (320x240)
  - [ ] 640x240 mode
  - [ ] Drawing primitives
  - [ ] Tiles and sprites
  - [ ] Optional double buffer
- [ ] USB
  - [ ] Mouse
  - [ ] Joystick

## Version 2.2

- [ ] Advanced audio
  - [ ] Multiple channels (?)
  - [ ] Samples using PIO (?)
  - [ ] Noise
- [ ] Github actions - generate uf2