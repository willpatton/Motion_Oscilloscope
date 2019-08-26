Motion Audio Computer/Oscilloscope
===========
This project is an audio analyzer and/or oscillosope used to visualize waveforms, make measurements, and detect events. It samples audio signals using a processor's analog-to-digital (ADC) to sample waveforms.  Waveforms can then be analyzed, displayed, or recorded.

## Applications

Audio Computer
Audio Oscilloscope
VU/Loudness Meter
Spectrum Analyzer
Sound Clip Capture
IoT Controller

FEATURES
* Screen 256x64 16K pixels
* Samples 12-bit resolution, 256 to 1024 samples, 20Hz refresh
* Vert, Horiz automatic settings optimized for audio 
* Trigger (auto, normal, rising, falling)
* Cursors XY (dotted, dashed). Automatic. Persistence effects. 
* Measurements (avg, peak, peak-to-peak, RMS, dBV, dBu, more...)
* Units Verticle: volts, dB 

ADVANCED 
* 3 Channels (left, right, sub-mono)
* Measurements (THD%)
* Calibration
* Automatic Gain Control (AGC)
* Active Filter Bandwidth (anti-aliasing)
* Events (sound, BPM, controller)
* Effects (persistence)
* Sound presence detection, BPM beats-per-minute detection
* Profiles for voice, instruments, BPM
* Microphone

CLOUD
* Capture and upload sound clips for signature matching
* View/Analyze downloaded sound clips
* Audio of Ethernet (AoE)


## Example
An example program is included in the examples folder.  
A basic oscilloscope screen consists of these capabilities:
* Waveform
* Verticle volts per division
* Horizontial time per division
* Measurements - peak-to-peak
* Trigger on zero crossing, rising edge
* Refresh rate

## Screen

256x64, 16K pixels
+--------------------------------------------------------+
|1.25Vpp                                            21 Hz| measurements, refresh rate
|-                  ___              _   . . . . . . . . | cursors
|                  /   \            / \                  | 
|-/\  ____________/     \       /\_/   \_/\  ___________ | waveform - 0V GND
|   \/                   \     /           \/            |
|-	                      \___/                          |
|1V/div                           	              1ms/div| controls
+--------------------------------------------------------+
 \
  \____ trigger: zero crossing rising edge


## SOFTWARE
### Build Instructions

IDE: Arduino IDE or similar.

SOURCE: Files .ino, .h, .cpp 

LIBRARY: This code is organized as an installable Arduino library and is listed as "Motion Audio Computer/Oscilloscope".  Copy this library into the Arduino libraries folder and #include the Motion_Oscilloscope.h file into your progam. 

GRAPHICS LIBRARY: Uses the U8G2 library.

## HARDWARE
MCU - Uses Motion MCU Board with SAMD21 processor or similar.  

MONITOR - Motion Audio Signal Monitor (converts line level to SAMD21 ADC compatible input).

GRAPHICS DISPLAY - NHD OLED 256x64 with SSD1306 chipset or similar.

## Contact
https://github.com/willpatton 
