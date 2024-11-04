/**
 * Comprehensive BLDC motor control example using encoder and the DRV8302 board
 *
 * Using serial terminal user can send motor commands and configure the motor and FOC in real-time:
 * - configure PID controller constants
 * - change motion control loops
 * - monitor motor variabels
 * - set target values
 * - check all the configuration values
 *
 * check the https://docs.simplefoc.com for full list of motor commands
 *
 */

/* 
Not using serial commander due to problems when using it together with an encoder with interupts.
The serial communication happens on core 0 and all rest including interups happens on core 1*/
#include <SimpleFOC.h>
#include <encoder_pcnt.h>

//#include "driver/temp_sensor.h"

// DRV8302 pins connections
// don't forget to connect the common ground pin
#define INH_A 21 //gpio D10
#define INH_B 18 //gpio D9
#define INH_C 17 //gpio D8
#define INL_A 4  //gpio A3
#define INL_B 13 //gpio A6
#define INL_C 12 //gpio A5


// Normal Pin numbering 
#define EN_GATE 7 // D7 gpio 10
#define M_PWM 4 // D4 gpio 7
#define OC_ADJ 5 // D5 gpio 8

unsigned int  monitor_decimals = 4; //!< monitor outputs decimal places
int hz = 500;
// FreeRTOS handles for the tasks
TaskHandle_t mainTaskHandle;
TaskHandle_t serialTaskHandle;
SemaphoreHandle_t mutex;

// Define the data structure to be sent via the queue
struct Data {
  float value;
};


// Motor instance
BLDCMotor motor = BLDCMotor(11);
//BLDCDriver3PWM driver = BLDCDriver3PWM(INH_A, INH_B, INH_C, EN_GATE);
BLDCDriver6PWM driver = BLDCDriver6PWM(INH_A,INL_A, INH_B,INL_B, INH_C,INL_C, EN_GATE);

// DRV8302 board has 0.005Ohm shunt resistors and the gain of 12.22 V/V

// encoder instance
//EncoderPCNT encoder = EncoderPCNT(5, 6, 2048); // GPIO D2, D3
EncoderPCNT encoder = EncoderPCNT(5, 6, 4096); // GPIO D2, D3


bool readSerial(char* msg, int& msg_cnt){
  try{
    while (Serial.available()) 
    {
      // get the new byte:
      char ch = Serial.read();
      // end of user input
      if (ch == '\n') {
        return 1;
      }
      msg[msg_cnt] = ch;
      msg_cnt += 1;
      if (msg_cnt >20){
        Serial.println("Too long in read");
        msg_cnt = 0;
      }
    }
  }
  catch (...){
    Serial.println("Something wrong in read");
  }


  return 0;
}



void setup() {
  
  // initialize encoder sensor hardware
  encoder.init();
  // link the motor to the sensor
  motor.linkSensor(&encoder);
  // DRV8302 specific code
  // M_OC  - enable overcurrent protection
  pinMode(M_PWM,OUTPUT);
  digitalWrite(M_PWM, LOW);
  pinMode(OC_ADJ,OUTPUT);
  digitalWrite(OC_ADJ, HIGH);
  setCpuFrequencyMhz(240);
  
  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 20;
  //driver.dead_zone = 0.05;

  //driver.pwm_frequency = 5000;
  driver.init();
  
  // link the motor and the driver
  motor.linkDriver(&driver);
  // link current sense and the driver
  // align voltage
  motor.voltage_sensor_align = 6;
    // choose FOC modulation
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
  //motor.foc_modulation = FOCModulationType::SinePWM;
  // control loop type and torque mode 
  motor.torque_controller = TorqueControlType::voltage;
  motor.controller = MotionControlType::torque;
  //motor.motion_downsample = 1.0;
  
  // velocity loop PID
  motor.PID_velocity.P = 0.2;
  motor.PID_velocity.I = 5.0;
  // Low pass filtering time constant 
  motor.LPF_velocity.Tf = 0.02;
  // angle loop PID
  motor.P_angle.P = 20.0;
  // Low pass filtering time constant 
  motor.LPF_angle.Tf = 0.0;
  // current q loop PID 
  motor.PID_current_q.P = 3.0;
  motor.PID_current_q.I = 100.0;
  // Low pass filtering time constant 
  motor.LPF_current_q.Tf = 0.02;
  // current d loop PID
  motor.PID_current_d.P = 3.0;
  motor.PID_current_d.I = 100.0;
  // Low pass filtering time constant 
  motor.LPF_current_d.Tf = 0.02;

  // Limits 
  motor.velocity_limit = 100.0; // 100 rad/s velocity limit
  motor.voltage_limit = 20.0;   // 12 Volt limit 
  motor.current_limit = 1;    // 2 Amp current limit

  
  _delay(500);
  // use monitoring with serial for motor init
  // monitoring port
  
  Serial.begin(115200);// Create a HardwareSerial object
  //Serial1.begin(115200, SERIAL_8N1, D12, D13);
  
  //Serial.setDebugOutput(true);
  //Serial.setTxTimeoutMs(0); 
  //Serial.enableReboot(false);
  
  //Serial.setRxBufferSize(1024);

  while (Serial != true){
    delay(10);
  }


  // comment out if not needed
  //motor.useMonitoring(Serial);
  //motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // monitor the two currents d and q
  //motor.monitor_variables = _MON_ANGLE; // monitor the two currents d and q
  //motor.monitor_downsample = 1;
  //motor.monitor_decimals = 3;
  //motor.modulation_centered	= false; Lowers standby voltage but not, the resistance of movment at 0 voltage
  // initialise motor
  motor.init();

  // align encoder and start FOC

  motor.initFOC();
  // set the inital target value
  motor.target = 0.0;

  Serial.println(F("Voltage control"));
  _delay(1000);
}


const int max_input_lenght = 20;
char received_chars[max_input_lenght+3] = {0}; //!< so far received user message - waiting for newline
int rec_cnt = 0; //!< number of characters receives
Data data;
//String msg = "";
int msg_length = 0;
unsigned long previousMicros = micros();    // Stores the last time loop ran
unsigned long currentMicros = micros();    // Stores the last time loop ran
const long interval = int(1e6/hz);          // Interval in milliseconds

void loop() {

  motor.loopFOC();
  motor.move(); 

  currentMicros = micros();
  if (currentMicros - previousMicros >= interval){
    previousMicros = currentMicros;
    float angle = motor.shaft_angle;
    Serial.println(angle, monitor_decimals);
  }


  if (readSerial(received_chars, rec_cnt)){
    if (received_chars[0] == 'T'){
      // Create a buffer to hold the substring
      char substring[rec_cnt] = {0}; // +1 for the null terminator
      //memset(substring, 0, rec_cnt); // Ensure it's null-terminated
      // Copy the characters from received_chars to substring
      for (int i = 0; i < rec_cnt-1; i++) {
        substring[i] = received_chars[1 + i];
      }
      try{
      float result = atof(substring); 
      motor.target = result;
      }catch(...){
        Serial.println("Converstion failed");
      }
    } 
    rec_cnt = 0;
  }
  if (rec_cnt >= max_input_lenght){
    Serial.println("Too long"); 
    rec_cnt = 0;
  }

}
