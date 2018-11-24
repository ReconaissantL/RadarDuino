/*  RadarDuino : 360 deg Radar.     
 *  Inspired by Dejan Nedelkovski, 
 *  From www.HowToMechatronics.com
 *
 *  Modified by Ahmed Doghri 
 */
 
import processing.serial.*;          // imports library for serial communication
import java.awt.event.KeyEvent;      // imports library for reading the data from the serial port
import java.io.IOException;

//Define the Global Variables
Serial myPort;              
String angle="";
String distance="";
String data="";
String noObject;
String val;
float pixsDistance_1,pixsDistance_2;
int iAngle, iDistance_1,iDistance_2;
int index1=0;
int index2=0;
PFont orcFont;

void setup(){
 size (1000, 1000);                            //Keep it as a square window -> change size to fit your screen resolution
 smooth();
 String port_name = Serial.list()[4];          // Change this accrodingly
 myPort = new Serial(this, port_name, 115200); // starts the serial communication
 orcFont = loadFont("OCRAExtended-30.vlw");    // Import the Radar Font, you can get it @ : https://github.com/lastralab/ArduinoRadar/blob/master/OCRAExtended-30.vlw
}

void draw() {
  //myPort.write(97);
  if ( myPort.available() > 0){
    data = myPort.readStringUntil('\n');
    print(data);  // for troubleshooting alternatives
    if (data != null ){
      String[] data_list = split(data, ',');
      iAngle = int(data_list[0]);
      iDistance_1 = int(data_list[1]);
      iDistance_2 = int(data_list[2]);
      println(iAngle);
      println(iDistance_1);
      println(iDistance_2);
    }
  }
  fill(98,245,31); 
  textFont(orcFont); 
  noStroke(); 
  fill(0,4); 
  rect(0, 0, width, height); 
  fill(98,245,31); 
  drawRadar(); 
  drawLine();
  drawObject();
  drawText();  
}


void drawRadar() { // Draws the radar grid.
  pushMatrix();
  translate(width/2,height/2);
  noFill();
  strokeWeight(2);
  stroke(98,245,31);
  ellipse(0, 0,(width-width*0.0625), (width-width*0.0625));
  ellipse(0, 0,(width-width*0.27), (width-width*0.27));
  ellipse(0, 0,(width-width*0.479), (width-width*0.479));
  ellipse(0, 0,(width-width*0.687), (width-width*0.687));
  line(-width/2,0,width/2,0);
  line((-width/2)*cos(radians(210)),(-width/2)*sin(radians(210)),(-width/2)*cos(radians(30)),(-width/2)*sin(radians(30)));
  line((-width/2)*cos(radians(240)),(-width/2)*sin(radians(240)),(-width/2)*cos(radians(60)),(-width/2)*sin(radians(60)));
  line((-width/2)*cos(radians(270)),(-width/2)*sin(radians(270)),(-width/2)*cos(radians(90)),(-width/2)*sin(radians(90)));
  line((-width/2)*cos(radians(300)),(-width/2)*sin(radians(300)),(-width/2)*cos(radians(120)),(-width/2)*sin(radians(120)));
  line((-width/2)*cos(radians(330)),(-width/2)*sin(radians(330)),(-width/2)*cos(radians(150)),(-width/2)*sin(radians(150)));
  popMatrix();
}

void drawObject() { //Draws the object lines. 
  pushMatrix();
  translate(width/2,height/2); 
  strokeWeight(9);
  stroke(255,10,10); 
  if (iDistance_1 ==0){iDistance_1 =60; }                      //60 cm tested pulseIn timeout 
  if (iDistance_2 ==0){iDistance_2 =60; }                      //60 cm tested pulseIn timeout 
  pixsDistance_1 = iDistance_1*((height-height*0.1666)*0.025); // converts the distance from the sensor from cm to pixels
  pixsDistance_2 = iDistance_2*((height-height*0.1666)*0.025); // converts the distance from the sensor from cm to pixels
  if(iDistance_1<40){  // limiting the range to 40 cms
  line(pixsDistance_1*cos(radians(iAngle)),-pixsDistance_1*sin(radians(iAngle)),(width-width*0.12)*cos(radians(iAngle)),-(width-width*0.12)*sin(radians(iAngle))); //0.505
  }
  if(iDistance_2<40){  // limiting the range to 40 cms
  line(pixsDistance_2*cos(radians(iAngle+180)),-pixsDistance_2*sin(radians(iAngle+180)),(width-width*0.12)*cos(radians(iAngle+180)),-(width-width*0.12)*sin(radians(iAngle+180)));
  }
  popMatrix();
}

