#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "src/onScreenDef.h"

//onScreenDef should contain (as time goes on) more and more things about the screen
//including generated initialization functions. These will be built by desktopapp
//at the time of initialization.


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned int screenSize = SCREEN_WIDTH * SCREEN_HEIGHT / 8;
char screenBuf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];
char serBuf[16];

void displayBuffer(char* buf, Adafruit_SSD1306* screen)
{
  screen->clearDisplay();
  int posn;
  for(int i=0; i < screenSize; i++)
  {
    for(int j=7; j >= 0; j--){ //for conveneience, read bytes big to little
      posn = i*8+j;
      Serial.print(posn%SCREEN_WIDTH);Serial.print(" , ");Serial.print(posn/SCREEN_WIDTH);
      if(buf[i] & (0b1 << j))
      {
        screen->drawPixel(posn%SCREEN_WIDTH,posn/SCREEN_WIDTH,SSD1306_WHITE);
        Serial.print(", draw");
        
      }
      Serial.println("");
    }
  }
  screen->display();
}


int getCommand(){
  int i = 0;
  //we may want to keep the last spot open for a null terminator
  while(i<14){
  if(Serial.available())
  {
    serBuf[i]=Serial.read();
    if(serBuf[i] == 10)//newline
    {
      if(!strncmp(serBuf,"write",5))
      {
        Serial.print("got write");
        return 1;
      }
      else// got a newline terminated string, but it wasnt a command
      {
        return 0;
      }
    }
    i++;// increment i to reflect that a char was read
  }
  }
  return 0;
}
  
void fillBuffer(){
  int i = 0;
  while(i<screenSize){
  if(Serial.available())
  {
    screenBuf[i]=Serial.read();
    i++;// increment i to reflect that a char was read
  }
  }
  Serial.println("OK: got screen buffer");
}

void setup() {
  Serial.begin(115200);
  Serial.println("in setup");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Serial.println("mid setup");

  display.clearDisplay();
  display.display();
  
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  Serial.println("exit setup");

}

void loop() {
  Serial.println("in main loop");
  int cmd=0;
  while(1){
    cmd = getCommand();
    switch(cmd)
    {
      case 1:
      Serial.println("case 1");
      fillBuffer();
      displayBuffer(screenBuf, &display);
      break;
    
      default:
      Serial.println("invalid command");
    }
  }
  
}
