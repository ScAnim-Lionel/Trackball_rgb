// Use a ps2 track ball to change a led strip color
// WTFPL License - http://en.wikipedia.org/wiki/WTFPL
// Lionel D. code for Science-Animation - 2015
// Code based on libraries for
// trackball and color conversion
// from http://playground.arduino.cc/ComponentLib/Ps2mouse
// & http://www.lucidtronix.com/tutorials/19


#include <ps2.h>
#include <Color.h>

// uncomment next line if serial debug is required
//#define DEBUG

// these are the pins connected to the RGB LEDs
int leds[] = {9,10,11}; 
Color cur_color = Color(1,1,1);

// initial variable settings
float hue = 0;
float sat = 1;
float val = 0.5;
float cumulX = 0;
float cumulY = 0;

/*
 * Pin 5 is the mouse data pin, pin 6 is the clock pin
 * Feel free to use whatever pins are convenient.
 */
PS2 mouse(6, 5);

/*
 * initialize the mouse. Reset it, and place it into remote
 * mode, so we can get the encoder data on demand.
 */
void mouse_init()
{
  mouse.write(0xff);  // reset
  mouse.read();  // ack byte
  mouse.read();  // blank */
  mouse.read();  // blank */
  mouse.write(0xf0);  // remote mode
  mouse.read();  // ack
  delayMicroseconds(100);
}

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
#endif

  mouse_init();
  for(int i = 0 ; i < 3; i++ ){
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
    delay(300); 
    digitalWrite(leds[i], HIGH);
    delay(300);
  }
}

/*
 * get a reading from the mouse and report it back to the
 * host via the serial line.
 */
void loop()
{

  char mstat;
  char mx;
  char my;

  /* get a reading from the mouse */
  mouse.write(0xeb);  // give me data!
  mouse.read();      // ignore ack
  mstat = mouse.read();
  mx = mouse.read();
  my = mouse.read();

#ifdef DEBUG // if debug is defined send data to serial
  /* send the data back up */
  Serial.print(mstat, BIN);
  Serial.print("\tX=");
  Serial.print(mx, DEC);
  Serial.print("\tY=");
  Serial.print(my, DEC);
  Serial.print("\tXcumul=");
  Serial.print(hue, DEC);
  Serial.print("\tVal=");
  Serial.print(val, DEC);
  Serial.println();
  //  delay(20);  /* twiddle */
#endif


  // cumulate mx values to record X trackball travel
  // and limit it to 0 - 1000 range

  cumulX = cumulX + (mx/2);
  if ( cumulX >=1000 ) cumulX = 1000;
  if (cumulX <=0) cumulX = 0;

  // cumulate mY values to record Y trackball travel
  // and limit it to 0 - 500 range

  cumulY = cumulY + (my/2);
  if ( cumulY >=500 ) cumulY = 500;
  if (cumulY <=0) cumulY = 0;

  // X trackball value set color (hue)
  // Y trackball value set value (val)
  // http://www.had2know.com/technology/hsv-rgb-conversion-formula-calculator.html

  hue = cumulX/1000;
  val = cumulY/1000;

  // set color to led strip
  cur_color.convert_hcl_to_rgb(hue,sat,val);
  display_color(cur_color);

}


void display_color(Color c){
  analogWrite(leds[0], c.red);
  analogWrite(leds[1], c.green);
  analogWrite(leds[2], c.blue);
  delay(1);
}

// unused fonction to create a rainbow effect
void rainbow(){
  hue += 0.02;
  if ( hue >=1 ) hue = 0;
  float sat = 0.8;
  float val = 0.2;
  cur_color.convert_hcl_to_rgb(hue,sat,val);
  display_color(cur_color);
  delay(1);
}

