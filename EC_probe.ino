#include <SoftwareSerial.h>
SoftwareSerial BTserial(4,5); // RX TX

// Software by Pierre Hertzog YouTube channel (AgriTech with Pierre) (2020)
// Modified by Jaeon Kim for serial comms via BlueTooth (2022)
// This is the link to his channel : https://www.youtube.com/channel/UCbmgXFBQrxO1SAEVH2qYeGQ  
// Great Content, you can tutorial manufacturing an EC Probe! 
// Please refer to the original video for explanation of the code


// Connections (Also see Video)
// One pin of EC probe to Arduino Gnd
// One pin of EC probe to 1k resistor and to pin A1 of Arduino
// Other pin of resistor to A0
// One pin of thermistor to A5 and one side of 10k resistor
// Other pin of thermistor to 5V on Arduino
// Other pin of 10k resistor to Gnd  
// *5k resistor was used instead since the thermistor was 5k. Must tweak the Steinhart Coefficients accordingly

int x[7];                    // array for storing ppm value as digits so as to faciliate sending as bytes via UART
int R1= 1000;                 // Resistor of EC probe circuit
int EC_Read = A0;
int ECPower = A1;
int Temp_pin = A5;
float Temp_C;
// Do not change
float Temp_F;
// Do not change
float Temp1_Value = 0;
float Temp_Coef = 0.019; // You can leave as it is

/////////////////This part needs your attention during calibration only///////////////
float Calibration_PPM = 1000 ; //Change to PPM reading measured with a separate meter
float K=3.14;
//You must change this constant once for your probe(see video)
float PPM_Con=0.5;  
//You must change this only if your meter uses a different factor
/////////////////////////////////////////////////////////////////////////////////////

float CalibrationEC= (Calibration_PPM*2)/1000;
float Temperature;
float EC;
float EC_at_25;
int ppm;
float A_to_D= 0;
float Vin= 5;
float Vdrop= 0;
float R_Water;
float Value=0;

//Leave the next 2 lines in if you need help later on///////////////////////////////////
//Ask any questions that you may have in the comment section of this video
//https://youtu.be/-xKIczj9rVA

void setup()
{
  Serial.begin(9600); 
  BTserial.begin(9600);
  pinMode(EC_Read,INPUT);
  pinMode(ECPower,OUTPUT);
//////////////////////////////////////////////////////////////////////////////////////////
  //Calibrate (); // After calibration put two forward slashes before this line of code//////////////////////////////////////////////////////////////////////////////////////////
}
void loop()
{
  GetEC();  //Calls GetEC()
   
  int d = getDigit();             // Call getDigit() which converts ppm from int -> bytes so that it can be sent via UART 
  
  for (int i = 0; i <= 2; i++){
    Serial.print(x[d-i]); 
  }
  Serial.println();
  
  if (BTserial.available()) {     // writing to BT port 
  BTserial.write(d);              // var d stores number of digits in ppm value. 
  for (int i = 0; i <= d; i++){
    BTserial.write(x[d-i]); 
  }
  
  }
  
  delay(10000); //Do not make this less than 6 sec (6000)
}
////////////////////////////////////////////////////////////////////////////////////
void GetEC()
{
  int val;
  double Temp;
  val=analogRead(Temp_pin);
  Temp = log(((10240000/val) - 10000));
  //Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp);
  Temp = 1 / (0.00112497 + (0.000234765 + (0.0000000854633 * Temp * Temp ))* Temp );
  
  Temp_C = Temp - 273.15;
  // Kelvin to Celsius
  Temp_F = (Temp_C * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit
  Temp1_Value = Temp_C;
  Temperature = Temp_C;
  digitalWrite(ECPower,HIGH);
  A_to_D= analogRead(EC_Read);
  A_to_D= analogRead(EC_Read);
  digitalWrite(ECPower,LOW);
  Vdrop= (Vin*A_to_D) / 1024.0;
  R_Water = (Vdrop*R1) / (Vin-Vdrop);
  EC = 1000/ (R_Water*K);
  EC_at_25 = EC / (1+ Temp_Coef*(Temperature-25.0));
  ppm=(EC_at_25)*(PPM_Con*1000);
  
  Serial.print(" EC: ");
  Serial.print(EC_at_25);
  Serial.print(" milliSiemens(mS/cm) ");
  Serial.print(ppm);
  Serial.print(" ppm ");
  Serial.print(Temperature);
  Serial.println(" *C ");
}
////////////////////////////////////////////////////////////////////////////////////
void Calibrate ()
{
  Serial.println("Calibration routine started");
  float Temperature_end=0;
  float Temperature_begin=0;
  int val;
  double Temp;
  val=analogRead(Temp_pin);
  Temp = log(((10240000/val) - 10000));
  //Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp);
  Temp = 1 / (0.00112497 + (0.000234765 + (0.0000000854633 * Temp * Temp ))* Temp );
  Temp_C = Temp - 273.15;
  // Kelvin to Celsius
  Temp_F = (Temp_C * 9.0)/ 5.0 + 32.0;
  // Celsius to Fahrenheit
  Temp1_Value = Temp_C;
  Temperature_begin=Temp_C;
  Value = 0;
  int i=1;
  while(i<=10){
    digitalWrite(ECPower,HIGH);
    A_to_D= analogRead(EC_Read);
    A_to_D= analogRead(EC_Read);
    digitalWrite(ECPower,LOW);
    Value=Value+A_to_D;
    i++;
    delay(6000);
  };
  A_to_D=(Value/10);
  val=analogRead(Temp_pin);
  Temp = log(((10240000/val) - 10000));
  //Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp);
  Temp = 1 / (0.00112497 + (0.000234765 + (0.0000000854633 * Temp * Temp ))* Temp );
  Temp_C = Temp - 273.15;
  // Kelvin to Celsius
  Temp_F = (Temp_C * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit
  Temp1_Value = Temp_C;
  Temperature_end=Temp_C;
  EC =CalibrationEC*(1+(Temp_Coef*(Temperature_end-25.0)));
  Vdrop= (((Vin)*(A_to_D))/1024.0);
  R_Water=(Vdrop*R1)/(Vin-Vdrop);
  float K_cal= 1000/(R_Water*EC);
  Serial.print("Replace K in line 23 of code with K = ");
  Serial.println(K_cal);
  Serial.print("Temperature difference start to end were = ");
  Temp_C=Temperature_end-Temperature_begin;
  Serial.print(Temp_C);
  Serial.println("*C");
  Serial.println("Temperature difference start to end must be smaller than 0.15*C");
  Serial.println("");
  Calibrate ();
}

int getDigit()            // store individual digits of ppm value in an array (ie. 424 ppm -> x[] = {4, 2, 5} 
{
    int i, ppm_x;
    i = 0;
    ppm_x = ppm; // used for calc
    while ((ppm_x / 10) != 0) {
      x[i++] = ppm_x % 10; 
      ppm_x = ppm_x / 10;
    }
    x[i] = ppm_x % 10;
    
    return i;
}
////////////////////////////////////////////////////////////////////////////////////
