/**
 * Motion_Oscilloscope.cpp
 *
 * Audio Computer / Oscilloscope
 * @author:   Will Patton 
 * @url:      http://willpatton.com 
 * @license:  MIT License


256x64, 16K pixels
+--------------------------------------------------------+
|1.25Vpp                                            21 Hz| measurements, refresh rate
|-                  ___              _   . . . . . . . . | cursors
|                  /   \            / \                  | 
|-/\  ____________/     \       /\_/   \_/\  ___________ | waveform - 0V GND
|   \/                   \     /           \/            |
|-                        \___/                          |
|1V/div                                           1ms/div| controls
+--------------------------------------------------------+
 \
  \____ trigger: zero crossing rising edge


 */

#include "Motion_Oscilloscope.h"


/**
 * constructor
 */
OSCILLOSCOPE::OSCILLOSCOPE(){

  //init the bit depth to 1024 or 4096
  bit_depth = (int)pow(2, ANALOG_RESOLUTION);

  //init this peak- value to the max bit depth
  peak_m = bit_depth;   

  //init sound detect to 1/2 of bit depth
  sound_detect_val  = bit_depth / 2;

  //calc horiz scale ratio
  //1:1 ratio is 1 sample per 1 pixel
  //e.g. 640 samples / WIDTH 256 pixels = ratio 2.5 samples per pixel
  scaleH = (float)SAMPLES / (float)(WIDTH+1); 

}

/**
 * getters
 */
int OSCILLOSCOPE::get(int* var){
  return *var;
}
bool OSCILLOSCOPE::getBool(bool* var){
  return *var;
}

/**
 * setter
 */
void OSCILLOSCOPE::set(int* x, int val){
  //nothing yet
}

/**
 * begin()
 *
 * Detects if the 
 * Motion Audio Signal Monitor V1.0 hardware is present 
 * by sensing 1/2 bias of VCC within 10%.
 *
 * If an active signal is on input, this will not yet work
 * 
 * returns true if present, else false
 * 
 */
bool OSCILLOSCOPE::begin(){
  
  Serial.print("Detecting oscilloscope... ");

  //debug
  if(0){
    Serial.print("\nbit_depth: ");Serial.println(bit_depth);
    Serial.print("scaleH: ");Serial.println(scaleH);
    Serial.print("SAMPLES ");Serial.println(SAMPLES);
    Serial.print("sample array sizeof: ");Serial.println(sizeof(sample));
    Serial.print("WIDTH+1: ");Serial.println(WIDTH+1);
    Serial.print("HEIGHT+1: ");Serial.println(HEIGHT+1);
    //Serial.println(get(&xr),HEX);
    //Serial.println(getBool(&xrB),HEX);
  }


  //acquire samples
  /*
  int trig_temp = trigger_normal; //save setting, not desired during detection
  samples();
  trigger_normal = trig_temp; //restore setting
  */
  //PATCH to detect scope
  pinMode(OSC_PIN_CH1, INPUT);
  setResolution();
  delayMicroseconds(500);
  //READ ONCE - skip 1st reading
  float adcVin = 0;
  adcVin = analogRead(A1);
  delayMicroseconds(500); //settling time
  //READ MANY
  adcVin = 0;
  for(int i=0; i < 8; i++){
    adcVin += analogRead(A1);
    delayMicroseconds(50);
  }
  avg = adcVin/8;
  

  //detect when input is quiet, no signal connected
  //calc if the avg bias is within an expected range of +/- 10% ?
  float min = bit_depth/2 * .9;  //12-bit typically: 1843 min
  float max = bit_depth/2 * 1.1; //    "     "       2253 max
  float constrainedInput = constrain(avg, min, max);  //12-bit bit typical reading of 2002.0 when found
  if(avg == constrainedInput){
    //Serial.print("Avg Bias "); Serial.print (avg);
    detected = true; //if avg signal is near the bias, then it is deemed "found"
  }
  Serial.print("Avg Bias "); Serial.print (avg);

  //alternatively, dectect if signal is present
  //maybe a signal greater than 4% or 28mVp-p is already present?
  if(!detected){
    //calc if there is a peak delta great than 4%
    if(peak_d > bit_depth * 0.04){ //28mVp-p creates a peak_d of 172.
      //apparent signal wiggling on pin
      Serial.print(" Signal "); Serial.print(peak_d);
      detected = true;
    }
  }

  if(detected){
    Serial.println(" FOUND.");
    return true;
  } 
  Serial.println(" NOT FOUND.");
  detected = false;
  return false;
}


