// Project: Radarduino
// Author: Ahmed Doghri 
// date: 07~21/11/2018

#include <Servo.h>
#include <Wire.h>
#include <stdlib.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define MODE            1        // mode 0 -> small LED display, Mode 1 -> monitor display
#define Servo_pin       9 
#define Trigger_pin_1   5 
#define Echo_pin_1      4
#define Trigger_pin_2   7 
#define Echo_pin_2      6
#define Pulse_Timeout   4300     //about max of 60~62 cm 
#define OLED_ADDR       0x3C     //OLED display address for I2C communication
#define Forward         1
#define Backward        0 
#define WHITE           1
#define BLACK           0 

#if (MODE)
#define Step        1
#else 
#define Step        2
#endif
#if (SSD1306_LCDHEIGHT != 64)     // making sure that the OLED display dimensions match
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

Servo servo;
Adafruit_SSD1306 display(-1);
char to_processing [20];          // contrains the result to be send serially
float  dist_1,dist_2,saved_dist_1,saved_dist_2;
int radius_array_1[7]={0};//radius array 1, 
int radius_array_2[7]={0};//radius array 2 
int saved_radius_array_1[7]={0};  // saved array to delete the drawn beams 
int saved_radius_array_2[7]={0};  // saved array to delete the drawn beam

void setup() {
  Serial.begin (115200);                           // Initialize The Serial Communication
  while (Serial.read() >=0){}      
  servo.attach(Servo_pin);                         // Initialize the Servo 
  servo.write (0);                 
  pinMode (Trigger_pin_1, OUTPUT);                 // Initialize both Ultrasonice Sensors
  pinMode (Echo_pin_1, INPUT);
  pinMode (Trigger_pin_2, OUTPUT);
  pinMode (Echo_pin_2, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);  // Initialize the OLED Display
  display.clearDisplay();
  display.display();
  display_grid();
}

void loop() {
  for (int i =0; i <=180; i= i +Step){
    //display.clearDisplay();
    display_grid();
    //draw_scanner(i,Forward ); 
    servo.write (i);
    dist_1 = ultrasonic(Echo_pin_1,Trigger_pin_1);
    dist_2 = ultrasonic(Echo_pin_2,Trigger_pin_2); 
    int r1= int (dist_1 * 32/40) ; // 32 pixels / 60 cm (max distance detectable)
    int r2= int(dist_2 *32/40);
    if (dist_1 ==0 || dist_1 > 40){r1 = 32;}
    if (dist_2 ==0 || dist_2 > 40){r2 = 32;}
    update_array(radius_array_1,r1);
    update_array(radius_array_2,r2);
    if (MODE){
      //while (Serial.read()!= 'a'){}
      sprintf(to_processing, "%d,%d,%d,",i,int(dist_1),int(dist_2));
      Serial.println(to_processing);
      }else {
    if (i > Step){
      draw_scanner(i-Step,saved_radius_array_1,saved_radius_array_2,Forward, BLACK); 
    }
    draw_scanner(i,radius_array_1,radius_array_2,Forward, WHITE); 
    display.display();
    copy_array(radius_array_1,saved_radius_array_1);
    copy_array(radius_array_2,saved_radius_array_2);
    delay(1);  
      }
   }
   if (!MODE){
     draw_scanner(180,saved_radius_array_1,saved_radius_array_2,Forward, BLACK); 
     display.display();
   }
 for (int i =180; i >=0; i= i -Step){
    //display.clearDisplay();
    display_grid();
    servo.write (i);
    dist_1 = ultrasonic(Echo_pin_1,Trigger_pin_1);
    dist_2 = ultrasonic(Echo_pin_2,Trigger_pin_2); 
    int r1= int (dist_1 * 32/40) ; // 32 pixels / 60 cm (max distance detectable)
    int r2= int(dist_2 *32/40);
    if (dist_1 ==0 || dist_1 > 40){r1 = 32;}
    if (dist_2 ==0 || dist_2 > 40){r2 = 32;}
    update_array(radius_array_1,r1);
    update_array(radius_array_2,r2);
    if (MODE){
      //while (Serial.read()!= 'a'){}
      sprintf(to_processing, "%d,%d,%d,",i,int(dist_1),int(dist_2));
      Serial.println(to_processing);
      }
    else {
      if (i < 180 - Step){
        draw_scanner(i+Step,saved_radius_array_1,saved_radius_array_2,Backward, BLACK); 
      }
      draw_scanner(i,radius_array_1,radius_array_2,Backward,WHITE); 
      display.display();
      copy_array(radius_array_1,saved_radius_array_1);
      copy_array(radius_array_2,saved_radius_array_2);
      delay(1);
    }
  }
  if (!MODE){
    draw_scanner(0,saved_radius_array_1,saved_radius_array_1,Backward, BLACK); 
    display.display();
  }
}


