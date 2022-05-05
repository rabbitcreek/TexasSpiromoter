#include <Arduino.h>
#include <Wire.h>
#include <Omron_D6FPH.h>
float area_1 = 0.000531; //these are the areas taken carefully from the 3D printed venturi 2M before constriction
float area_2 = 0.000201;// this is area within the venturi
Omron_D6FPH mySensor;
float rho = 1.225; //Demsity of air in kg/m3;
float massFlow = 0;
float volFlow = 0.0;;
float maxFlow = 0.0;
float volumeTotal = 0; //variable for holding total volume of breath
float corrvolumeTotal = 0.0;  // correcion of volumeTotal based on actual measurement(currently set equal)
float TimerNow = 0.0;
float fevC = 0.0;
float fev1 = 0.0;
float clocker = 1000.0;
float fevTimer = 0.0;
int tag = 0;
bool flag = 0;
bool setter = 0;
void setup() {
    Serial.begin(115200);
    Wire.begin();
    mySensor.begin(MODEL_5050AD3);
    
} 

void loop() {

    float pressure = mySensor.getPressure();
    if(isnan(pressure)){
    Serial.println("Error: Could not read pressure data");
    }else{
        if(abs(pressure) < 0.5) pressure = 0;
        if((pressure > 0.0) && (flag == 0)) {
          TimerNow = millis();
          setter = 0;
          fevTimer = millis();
          volumeTotal = 0;         
          flag = 1;
          tag = 1;
          Serial.print("Max Flow =  ");
          Serial.println(maxFlow);
          maxFlow = 0;
        }
         else if((pressure < 0.0) && (flag == 1)) {
          fevC = volumeTotal;
          TimerNow = millis();
          volumeTotal = 0;          
          flag = 0;
          tag = -1;
          Serial.print("End of Exhale FEVC=  ");
          Serial.println(fevC);
          Serial.print("fev1/fevC = ");
          Serial.print("Max Flow =  ");
          Serial.println(maxFlow);
          maxFlow = 0;
        }
        
        
        
        //Serial.print("Differential Pressure: ");
        Serial.println(pressure);
        massFlow = 1000*sqrt((abs(pressure)*2*rho)/((1/(pow(area_2,2)))-(1/(pow(area_1,2))))); //Bernoulli equation
        volFlow = massFlow/rho; //volumetric flow of air
        volumeTotal = volFlow * (millis() - TimerNow) + volumeTotal;//int
        TimerNow = millis(); ///This is part of the integral function to keep calculation volume over time..resets amount of time between calcs
        if(volFlow > maxFlow) maxFlow = volFlow;
        if(((millis() - fevTimer) > 1000.0) && (setter == 0)) {
          fev1 = volumeTotal;
          Serial.print("FEV1 =  ");
          Serial.println( fev1);
          setter = 1;
        }
        Serial.print("liters/sec  ");
        Serial.print(volFlow * tag);
        Serial.print("totalvolume  ");
        Serial.println(volumeTotal * tag);
        delay(20);

        
    }
/*
    float temperature = mySensor.getTemperature();
    if(isnan(temperature)){
    Serial.println("Error: Could not read temperature data");
    }else{
        Serial.print("Temperature C: ");
        Serial.println(temperature);
    }
    Serial.println(temperature);
 */
  delay(25);
}
