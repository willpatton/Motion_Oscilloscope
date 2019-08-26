Motion Audio Analyzer/Visualizer and Oscilloscope
===========
This library is an audio analyzer and/or oscilloscope used to visualize waveforms, make measurements, and detect events. It samples audio signals using a processor's analog-to-digital (ADC) to sample waveforms.  Waveforms can then be analyzed, displayed, or recorded.

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
* Horizontal time per division
* Measurements - peak-to-peak
* Trigger on zero-crossing, rising/falling edge
* Refresh rate

## Oscilloscope
The oscilloscope feature is a 256x64 screen (16K pixels) divided into 10 horizontal time divisions (1ms/div) and 4 verticle voltage divisions (1V/div). A waveform acquisition is triggered by sensing the signal's zero-crossing point and looking for a rising or falling edge.  Once triggered, a memory buffer is filled with the samples that represent the waveform acquisition. Then, the buffer is analyzed for peaks and patterns thus making automatic measurements of the waveform. Next, the buffer's samples are rendered to the graphics display where 1 sample is rendered as 1 pixel (1:1). 
![Motion Oscilloscope Text Waveform](https://github.com/willpatton/Motion_Oscilloscope/blob/master/motion_oscilloscope_text_waveform.png)


## SOFTWARE
### Build Instructions

IDE: Arduino IDE or similar.

SOURCE: Files .ino, .h, .cpp 

LIBRARY: This code is organized as an installable Arduino library and is listed as "Motion Audio Computer/Oscilloscope".  Copy this library into the Arduino libraries folder and #include the Motion_Oscilloscope.h file into your program. 

GRAPHICS LIBRARY: Uses the U8G2 library.

## HARDWARE
MCU - Uses Motion MCU Board with SAMD21 processor or similar.  

MONITOR - Motion Audio Signal Monitor (converts line level to SAMD21 ADC compatible input).

GRAPHICS DISPLAY - NHD OLED 256x64 with SSD1306 chipset or similar.

## Contact
https://github.com/willpatton 