void display_grid(){   //Displays the radar grid 
  display.writeLine(0,32,128,32,WHITE);
  display.writeLine(64,0,64,64,WHITE); 
  display.writeLine(64,32,64 + cos(45*PI/180)*64,32+sin(45*PI/180)*64,WHITE); 
  display.writeLine(64,32,64 + cos(135*PI/180)*64,32+sin(135*PI/180)*64,WHITE); 
  display.writeLine(64,32,64 + cos(-45*PI/180)*64,32+sin(-45*PI/180)*64,WHITE); 
  display.writeLine(64,32,64 + cos(-135*PI/180)*64,32+sin(-135*PI/180)*64,WHITE); 
  display.drawCircle (64,32,12,WHITE);
  display.drawCircle (64,32,21,WHITE);
  display.drawCircle (64,32,31,WHITE);
  display.drawCircle (64,32,44,WHITE);
  display.drawCircle (64,32,63,WHITE);
  display.display();
}
void draw_scanner (int i, int radius_array_1[7] , int radius_array_2[7], bool dir,bool value){ // Draws the detection lines on the radar screen
  if (dir){ //forward direction
    if (i <= 4 ){    // Dsiplays the beginning of the scan (edge case)
      int index =0;
      for ( int j = i; j >=0; j = j-1){
        display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
        display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
        index ++;
        }
      }
    else if (i <=20){ // Dsiplays the beginning of the scan (edge case)
      int index =0;
      for ( int j = i; j >i - 4; j =j-1){
        display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
        display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
        index++;
        }
      for ( int j = i-4; j >=0; j= j = j -6){  
        display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
        display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
        index ++;
        }
      }
    else {  // Displays the scan in the middle of the process (general case)
      int index =0;
      for ( int j = i; j >i-4; j = j - 1){
        display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
        display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
        index++;
        }
      for ( int j = i-4; j >i-20; j = j-6){
        display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
        display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
        index ++;
        }     
      }   
  }
  else {  // backward direction
   if (180- i <= 4 ){     // Dsiplays the beginning of the scan (edge case)
    int index =0;
    for ( int j = i; j <=180; j = j+1){
      display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
      display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
      index ++;
      }
    }
  else if (180-i <= 20){  // Dsiplays the beginning of the scan (edge case)
    int index =0;
    for ( int j = i; j < i+4; j =j+1){
      display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
      display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
      index++;
      }
    for ( int j = i+4; j <=180; j= j = j +6){
      display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
      display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
      index++;
      }
    }
  else {   // Displays the scan in the middle of the process (general case)
    int index =0;
    for ( int j = i; j < i+4; j =j+1){
      display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
      display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
      index++;
      }
    for ( int j = i+4; j <= i+20; j= j = j +6){
      display.writeLine(64,32,64 + cos(j*PI/180)*radius_array_1[index],32-sin(j*PI/180)*radius_array_1[index],value); 
      display.writeLine(64,32,64 - cos(j*PI/180)*radius_array_2[index],32+sin(j*PI/180)*radius_array_2[index],value);
      index++;
    }     
   }    
  }    
}

int ultrasonic (int echo, int trig){ // Gets the distance from the Ultrasonic Sensor
  digitalWrite(echo, LOW);
  delayMicroseconds(1);
  digitalWrite(trig, HIGH);
  delayMicroseconds(2);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH,Pulse_Timeout)*0.034/2;
}

void update_array (int arr[7], int element){  //updates the distance array 
  for (int i = 6; i >0; i --){
    arr[i]=arr[i-1];
  }
  arr[0]= element; 
}

void copy_array (int to_be_copied[7], int result_array[7]){ // copies one array into the other 
  for (int i =0; i <=6; i ++ ){
    result_array[i]= to_be_copied[i];
  } 
}
