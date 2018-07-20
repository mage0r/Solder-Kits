/*

 
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <Bounce2.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#define VERSION "Multi-Meter V.1.0"

U8G2_ST7565_NHD_C12832_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ A0, /* reset=*/ 8);

String display_value = "0.00";
String display_unit = "V";

byte menu_item = 0;
const byte menu_elements = 3;
byte menu_pos[3] = {9,19,29};

const byte button_1 = 8;
const byte piezo = 3;
const byte display_brightness = 9;
unsigned int display_update = 0;
unsigned int display_update_interval = 250; //500 milliseconds.


// Voltage calculation
const int voltage_pin = A1;
float vout = 0.0;
float vin = 0.0;
float vin_old = 0.0;
const float R1 = 10000.0;
const float R2 = 1000.0;

void setup(void) {
  u8g2.begin();

  pinMode(piezo, OUTPUT);
  
  // This sets the display brightness.
  // with analogWrite can set the display lower, but at 1mhz it's got a visible flicker.
  pinMode(display_brightness, OUTPUT);
  //analogWrite(display_brightness, 255);
  digitalWrite(display_brightness, HIGH);

  pinMode(voltage_pin, INPUT);
  
  

  pinMode(button_1, INPUT);
  //debouncer.attach(button_1);
  //debouncer.interval(1); // interval in ms

  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_cu12_tr); // choose a suitable font
  u8g2.setDisplayRotation(U8G2_R2);
  u8g2.drawStr(0,12,"Multimeter Ruler"); // write something to the internal memory
  u8g2.drawStr(0,30,"Version 1.0");  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(2000);

  
}

void loop(void) {

  // check for button input.
  //debouncer.update();
  //if(debouncer.rose()) {
  if(digitalRead(button_1)) {
    menu_item++;
    if (menu_item >= menu_elements)
      menu_item = 0;
  }

  // run command according to button
  if(menu_item == 0) {
    // continuinity check.
    tone(piezo, 262, 100); // this will play a C4 for 1 second
  } else if(menu_item == 1) {
    noTone(piezo);
    // Voltage check.
    check_voltage();
  } else if (menu_item == 2) {
    noTone(piezo);
    // Resistance Check
  }

  // Update the display.
  if( millis() > (display_update + display_update_interval))
  {
    u8g2.clearBuffer();
    
    display_menu();
  
    u8g2.setFont(u8g2_font_fur20_tf);
    String temp = display_value + display_unit;
    u8g2.drawStr(42,27, temp.c_str());
    //u8g2.drawStr(42,27, "00.00V");
  
    u8g2.sendBuffer();

    display_update = millis();
  }
}

void check_contin() {
  // check for continuinity between probes.
  if(analogRead(voltage_pin) > 0) {
    tone(piezo, 262, 1000); // this will play a C4 for 1 second
    noTone(piezo);
  }
  display_value = analogRead(voltage_pin);
}

void check_voltage() {
  // Check the voltage between the probes
  vout = (analogRead(voltage_pin) * 3.3) / 1024.0; // 3.3v should be our voltage from the regulator.
  
  vin = vout / (R2/(R1+R2));

  if(vin > (vin_old + 0.1) || vin < (vin_old - 0.1)) {
    vin_old = vin;
  }

  // got a bit of flicker here.  not sure why.  Related to unstable voltage?

  display_value = vin_old;
}

void check_ohm() {
  // check the resistance between the probes
}

void display_menu() {
  // all the elements to set up the menu in it's current state.

  //first, set the regular elements
  u8g2.setFont(u8g2_font_open_iconic_play_1x_t);
  u8g2.drawGlyph(10,10,79);
  u8g2.setFont(u8g2_font_helvB08_tf);
  u8g2.drawStr(10,20,"V");
  u8g2.drawStr(10,30,"O");

  
  u8g2.setFont(u8g2_font_helvB08_tf);
  u8g2.drawStr(2,menu_pos[menu_item],">");
  
}

