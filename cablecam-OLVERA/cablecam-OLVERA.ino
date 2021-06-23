

//  /////////////////////////////////////////////////////////////////
//  /////////////////////////////////////////////////////////////////
//  
//  IGLESIA DE JESUCRISTO MAHANAIM - PALABRA MI-EL
//  PASTOR: ARTURO CRUZ ONTIVEROS
//  
//  AREA: INTERNET
//  
//  /////////////////////////////////////////////////////////////////
//  
//    RADIO FREQUENCY CABLE CAM
//  
//  CHARACTERISTICS:
//    1. PAN
//    2. TILT
//    3. ZOOM
//    4. ROLL
//    5. FOCUS
//    6. RIGHT W/ VARIABLE SPEED
//    7. LEFT W/ VARIABLE SPEED
//    8. TELEMETRY VOLTAGE AND TEMPERATURE
//    9. RESET OPTION
//  
//  COMPONENTS:
//    1. ARDUINO NANO
//    2. MOTOR NEMA 17
//    3. TWO ULTRASONIC SENSORS HCSR04
//    4. TRANSMITTER RF FLYSKY FS-I6
//    5. RECEIVER RF FLYSKY FS-IA10B
//    6. LIPO BATTERIES 2S 1000MAH
//    7. STEREN CAM-500 WITH ZOOM LENS
//    8. TWO SERVOS MAX ES08 MD II (ZOOM AND FOCUS)
//    9. GIMBAL STORM32
//  
//  LAST UPDATE: 21-MAYO-2021.
//  
//  /////////////////////////////////////////////////////////////////
//  /////////////////////////////////////////////////////////////////



// MOTOR DRIVER CONFIGURATION
#include <TMCStepper.h>
#define DIR_PIN          7 // Direction
#define STEP_PIN         8 // Step
#define EN_PIN           9 // Enable
#define SW_RX            63 // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX            40 // TMC2208/TMC2224 SoftwareSerial transmit pin
#define R_SENSE 0.11f // Match to your driver
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2
TMC2209Stepper driver(SW_RX, SW_TX, R_SENSE, DRIVER_ADDRESS);



// HCSR04 CONFIG
int periodo = 1000;
unsigned long TiempoAhora = 0;

int TRIG_DER = 6; // pwm out
int ECO_DER = 5; // pwm in
int DURACION_DER;
int DISTANCIA_DER;
bool der = true;

int TRIG_IZQ = 11; // pwm out
int ECO_IZQ = 10; // pwm in
int DURACION_IZQ;
int DISTANCIA_IZQ;
bool izq = true;



// RECEIVER CONFIGURATION  FSIA10B CHANNEL 1, PIN D2 INT
volatile int pwm_value = 0;
volatile int delay_us = 0;
volatile int prev_time = 0;



