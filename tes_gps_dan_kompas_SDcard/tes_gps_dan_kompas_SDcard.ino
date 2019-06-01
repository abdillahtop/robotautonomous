#include <Wire.h>
#include "compass.h"
#include <TinyGPS++.h>
#include <AFMotor.h>
#include <SD.h>

#define Task_t 10          // Task Time in milli seconds

TinyGPSPlus gps;
//Motor Shield Initialize
AF_DCMotor motor_r1(1, MOTOR12_64KHZ); 
AF_DCMotor motor_r2(2, MOTOR12_64KHZ); 
AF_DCMotor motor_r3(3, MOTOR34_64KHZ); 
AF_DCMotor motor_r4(4, MOTOR34_64KHZ); 
//SD Card
File myfile;

unsigned long lastUpdateTime = 0;
double tampilheading ;
double tampilbearing ;
int dt=0;
unsigned long t;

#define LAT_TUJUAN -7.946372//lat tujuan
#define LNG_TUJUAN 112.613070 //long tujuan

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial1 GPS
  Serial1.begin(9600);
  Serial.println("Percobaan GPS dan Compass untuk menentukan arah");
  Serial.println("27-mei-2019");
  Serial.println();

  motor_r1.setSpeed(200);
  motor_r2.setSpeed(200);
  motor_r3.setSpeed(200);
  motor_r4.setSpeed(200);
  
  // Serial.print("Setting up I2C ........\n");
  Wire.begin();
  compass_x_offset = 122.17;
  compass_y_offset = 230.08;
  compass_z_offset = 389.85;
  compass_x_gainError = 1.12;
  compass_y_gainError = 1.13;
  compass_z_gainError = 1.03;
  
  compass_init(2);
  // compass_offset_calibration(3);

    pinMode(53, OUTPUT); // change this to 53 on a mega
      
    if (!SD.begin(chipSelect)){
    Serial.println("Initialization failed!");
    while(1);
  }else{
    Serial.println("initialization done."); 
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  getGPS();
  t = millis();
 
  float load;

  compass_scalled_reading();

  compass_heading();

  //tampil heading
  tampilheading = bearing;


    // GPS 
    // Every 1 seconds, do an update.
  if (millis() - lastUpdateTime >= 500)
  {
    lastUpdateTime = millis();
    Serial.println();

    // Establish our current status
    double distanceToDestination = TinyGPSPlus::distanceBetween(
      gps.location.lat(), gps.location.lng(),LAT_TUJUAN, LNG_TUJUAN);
    double courseToDestination = TinyGPSPlus::courseTo(
      gps.location.lat(), gps.location.lng(), LAT_TUJUAN, LNG_TUJUAN);
    const char *directionToDestination = TinyGPSPlus::cardinal(courseToDestination);
    int courseChangeNeeded = (int)(360 + courseToDestination - gps.course.deg()) % 360;
    
    // Within 20 meters of destination?  We're here!
    if (distanceToDestination <= 15.0)
    {
      Serial.println("CONGRATULATIONS: You've arrived!");
      berhenti();
      exit(1);
    }

    //Menjalankan Motor Shield dengan toleransi 10 Derajat
     double x = courseToDestination-5;
     double y = courseToDestination+5;
     double z = y-10;

      myfile = SD.open("GPS.csv",FILE_WRITE);
      if(myfile){
        myfile.println(dataStringlat+","+dataStringlong+);
        myfile.close();
        Serial.println(dataStringlat+","+dataStringlong);
      }else{
        Serial.println("Error Opening...");
      }
      
    
    //DEBUG
    Serial.print("Course2Dest: ");
    Serial.print(courseToDestination);
    Serial.print(" Dir2Dest: ");
    Serial.print(courseChangeNeeded);
    Serial.print (" Heading angle = ");
    Serial.print (tampilheading);
    Serial.println(" Degree");
    Serial.print("DISTANCE: ");
    Serial.print(distanceToDestination);
    Serial.println(" meters to go.");
    Serial.print("INSTRUCTION: ");
     
    if (tampilheading > x && tampilheading < y ){
        maju();
      } else if (tampilheading > z){
        kiri();
      } else{
        kanan();    
      }
    Serial.print("Nilai Z: ");
    Serial.print(z);
    delay(500);
  }

  
  
}


void getGPS(){
  // If any characters have arrived from the GPS,
  // send them to the TinyGPS++ object
  while (Serial1.available() > 0)
    gps.encode(Serial1.read());
}

void kiri (){
   Serial.print("Kiri");
   motor_r1.run(BACKWARD); 
   motor_r2.run(FORWARD); 
}

void kanan (){
   Serial.print("Kanan");
   motor_r1.run(FORWARD); 
   motor_r2.run(BACKWARD); 
}

void maju (){
   Serial.print("Maju");
   motor_r1.run(BACKWARD); 
   motor_r2.run(BACKWARD);
   motor_r3.run(BACKWARD); 
   motor_r4.run(BACKWARD);
}

void berhenti () {
   Serial.print("Berhenti");
   motor_r1.run(RELEASE); 
   motor_r2.run(RELEASE);
   motor_r3.run(RELEASE); 
   motor_r4.run(RELEASE);
}
