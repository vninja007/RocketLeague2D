  /* Adapted from the Adafruit graphicstest sketch, see original header at end
 of sketch.

 This sketch uses the GLCD font (font 1) only.

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/

//Code and implementation by Vrishak Vemuri, using external libraries


#include "SPI.h"
#include "TFT_eSPI.h"
#define FPS 24
#define W 25
#define H 50
#define sp 8
#define ballr 10
#define mycolor 0x555F
#define ballcolor 0xFFFF
#define opcolor 0xFE8E
TFT_eSPI tft = TFT_eSPI();

unsigned long total = 0;
unsigned long tn = 0;
double halfw = W/2.0;
double halfh = H/2.0;
int R = sqrt(halfw*halfw+halfh*halfh);
double frame = 1.0/FPS *1000;
void setup() {
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(32, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  Serial.begin(9600);
  while (!Serial);

  Serial.println(""); Serial.println("");
  Serial.println("TFT_eSPI library test!");

  tft.init();

  tn = micros();
  tft.fillScreen(TFT_BLACK);

  yield(); Serial.println(F("Benchmark                Time (microseconds)"));
}


float distPL(int x1, int y1, int x2, int y2, int xp, int yp){
  float A = xp-x1;
  float B = yp-y1;
  float C = x2-x1;
  float D = y2-y1;

  float dp = A*C+B*D;
  float lensq = C*C+D*D;
  float param = -1;
  if(lensq!=0){
    param = dp/lensq;
  }
  float xx = -999.0;
  float yy = -999.0;
  if(param < 0){
    xx = x1;
    yy = y1;
  }
  else if (param > 1){
    xx = x2;
    yy = y2;
  }
  else{
    xx = x1 + param *C;
    yy = y1 + param *D;
  }
  float dx = xp-xx;
  float dy = yp-yy;
  return sqrt(dx*dx+dy*dy);
}



float theta = 0;
  float drift = 3*3.14159/2;
  int cx = 120;
  int cy = 260;
  int bx = 120;
  int by = 160;
  float bvx = 3;
  float bvy = 3;
  
  boolean ctimeout = true;
  float friction = 0.1;
void loop(void) {
  tft.fillScreen(TFT_BLACK);
  int xa = analogRead(34)/40;
  int ya = analogRead(35)/40;
  int sa = digitalRead(32);
  Serial.print(cx);
  Serial.print(" ");
  Serial.print(cy);
  Serial.print(" ");
  Serial.print(digitalRead(0));
  Serial.print(" ");
  Serial.print(theta);
  Serial.print(" ");
  Serial.print(drift);
  Serial.print("\t\t\t");


  float os = atan((W+0.0)/H);
  theta = 0-analogRead(34)/2048.0*2*3.14159;

  int c1x = cx-R*cos(drift+os);
  int c1y = cy+R*sin(drift+os);
  int c2x = cx-R*cos(drift-os);
  int c2y = cy+R*sin(drift-os);
  int c3x = cx-R*cos(drift+os+3.14159);
  int c3y = cy+R*sin(drift+os+3.14159);
  int c4x = cx-R*cos(drift-os+3.14159);
  int c4y = cy+R*sin(drift-os+3.14159);

  
  float ud = distPL(c1x, c1y, c2x, c2y, bx, by);
  float rd = distPL(c2x, c2y, c3x, c3y, bx, by);
  float dd = distPL(c3x, c3y, c4x, c4y, bx, by);
  float ld = distPL(c4x, c4y, c1x, c1y, bx, by);

  tft.drawLine(c1x, c1y, c2x, c2y, TFT_GREEN);
  tft.drawLine(c2x, c2y, c3x, c3y, TFT_GREEN);
  tft.drawLine(c3x, c3y, c4x, c4y, TFT_GREEN);
  tft.drawLine(c4x, c4y, c1x, c1y, TFT_GREEN);
  

  Serial.print(ld);
  Serial.print(" ");
  Serial.print(ud);
  Serial.print(" ");
  Serial.print(rd);
  Serial.print(" ");
  Serial.print(dd);
  Serial.print("\t\t");
  
  drift += (theta+6.17)/20;

  int accel = (1-digitalRead(0))*sp;
  cx -= accel*cos(drift);
  cy += accel*sin(drift);
  bx += bvx;
  by += bvy;

  if(bx > 240-ballr || bx<ballr){ bvx *= -1;}
  if(by > 320-ballr || by<ballr){ bvy *= -1;}
  if(bx > 240-ballr){bx = 240-ballr;}
  if(bx < ballr){bx = ballr;}
  if(by > 320-ballr){by = 320-ballr;}
  if(by < ballr){by = ballr;}

  if((ud<ballr || ld<ballr || dd<ballr || rd<ballr) and ctimeout){
    bvx *= -1;
    bvy *= -1;
    if(ud<ballr){
      bvx -= 2.5*accel * cos(drift);
      bvy += 2.5*accel * sin(drift);
    }
    ctimeout = false;
  }
  else if (ud > ballr+7 && ld > ballr+7 && dd>ballr+7 && rd>ballr+7){
    ctimeout = true;
  }
  
  bvx *= (1-friction);
  bvy *= (1-friction);
  Serial.print(bvx);
  Serial.print(" ");
  Serial.print(bvy);
  Serial.print("\t\t");

  Serial.println(ctimeout);
  if(not ctimeout and (ud<ballr || ld<ballr || dd<ballr || rd<ballr)){
    float xway = bx-cx;
    float yway = by-cy;
    float mag = sqrt(xway*xway + yway*yway);
    xway = xway/mag;
    yway = yway/mag;
    bvx += 2.5*xway;
    bvx += 2.5*yway;
  }
  
  tft.fillCircle(bx,by,ballr, ballcolor);
  tft.fillTriangle(c1x, c1y, c2x, c2y, c3x, c3y, mycolor);
  tft.fillTriangle(c3x, c3y, c4x, c4y, c1x, c1y, mycolor);
  delay(frame);
}
