
#include <ArduinoBLE.h>
#include <avr/dtostrf.h>

byte statusLed    = 13;
boolean led = true;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// Flow sensor outputs approximately 2500 pulses per litre according to datasheet
float calibrationFactor = 2500;
volatile byte pulseCount;
 
float flowRate;
unsigned int flowMilliLitres;
unsigned int totalCount;
unsigned int totalCentiLitres;
unsigned long oldTime;

unsigned int test = 1;
String testString = "";

//Defining BLEservice and characteristics
BLEService fuelService("180F");
BLEUnsignedIntCharacteristic fuelTotalChar("2A6E", BLENotify);
BLEWordCharacteristic fuelFlowChar("2A20", BLENotify);
// "2A3D" är för generisk String i UTF8S format, hur man nu skickar det.
// går att lägga till BLERead men behövs inte då datat ändras ofta.

void setup()
{
  
  // Initialize a serial connection for reporting values to the host
  Serial.begin(38400);
   
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  //pinMode(sensorPin, INPUT);
  //digitalWrite(sensorPin, HIGH);
  pinMode(sensorPin, INPUT_PULLUP);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  totalCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalCentiLitres  = 0;
  oldTime           = 0;

  // Attach Interrupt to correct pin (number 2)
  // Configured to trigger on a FALLING state change (transition from HIGH state to LOW state)
  attachInterrupt(digitalPinToInterrupt(2), pulseCounter, FALLING);

  if(!BLE.begin())
  {
    Serial.println("Starting BLE failed! You fool!");
    while(1);
  }
  BLE.setLocalName("FlowMonitor");
  BLE.setAdvertisedService(fuelService);
  fuelService.addCharacteristic(fuelTotalChar);
  fuelService.addCharacteristic(fuelFlowChar);
  BLE.addService(fuelService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{

  
   BLEDevice central = BLE.central();
    
   // Loop every 2 seconds, adjusted, to count pulses
   if((millis() - oldTime) > 10000){ 

      //totalMilliLitres++;
      
      // Disable the interrupt while calculating flow rate and sending the value to
      // the host
      detachInterrupt(sensorInterrupt);
          
      // Because this loop may not complete in exactly 1 second intervals we calculate
      // the number of milliseconds that have passed since the last execution and use
      // that to scale the output. We also apply calibration factor to get liter/hour
      flowRate = ((10000.0 / (millis() - oldTime)) * pulseCount * 6 * 60) / calibrationFactor;
      
      // Note the time this processing pass was executed. Note that because we've
      // disabled interrupts the millis() function won't actually be incrementing right
      // at this point, but it will still return the value it was set to just before
      // interrupts went away.
      oldTime = millis();
      
      // Divide the flow rate in litres/minute by 60 to determine how many litres have
      // passed through the sensor in this 1 second interval, then multiply by 1000 to
      // convert to millilitres.
      flowMilliLitres = (flowRate / 60) * 1000;
      
      // Add the millilitres passed in this second to the cumulative total
      totalCount += pulseCount;
      totalCentiLitres = totalCount/25;
        
      unsigned int frac;
      
      // Print the flow rate for this second in litres / minute
      Serial.print("Flow rate: ");
      Serial.print(int(flowRate));  // Print the integer part of the variable
      Serial.print(".");             // Print the decimal point
      // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
      frac = (flowRate - int(flowRate)) * 10;
      Serial.print(frac, DEC) ;      // Print the fractional part of the variable
      Serial.print("L/hr");
      // Print the number of litres flowed in this second
      //Serial.print("  Current Liquid Flowing: ");             // Output separator
      //Serial.print(flowMilliLitres);
      //Serial.print("mL/Sec");
  
      // Print the cumulative total of litres flowed since starting
      Serial.print("  Output Liquid Quantity: ");             // Output separator
      Serial.print(totalCentiLitres);
      Serial.println("cl");
  
      // Blink Status LED
      if (led) {
        digitalWrite(statusLed, LOW);
        led = false;
      }else{
        digitalWrite(statusLed, HIGH);
        led = true;
      }
      
      if (central.connected()){
        fuelTotalChar.writeValue(totalCentiLitres);
        fuelFlowChar.writeValue(flowRate*10);
      }
  
      // Reset the pulse counter so we can start incrementing again
      pulseCount = 0;
      
      // Enable the interrupt again now that we've finished sending output
      attachInterrupt(digitalPinToInterrupt(2), pulseCounter, FALLING);    
   }
}

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