/**
 * set analog resolution based on processor capabilities
 */
void OSCILLOSCOPE::setResolution(){

  //SELECT input
  pinMode(osc_pin_ch1, INPUT);  

  //AREF
  //Serial.println("Setting AREF to: AR_DEFAULT");
  analogReference(AR_DEFAULT); //AR_DEFAULT = 3.3V  

  //ADC - ANALOG READ resolution (SAMD)
  //Serial.println("Setting DAC to 12-bit READ resolution.");
  analogReadResolution(ANALOG_RESOLUTION);  //default AVR 10 = 1023, SAMD 12 = 4096

  //ADC SETTLING TIME
  delayMicroseconds(50); //TODO: needed? Optimize: What minimum could this be?

}


/**
 * acquire - captures samples and trigger at zero crossing
 */
void OSCILLOSCOPE::samples(){

  //REFRESH
  //capture stats to calculate oscilloscope refresh rate
  //reset count each second
  if(c_samples == 0){
    tsec_start = micros();
  }
  //capture count each second
  if(micros() - tsec_start >= 1000000){
    samples_refresh_hz = c_samples;
    c_samples = 0;
    tsec_start = micros();
  }
 
  //RESOLUTION
  setResolution();  //

  //SAMPLE / ACQUIRE
  for(int i=0; i < SAMPLES; i++){
    sample[i] = analogRead(osc_pin_ch1);
    if(i==0){
      //start: acquistion timer only when i==0
      t_samples = micros(); 
    }

    //TRIGGER NORMAL MODE
    //this inherently adds aquistion delay to wait for trigger
    if(trigger_normal){

      //find near zero crossing on first sample +/- 4%
      if(i==0){
        int trigger_level = bit_depth / 2;
        if(trigger_level != constrain(trigger_level, sample[i] * .98, sample[i] * 1.02)){ 
          i=-1; //reset i
          //continue;
        }
      } 

      //EDGE
      //If nth samples are equal to or higher than previous samples, it must be a rising edge. Right?   
      if(i > 0 && i<2){
        //rising edge
        if(edge > 0 && (sample[i] - sample[i-1] <= 0)){
          i=-1; //reset i
        }
        //falling edge
        if(edge < 0 && (sample[i] - sample[i-1] > 0)){
          i=-1; //reset i
        }
      }
    }
  }//end for
  //stop: acquistion timer
  t_samples = micros() - t_samples;

  //debug - dump array
  //for(int i; i<SAMPLES; i++){
  //  Serial.print("Sample: "); Serial.print(i); Serial.print(" "); Serial.println(sample[i]);
  //}

  //clear detect logic before calculations
  sound_detect = false;   
  overdrive_detect_p = false; //top pos+ rail
  overdrive_detect_m = false; //bot neg- rail


  //MEASUREMENTS
  //find peak +/-, peak-to-peak, sound detect, overdrive
  avg = 0;
  peak_p = 0;
  peak_m = bit_depth;
  for(int i=0; i < SAMPLES; i++){
    if(sample[i] > peak_p){peak_p = sample[i];}
    if(sample[i] < peak_m){peak_m = sample[i];}
    avg += sample[i];

    //sound detect.  No sound if within +/- 4% of gnd/bias 
    if(sound_detect_val != constrain(sound_detect_val, sample[i] * .96, sample[i] * 1.04)){
      sound_detect = true;
    }

    //overdrive detect. 
    //Deemed overdriven if peak sample is near rails 
    //NOTE: Only the pos+ rail is used to detect overdrive because it can be driven 100% to the pos+ rail.
    if(peak_p >= bit_depth - 10){  //4095 typical "at the top rail" value
      overdrive_detect_p = true; //signal near top pos(bit_depth)
    }
    if(peak_m <= 70){ //58 to 63 typical "at the bottom rail" value
      //NOTE: A reliable value of below 60 can't be sampled by the MCU because 
      //  of the opamp's low rail limit is slightly above 0V GND. 
      overdrive_detect_m = true;       //signal at bottom neg(0) rail
    } 

  }
  avg = avg/SAMPLES;
  peak_d = peak_p - peak_m; 
  
  //refresh rate counter - count acquisitions per second (e.g. refresh Hz)
  c_samples++;
}


