// Include RadioHead Amplitude Shift Keying Library
#include <RH_ASK.h>
// Include dependant SPI Library 
#include <SPI.h> 
 
// Create Amplitude Shift Keying Object
RH_ASK rf_driver;

 int rec;
 int buzzer = 4;
 int red_led = 6;
 int green_led = 8;
 int count = 0;

 unsigned long previous;
void setup()
{
    // Initialize ASK Object
    rf_driver.init();
    // Setup Serial Monitor
    Serial.begin(9600);
    pinMode(buzzer, OUTPUT);
    pinMode(red_led, OUTPUT);
    pinMode(green_led, OUTPUT);
}
 
void loop()
{
    // Set buffer to size of expected message
    uint8_t buf[1];
    uint8_t buflen = sizeof(buf);
    
    if(millis() - previous >= 5000){
      digitalWrite(green_led,LOW);
    }
    // Check if received packet is correct size
    if (rf_driver.recv(buf, &buflen))
    {
      // Message received with valid checksum
      Serial.print("Message Received: ");
      Serial.println((char*)buf);  
      String i = (char*)buf;
      rec = i.toInt();
      
      if(rec == 0){
        Serial.println("Alive");  
        previous = millis();
        digitalWrite(green_led, HIGH);   
      }
      
      else if (rec == 2){
        Serial.println("cool"); 
        for(int i=0; i<=10; i++){
         digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer,LOW);
        delay(50);
        }
        
      }
      else if(rec == 1){
        Serial.println("Danger"); 
        digitalWrite(red_led, HIGH);
        digitalWrite(buzzer, HIGH);
        delay(1500);
        digitalWrite(buzzer,LOW);
        Serial.println("Danger"); 
        digitalWrite(red_led, LOW);
      }
      else{
        Serial.println("Connectiong error !!"); 
      }
    }
}
