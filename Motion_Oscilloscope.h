/**
 * Motion_Oscilloscope.h
 *
 * Audio Computer / Oscilloscope
 * @author:   Will Patton 
 * @url:      http://willpatton.com 
 * @license:  MIT License

 Uses the Motion Audio Signal Monitor V1.0 
 
 */
#include <Arduino.h>

#ifndef __MOTION_OSCILLOSCOPE_H
#define __MOTION_OSCILLOSCOPE_H


//U8G2 - OLED
#include <U8g2lib.h>
extern U8G2_SSD1322_NHD_256X64_2_4W_HW_SPI u8g2;
#ifndef WIDTH
#define WIDTH 255
#endif
#ifndef HEIGHT
#define HEIGHT 63
#endif


//PINOUT
#define OSC_PIN_CH1 A1		//input pin - line level audio signal (left)
#define OSC_PIN_CH2 A2		//input pin - line level audio signal (right)
#define OSC_PIN_CH3 A3		//input pin - line level audio signal (sub mono)


//VERT - ADC
#if defined(SAMD_SERIES) 	
#define ANALOG_RESOLUTION 12 	//12-bit = 4096 SAMD processors 
#else
#define ANALOG_RESOLUTION 10	//10-bit = 1024 (default) AVR processors
#endif

//HORIZ - SAMPLES
#define SAMPLES   384  	//256 	(1:1) optimized for 1 sample per 1 pixel
					    //384   (1.5:1) optimized for refresh
					    //512 	(2:1) 
						//640 	(2.5:1) optimized for 1msec per division
						//768 	(3:1)  
						//1024 	(4:1)   optimized for low freq audio



//CLASS
class OSCILLOSCOPE
{

public:
	
	bool detected = false;   //true if hardware detected

	//prototypes
	OSCILLOSCOPE();
	bool begin();   		//is hardware present
	void setResolution(); 	//set VREF, ADC, DAC analog resolution (based on processor capabilities)
	void samples();			//capture array of data 
	void render();  		//display array/graphics to screen
	int  get(int *);	    //get  int val by pass by ref
	bool getBool(bool *);	//get bool val by pass by ref
	void set(int *, int);	//set  int val by pass by ref

private:

	int xr = 0x55aa; 	//value
    //int *xptrr = &xr;   //ptr
	bool xrB = true; 	//value


	//INPUT
	int osc_pin_ch1 = OSC_PIN_CH1;	//the input signal to wired to this ADC pin

	//BIT DEPTH RESOLUTION
	unsigned int bit_depth;	//calculated as 2^10 = 1024 or 2^12 = 4096

	//SCALE
	float scaleV = 1;	//calculated ration of vert samples to pixels
						//1:1 ration is 1 sample per 1 pixel high
	float scaleH;		//calculated ratio of horiz samples to pixels
						//1:1 ratio is 1 sample per 1 pixel	wide  		
						//2.5 ratio is 2.5 samples per pixel
	                    //e.g. ratio 2.5 * 640 samples  = 1.0 msec per division

	//SAMPLES
	int sample[SAMPLES]; 			//the main array of samples
	unsigned long c_samples = 0; 	//count per second
	unsigned long t_samples = 0; 	//usec for 1 screen of samples
	unsigned long tsec_start = 0; 	//1 sec interval timer
	float samples_refresh_hz = 0;	//the screen's refresh rate

	//TRIGGER
	int trigger_normal = true;
	int edge = 1;  //1 = positive -1 = negative  0 = +/- don't care

	//ACQUISITION
	float avg = 0;
	int peak_p = 0;
	int peak_m;     //calculated at runtime to be max bit_depth of 1024 or 4096
	int peak_d = 0;

	//ADJUSTMENTS
    int offset = 2; //corrects for slight difference between waveform zero and screen zero

    //SOUND
    bool sound_detect = false;		//no sound event = true when sound is present
    int sound_detect_val;    	//calculated at runtime
  
    bool overdrive_detect_p = false; 	//overdrive top pos+ rail = true when overdriving
    bool overdrive_detect_m = false; 	//overdrive bot neg- rail = true when overdriving
    //int overdrive_detect_val = 0;   	//calculated at runtime

    //OVERDRIVEN
	bool overdriven = false;

    //AGC
    //TODO - fuzzy logic for AGC gain
    int agc = 0;  //0 nominal, pos+ gain, neg- attenuation

};

#endif