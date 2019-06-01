#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <TinyGPS++.h>

// The serial connection to the GPS device

// The TinyGPS++ object
TinyGPSPlus gps;
unsigned long lastUpdateTime = 0;
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

double tampilheading ;
double tampilbearing ;

#define EIFFEL_LAT -7.946372//lat tujuan
#define EIFFEL_LNG 112.613070 //long tujuan

/* This example shows a basic framework for how you might
   use course and distance to guide a person (or a drone)
   to a destination.  This destination is the Eiffel Tower.
   Change it as required.  

   The easiest way to get the lat/long coordinate is to 
   right-click the destination in Google Maps (maps.google.com),
   and choose "What's here?".  This puts the exact values in the 
   search box.
*/

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

  Serial.println("Percobaan GPS dan Compass untuk menentukan arah");
  Serial.println("24-mei-2019");
  Serial.println();

}

void loop()
{
    getGPS();

   /* Compass */
   /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);

  // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  
  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  float declinationAngle = 0.53;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 
  
    tampilheading = headingDegrees;
    
   if (headingDegrees == 0 || headingDegrees == 360 ) {
    Serial.println("N");
    }
    if (headingDegrees > 0 && headingDegrees < 90 ) {
    Serial.print("N");
    // Degree from north towards east direction
    Serial.print(headingDegrees);
    Serial.write(176);
    Serial.println("E");
    }
    if (headingDegrees == 90) {
    Serial.println("E");
    }
    if (headingDegrees > 90 && headingDegrees < 180 ) {
    // Degree from south towards east direction
    headingDegrees = 180 - headingDegrees;
    Serial.print("S");
    Serial.print(headingDegrees);
    Serial.write(176);
    Serial.println("E");
    }
    if (headingDegrees == 180) {
    Serial.println("S");
    }
    if (headingDegrees > 180 && headingDegrees < 270 ) {
    // Degree from south towards west direction
    headingDegrees = headingDegrees - 180;
    Serial.print("S");
    Serial.print(headingDegrees);
    Serial.write(176);
    Serial.println("W");
    }
    if (headingDegrees == 270) {
    Serial.println("W");
    }
    if (headingDegrees > 270 && headingDegrees < 360 ) {
    // Degree from North towards west direction
    headingDegrees = 360 - headingDegrees;
    Serial.print("N");
    Serial.print(headingDegrees);
    Serial.write(176);
    Serial.println("W");
    }

     tampilbearing = headingDegrees;
    Serial.print("Heading :");
    Serial.print(tampilheading);
    Serial.print("||");
    Serial.print("Bearing :");
    Serial.print(tampilbearing);
    Serial.println("."); 
    delay(1000);
  
  // Every 5 seconds, do an update.
  if (millis() - lastUpdateTime >= 1000)
  {
    lastUpdateTime = millis();
    Serial.println();

    // Establish our current status
    double distanceToDestination = TinyGPSPlus::distanceBetween(
      gps.location.lat(), gps.location.lng(),EIFFEL_LAT, EIFFEL_LNG);
    double courseToDestination = TinyGPSPlus::courseTo(
      gps.location.lat(), gps.location.lng(), EIFFEL_LAT, EIFFEL_LNG);
    const char *directionToDestination = TinyGPSPlus::cardinal(courseToDestination);
    int courseChangeNeeded = (int)(360 + courseToDestination - gps.course.deg()) % 360;

    // debug
    Serial.print("DEBUG: Course2Dest: ");
    Serial.print(courseToDestination);
    Serial.print("  CurCourse: ");
    Serial.print(gps.course.deg());
    Serial.print("  Dir2Dest: ");
    Serial.print(directionToDestination);
    Serial.print("  RelCourse: ");
    Serial.print(courseChangeNeeded);

    // Within 20 meters of destination?  We're here!
    if (distanceToDestination <= 10.0)
    {
      Serial.println("CONGRATULATIONS: You've arrived!");
      exit(1);
    }

    Serial.print("DISTANCE: ");
    Serial.print(distanceToDestination);
    Serial.println(" meters to go.");
    Serial.print("INSTRUCTION: ");
    Serial.print(tampilheading);
    Serial.print("||");
    Serial.println(courseToDestination);

    double x = courseToDestination-5;
    double y = courseToDestination+5;
    
   
    if (tampilheading > x && tampilheading < y ){
      Serial.println("Maju beh");
    } else if (tampilheading >= 0 && tampilheading <= 180){
      Serial.println("Nganan Beh");
    } else {
      Serial.println("Ngiri Beh");      
    }

   }

}

void getGPS(){
  // If any characters have arrived from the GPS,
  // send them to the TinyGPS++ object
  while (Serial1.available() > 0)
    gps.encode(Serial1.read());
}