/**
 * renders a waveform from an array as an "oscilloscope trace"
 */
void OSCILLOSCOPE::render(){

  if(!detected){return;} //is the hardware detected? If not, not need to render.

  int x_grph = 0;
  int y_grph = 0; 

 u8g2.firstPage();
      do {    

        //WAVEFORM
        if(1){
          for(int i = 0; i < SAMPLES; i++) {  
            //map(value, fromLow, fromHigh, toLow, toHigh)
            x_grph = map(i, 0, SAMPLES, 0, SAMPLES/scaleH);  // 0 to n max pixels wide (e.g. 255)
            y_grph = map(sample[i], 0, bit_depth, 0, HEIGHT/scaleV);  // 0 to n max pixels tall (e.g. 63)
            //y_grph = random(0,HEIGHT);  //makes snow!
            u8g2.drawPixel(x_grph, HEIGHT - offset - y_grph);
          }
          //debug - dump array
          /*
          if(0){
            for(int i = 0; i < SAMPLES; i++){
                Serial.print("{"); Serial.print(i); Serial.print(","); Serial.print(y_grph); Serial.print("} ");    
                if(i == SAMPLES - 1) {Serial.println();}  
            }
          }
          */
        }
       
        //GRATICULE LINES 
        //VERT - about 4 divisions
        if(1){   
          //NOTE: 16 PIXELS MISSING FROM RIGHT SIDE - FIX ME!! 
          //NOTE: 16 PIXELS MISSING FROM RIGHT SIDE - FIX ME!! 
          //NOTE: 16 PIXELS MISSING FROM RIGHT SIDE - FIX ME!! 
          //centerline
          //u8g2.drawLine((WIDTH/2)-offset, 0, (WIDTH/2)-offset, HEIGHT);  
          //right edge
          //u8g2.drawLine(WIDTH-16, 0, WIDTH-16, HEIGHT);  
          //left edge
          //u8g2.drawLine(0, 0, 0, HEIGHT);   

          //VERT - 4 divisions, 0.825V/div WANT: 1V/div
          //vert center line -> PATCHED to appear on left side
          x_grph = 2; //WIDTH / 2;        
          u8g2.drawLine(x_grph - offset, 0, x_grph - offset, HEIGHT);
          //ticks
          int width_tick = 4;//2;
          float v_ticks = (HEIGHT+1) / 4; //map(3.3 / 4 * (HEIGHT)/scaleV, 0, 4, 0, (HEIGHT)/scaleV);
          for(int i=1; i<5; i++){
            u8g2.drawLine(x_grph - offset /*- width_tick*/, i * v_ticks, x_grph - offset + width_tick, i * v_ticks); 
          }          
        }

        //HORIZ - about 10 divisions
        if(0){
          //horiz middle
          y_grph = map(bit_depth / 2, 0, bit_depth, 0, HEIGHT);        
          u8g2.drawLine(0, y_grph, WIDTH, y_grph);
          //ticks
          int height_tick = 2;
          float division = (WIDTH) / 10;
          for(int i=1; i<11; i++){
            u8g2.drawLine(i * division, y_grph - height_tick, i * division, y_grph + height_tick); 
          } 
        }
   
        //REFRESH
        if(1){
          /*
          //acquistion timer - usec
          u8g2.setFont(u8g2_font_helvB08_tr);
          String str_o = String((float)t_samples/1000,1);
          str_o = String(str_o + " ms");
          u8g2.drawStr(2, HEIGHT, str_o.c_str()); 
          */

          //refresh Hz
          //u8g2.setFont(u8g2_font_unifont_t_symbols);
          //u8g2.drawGlyph(194, 10, 0x21BA);  //refresh symbol
          ////u8g2.setFont(u8g2_font_unifont_t_0_86);       //does not exist
          ////u8g2.drawGlyph(205, 10, 0x2B6F);  //bigger refresh symbol
          u8g2.setFont(u8g2_font_helvB08_tr);
          String str_hz = String(samples_refresh_hz,0);
          str_hz = String(str_hz + " Hz");
          u8g2.drawStr(204, 10, str_hz.c_str()); 
          

        }
      
        //MEASUREMENTS - TEXT
        if(0){

          //VERT - volt per division
          float v_div = 3.3 / 4; //map(1/HEIGHT, 0, 3.3, 0, HEIGHT); //
          //float v_middle = HEIGHT/2 * v_div;
          String str_v_div = String(v_div);
          str_v_div = String(str_v_div + " V/div");
          u8g2.drawStr(2, HEIGHT-2, str_v_div.c_str()); 

          //HORIZ - time per divsion
          float t_div = t_samples;
          String str_t_div = String((float)t_div / 10000,1);
          str_t_div = String(str_t_div + " ms/div");
          u8g2.drawStr(180, HEIGHT-2, str_t_div.c_str()); 

          //PEAK
          if(0){
            /*
            //peak plus+
            String str_peak_p = String(peak_p);       //was: peak_p - bit_depth / 2
            str_peak_p = String(str_peak_p + " pk+");
            //u8g2.drawStr(90, HEIGHT-2, str_peak_p.c_str());  
            //peak minus-
            String str_peak_m = String(peak_m);       //was: -(int)(bit_depth / 2 - peak_m)
            str_peak_m = String(str_peak_m + " pk-");
            //u8g2.drawStr(140, HEIGHT-2, str_peak_m.c_str()); 
            //peak-to-peak delta 
            */
            String str_peak_d = String(peak_d);
            str_peak_d = String(str_peak_d + " pk-pk");
            u8g2.drawStr(104, HEIGHT-2, str_peak_d.c_str());  
          }

          //VOLTS
          if(1){
            //volts pk-pk
            float volts_pkpk = (peak_d * 0.818)/1000;  //amplification adjust
            String str_volts_pkpk = String(volts_pkpk,2);
            str_volts_pkpk = String(str_volts_pkpk + " Vpp"); //Vpk-pk
            u8g2.drawStr(2, 10, str_volts_pkpk.c_str());   
          }     
        }
         

        //CURSORS - "Y" VERT
        //overlay peak cursors onto waveform
        if(1){

          //Y-TOP cursor
          int yP =  map(peak_p, 0, bit_depth, 0, HEIGHT/scaleV); 
          if(yP > 44){
            //solid line
            yP = HEIGHT - offset - yP;
            if(yP < 0){yP=0;}
            if(HEIGHT - offset - yP < 1){yP = 0;}
            u8g2.drawLine(0, yP , WIDTH, yP );
          } else {
            //dotted, dashed-line
            int step = 1; //1=dotted, 2 or more dashed
            //int b2 = 0;
            for(int x1,i=0; i < WIDTH; ){
              i+=step;
              u8g2.drawLine(x1, HEIGHT - offset - yP, i, HEIGHT - offset - yP);
              i = i + step * 5;//blank
              x1 = i;
            }
          }
          //Y-BOTTOM cursor
          int yM =  map(peak_m, 0, bit_depth, 0, HEIGHT/scaleV); 
          yM = HEIGHT - offset - yM;
          if(yM > HEIGHT){yM = HEIGHT;}
          u8g2.drawLine(0, yM, WIDTH, yM);
        }  

        //SOUND DETECT ICON
        if(1){
          if(!sound_detect){
            //too little sound
            u8g2.setFont(u8g2_font_unifont_t_symbols);
            u8g2.drawGlyph(127-8, 10, 0x23F9);  //no sound symbol "square" 0x23F9
          } else {
            //sound okay
            //u8g2.setFont(u8g2_font_unifont_t_symbols);
            //u8g2.drawGlyph(127-8, 10, 0x23F5);  //sound symbol "arrow" 0x23F5
          }
        }

        //OVERDRIVE DETECT ICON
        if(1){
          if(overdrive_detect_p){
            //too much pos+ signal icon
            u8g2.setFont(u8g2_font_unifont_t_symbols);
            u8g2.drawGlyph(127-8, 10, 0x25B3);  //overdrive symbol "empty triangle" 0x25B3
          } 
          if(overdrive_detect_m){
            //too much pos+ signal icon
            u8g2.setFont(u8g2_font_unifont_t_symbols);
            u8g2.drawGlyph(127-8, HEIGHT, 0x25BD);  //overdrive symbol "empty triangle inverted" 0x25BD
          } 
        }

        //0 dBV LEVEL DETECT ICON
        //REFERENCE: 0 dBV Reference Level      = 1Vrms = 1.44Vpk = 2.88Vpp
        //REFERENCE: -10 dBV Line Level(Nominal)= 0.316Vrms = 0.447Vpk = 0.894Vpp



      } while (u8g2.nextPage()); 
      //delay(700); //debug - slow down refresh to observe visual effects
}