void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  driver.pdn_disable(1);  // Use PDN/UART pin for communication
  driver.I_scale_analog(0);
  driver.rms_current(100); // Set current to 500mA
  driver.microsteps(1);
  driver.toff(0x2); // Turn on driver
  
  pinMode(TRIG_DER, OUTPUT);
  pinMode(ECO_DER, INPUT);
  pinMode(TRIG_IZQ, OUTPUT);
  pinMode(ECO_IZQ, INPUT);
  
  Serial.begin(9600);
  
  attachInterrupt(0, rising, RISING);
  
  Serial.println(" ////////////////////////////////////////////////////////////");
  Serial.println(" ////////////////////////////////////////////////////////////");
  Serial.println("");
  Serial.println("IGLESIA DE JESUCRISTO MAHANAIM - PALABRA MI-EL");
  Serial.println("PASTOR: ARTURO CRUZ ONTIVEROS");
  Serial.println("");
  Serial.println("AREA: INTERNET");
  Serial.println("PROYECTO: CABLE-CAMERA CON MANDO A DISTANCIA DE RF");
  Serial.println("");
  Serial.println("CARACTERISTICAS:");
  Serial.println("1. PAN");
  Serial.println("2. TILT");
  Serial.println("3. ZOOM");
  Serial.println("4. VIEW");
  Serial.println("5. FOCUS");
  Serial.println("6. RIGHT W/SPEED");
  Serial.println("7. LEFT W/SPEED");
  Serial.println("8. TELEMETRY VOLTAGE");
  Serial.println("9. RESET");
  Serial.println("");
  Serial.println("COMPONENTES:");
  Serial.println("1. ARDUINO NANO");
  Serial.println("2. MOTOR NEMA 17");
  Serial.println("3. SENSORES ULTRASONICOS HCSR04");
  Serial.println("4. EMISORA RF FLYSKY FS-I6");
  Serial.println("5. RECEPTOR RF FLYSKY FS-IA10B");
  Serial.println("6. BATERIAS LIPO 2S 1000MAH");
  Serial.println("7. STEREN CAM-500");
  Serial.println("8. SERVO MOTORES MAX ES08 MD II");
  Serial.println("9. ESTABILIZADOR STORM32");
  Serial.println("");
  Serial.println("Ejecutando sketch cablecam-OLVERA v1.01 -- 21-mayo-2021");
  Serial.println("");
  Serial.println(" ////////////////////////////////////////////////////////////");
  Serial.println(" ////////////////////////////////////////////////////////////");
}



void rising() {
  attachInterrupt(0, falling, FALLING);
  prev_time = micros();
}



void falling() {
  attachInterrupt(0, rising, RISING);
  pwm_value = micros()-prev_time;
  //Serial.println(pwm_value);
}



void hcsr04_ultrasonic() {
  if(millis() > TiempoAhora + periodo){
    TiempoAhora = millis();
    
    digitalWrite(TRIG_DER, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_DER, LOW);
    DURACION_DER = pulseIn(ECO_DER, HIGH);
    DISTANCIA_DER = DURACION_DER / 29.15 / 2;
    
    digitalWrite(TRIG_IZQ, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_IZQ, LOW);
    DURACION_IZQ = pulseIn(ECO_IZQ, HIGH);
    DISTANCIA_IZQ = DURACION_IZQ / 29.15 / 2;
    
    //Serial.println((String)"DISTANCIA IZQ Y DER: "+DISTANCIA_IZQ+"-"+DISTANCIA_DER+" "+pwm_value);
  }
}



void spinner(){
  if(digitalRead(EN_PIN) == HIGH){
    digitalWrite(EN_PIN, LOW);
  }
  if(digitalRead(DIR_PIN) == true){
    delay_us = 50 + pow( ((pwm_value - 1000) / 7), 2 );
    //for(int i=0; i<7; i++){
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(delay_us);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(delay_us);
    //}
  }
  if(digitalRead(DIR_PIN) == false){
    delay_us = 50 + pow( ((2000 - pwm_value) / 7), 2 );
    //for(int i=0; i<7; i++){
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(delay_us);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(delay_us);
    //}
  }
  //Serial.println(pwm_value, delay_ms);
}



void loop(){
  // HCSR04 ULTRASONIC
  hcsr04_ultrasonic();
  
  
  // CENTER - STATIC
  if(pwm_value > 1400 && pwm_value < 1600 && digitalRead(EN_PIN) == LOW){
    digitalWrite(EN_PIN, HIGH);
  }
  
  
  // LEFT SPIN
  if(pwm_value >= 1000 && pwm_value <= 1400 && izq == true){
    if(DISTANCIA_DER > 20){
      der = true;
    }
    if(DISTANCIA_IZQ <= 20){
      izq = false;
    }
    digitalWrite(DIR_PIN, true);
    spinner();
  }
  
  
  // RIGHT SPIN
  if(pwm_value >= 1600 && pwm_value <= 2000 && der == true){
    if(DISTANCIA_IZQ > 20){
      izq = true;
    }
    if(DISTANCIA_DER <= 20){
      der = false;
    }
    digitalWrite(DIR_PIN, false);
    spinner();
  }
  
}
