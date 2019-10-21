/*
 * Mega Drive mini Joystick adapter by Arduino Leonardo
 * 2019/10/10 tounosumura302
 */

#include <Joystick.h>

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK,
  10, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X, Y, and Z Axis
  false, false, false,   // Rx, but no Ry or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

const int ARROW_MIN=0;
const int ARROW_MAX=511;
const int ARROW_NEUTRAL=(ARROW_MAX-ARROW_MIN)/2;

const int DP08_PB=0x10; //digital pin  8 = port B 4
const int DP09_PB=0x20; //digital pin  9 = port B 5
const int DP10_PB=0x40; //digital pin 10 = port B 6
const int DP11_PB=0x80; //digital pin 11 = port B 7

const int DP02_PD=0x02; //digital pin  2 = port D 1
const int DP03_PD=0x01; //digital pin  3 = port D 0
const int DP04_PD=0x10; //digital pin  4 = port D 4
const int DP06_PD=0x80; //digital pin  6 = port D 7
const int DP12_PD=0x40; //digital pin 12 = port D 6


void setup() {
  // Initialize Button Pins
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  // Initialize Joystick Library
  Joystick.begin(false);
  Joystick.setXAxisRange(ARROW_MIN, ARROW_MAX);
  Joystick.setYAxisRange(ARROW_MIN, ARROW_MAX);
  Joystick.setXAxis(ARROW_NEUTRAL);
  Joystick.setYAxis(ARROW_NEUTRAL);
}

  int lastButtonState_U=0;
  int lastButtonState_D=0;
  int lastButtonState_L=0;
  int lastButtonState_R=0;
  int lastButtonState_A     =0;
  int lastButtonState_B     =0;
  int lastButtonState_C     =0;
  int lastButtonState_START =0;
  int lastButtonState_MODE  =0;
  
  const int MD_BUTTON_ID_A=1;
  const int MD_BUTTON_ID_B=2;
  const int MD_BUTTON_ID_C=4;
  const int MD_BUTTON_ID_START=9;
  const int MD_BUTTON_ID_MODE=8;

/*
 * port = port (PINB,PIND)
 * testvalue = test bit of port which indicates pin number
 * buttonid = press / release button id
 * laststate = last state of pin (0=released)
 */
inline bool setButton(int port,int testvalue,int buttonid,int *laststate){
  int state=port&testvalue;
  if (state!=*laststate){
    return false;
  }
  if (state==0){
    Joystick.pressButton(buttonid);
    *laststate=testvalue;
  }
  else{
    Joystick.releaseButton(buttonid);
    *laststate=0;
  }
  return true;
}

inline bool setXAxis(int port,int testvalue,int offvalue,int onvalue,int *laststate){
  int state=port&testvalue;
  if (state!=*laststate){
    return false;
  }
  if (state==0){
    Joystick.setXAxis(onvalue);
    *laststate=testvalue;
  }
  else{
    Joystick.setXAxis(offvalue);
    *laststate=0;
  }
  return true;
}

inline bool setYAxis(int port,int testvalue,int offvalue,int onvalue,int *laststate){
  int state=port&testvalue;
  if (state!=*laststate){
    return false;
  }
  if (state==0){
    Joystick.setYAxis(onvalue);
    *laststate=testvalue;
  }
  else{
    Joystick.setYAxis(offvalue);
    *laststate=0;
  }
  return true;
}


void loop() {
  bool sendFlag = false;
  int currentPortD=PIND;  //read digital pin state from port register
  int currentPortB=PINB;  //read digital pin state from port register

  sendFlag|=setButton(currentPortD,DP02_PD,MD_BUTTON_ID_A    ,&lastButtonState_A);
  sendFlag|=setButton(currentPortD,DP03_PD,MD_BUTTON_ID_B    ,&lastButtonState_B);
  sendFlag|=setButton(currentPortD,DP04_PD,MD_BUTTON_ID_C    ,&lastButtonState_C);
  sendFlag|=setButton(currentPortD,DP06_PD,MD_BUTTON_ID_START,&lastButtonState_START);
  sendFlag|=setButton(currentPortD,DP12_PD,MD_BUTTON_ID_MODE ,&lastButtonState_MODE);
  sendFlag|=setXAxis(currentPortB,DP08_PB,ARROW_NEUTRAL,ARROW_MIN,&lastButtonState_L);
  sendFlag|=setXAxis(currentPortB,DP09_PB,ARROW_NEUTRAL,ARROW_MAX,&lastButtonState_R);
  sendFlag|=setYAxis(currentPortB,DP10_PB,ARROW_NEUTRAL,ARROW_MIN,&lastButtonState_U);
  sendFlag|=setYAxis(currentPortB,DP11_PB,ARROW_NEUTRAL,ARROW_MAX,&lastButtonState_D);

  if (sendFlag){
    Joystick.sendState();
  }

  delay(10);

}