void drawLine() { //Draws the green traversing lines
  pushMatrix();
  strokeWeight(9);
  stroke(30,250,60);
  translate(width/2,height/2); 
  line(0,0,(height-height*0.12)*cos(radians(iAngle)),-(height-height*0.12)*sin(radians(iAngle)));         // draws the line according to the angle
  line(0,0,(height-height*0.12)*cos(radians(iAngle+180)),-(height-height*0.12)*sin(radians(iAngle+180))); // draws the line according to the angle
  popMatrix();
}


void drawText() { // draws the texts.
  pushMatrix();
  fill(0,0,0);
  noStroke();
  fill(98,245,31);
  textSize(int(20*width/1000));
  text("10cm",width-width*0.375,height- height*0.51);
  text("20cm",width-width*0.270,height- height*0.51);
  text("30cm",width-width*0.165,height- height*0.51);
  text("40cm",width-width*0.062,height- height*0.51);
  
  
  textSize(int(20*width/1000));
  if(iDistance_1<40) {
  text("Distance 1: ", width-width*0.30,int (30*width/1000));
  text("    " + iDistance_1 +" cm", width-width*0.18, int (30*width/1000));
  text("Angle 1: " + iAngle +" °", width-width*0.23, int(60*width/1000));
  }
  if(iDistance_2<40) {
  text("Distance 2: ", int(10*width/1000), height- height*0.03);
  text("    " + iDistance_2 +" cm", int(120*width/1000), height- height*0.03);
  text("Angle 2: " + iAngle +" °", int(10*width/1000), height- height*0.06);
  }
  textSize(int (width*25/1000));
  fill(98,245,60);

  //upper half 
  translate(width/2+0.95*width/2*cos(radians(-1)),height/2-0.95*width/2*sin(radians(-1)));
  rotate(radians(90));
  text("0°",0,0);
  resetMatrix();
  translate(width/2 +0.95*width/2*cos(radians(28.9)), height/2- 0.95*width/2*sin(radians(28.9)));
  rotate(-radians(-60));
  text("30°",0,0);
  resetMatrix();
  translate(width/2+0.95*width/2*cos(radians(59.9)), height/2 -0.95*width/2*sin(radians(59.9)));
  rotate(-radians(-30));
  text("60°",0,0);
  resetMatrix();
  translate(width/2 +0.95*width/2*cos(radians(89.9)), height/2-0.95*width/2*sin(radians(89.9)));
  rotate(radians(0));
  text("90°",0,0);
  resetMatrix();
  translate(width/2+0.95*width/2*cos(radians(119.9)),height/2-0.95*width/2*sin(radians(119.9)));
  rotate(radians(-30));
  text("120°",0,0);
  resetMatrix();
  translate(width/2+0.95*width/2*cos(radians(149.9)),height/2-0.95*width/2*sin(radians(149.9)));
  rotate(radians(-60));
  text("150°",0,0);
  resetMatrix();

  //lower half 
  translate(width/2+0.95*width/2*cos(radians(179.7)),height/2-0.95*width/2*sin(radians(179.7)));
  rotate(radians(-90));
  text("180°",0,0);
  resetMatrix();
  translate(width/2 +0.95*width/2*cos(radians(209.8)), height/2- 0.95*width/2*sin(radians(209.8)));
  rotate(-radians(120));
  text("210°",0,0);
  resetMatrix();
  translate(width/2+0.95*width/2*cos(radians(239.8)), height/2 -0.95*width/2*sin(radians(239.8)));
  rotate(-radians(150));
  text("240°",0,0);
  resetMatrix();
  translate(width/2 +0.95*width/2*cos(radians(269.8)), height/2-0.95*width/2*sin(radians(269.8)));
  rotate(radians(180));
  text("270°",0,0);
  resetMatrix();
  translate(width/2+0.95*width/2*cos(radians(299.8)),height/2-0.95*width/2*sin(radians(299.8)));
  rotate(radians(150));
  text("300°",0,0);
  resetMatrix();
  translate(width/2+0.95*width/2*cos(radians(329.8)),height/2-0.95*width/2*sin(radians(329.8)));
  rotate(radians(120));
  text("330°",0,0);
  resetMatrix();
  popMatrix(); 
}
