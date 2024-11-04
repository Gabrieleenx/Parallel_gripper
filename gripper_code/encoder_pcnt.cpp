#include "encoder_pcnt.h"
//pcnt_unit = PCNT_UNIT_0;
//pcnt_channel = PCNT_CHANNEL_0;
pcnt_config_t pcnt_config;
pcnt_unit_t pcnt_unit = PCNT_UNIT_0;;
pcnt_channel_t pcnt_channel = PCNT_CHANNEL_0;
/*
  Encoder(int encA, int encB , int cpr, int index)
  - encA, encB    - encoder A and B pins
  - cpr           - counts per rotation number (cpm=ppm*4)
  - index pin     - (optional input)
*/

// ISR handler for pulse counter events
void IRAM_ATTR pcnt_intr_handler(void* arg) {
  uint32_t status = 0;
  
  // Get the event status
  pcnt_get_event_status(pcnt_unit, &status);

  // Check if the event is from the pulse counter
  if (status & PCNT_EVT_H_LIM) {
    // Handle overflow event
  } else if (status & PCNT_EVT_L_LIM) {
    // Handle underflow event
  }

  // Read the current counter value
  //pcnt_get_counter_value(pcnt_unit, &pulseCount);
}

EncoderPCNT::EncoderPCNT(int _encA, int _encB , float _ppr){

  // Encoder measurement structure init
  // hardware pins
  pinA = _encA;
  pinB = _encB;

  cpr = _ppr;

  // velocity calculation variables
  prev_Th = 0;
  pulse_per_second = 0;
  prev_pulse_counter = 0;
  prev_timestamp_us = micros();
  vel_alpha = 0.3;

  pcnt_config.pulse_gpio_num = _encA;
  pcnt_config.ctrl_gpio_num = _encB;
  pcnt_config.channel = pcnt_channel;
  pcnt_config.unit = pcnt_unit;
  pcnt_config.pos_mode = PCNT_COUNT_INC; // Count on positive edge of pulse input
  pcnt_config.neg_mode = PCNT_COUNT_DEC; // Count on negative edge of pulse input
  pcnt_config.lctrl_mode = PCNT_MODE_KEEP; // Rising edge of control input does nothing
  pcnt_config.hctrl_mode = PCNT_MODE_REVERSE; // Falling edge of control input reverse counting direction
  pcnt_config.counter_h_lim = INT16_MAX; // Set upper limit of counter
  pcnt_config.counter_l_lim = INT16_MIN; // Set lower limit of counter

}


// Sensor update function. 
void EncoderPCNT::update() {
  // Copy volatile variables in minimal-duration blocking section to ensure no interrupts are missed
  //angle_prev_ts = pulse_timestamp;
  int16_t pulseCount;
  pcnt_get_counter_value(pcnt_unit, &pulseCount);  
  // timestamp
  long timestamp_us = micros();
  full_rotations = pulseCount / (int)cpr;
  angle_prev = 2*PI * ((pulseCount) % ((int)cpr)) / ((float)cpr);
  sensor_angle = 2*PI * (pulseCount) / ((float)cpr);


  // sampling time calculation
  float Ts = (timestamp_us - prev_timestamp_us) * 1e-6f;
  // quick fix for strange cases (micros overflow)
  if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;

  // time from last impulse
  long dN = (long)pulseCount - prev_pulse_counter;


  pulse_per_second = dN/Ts;

  float vel_pre_filter = pulse_per_second / ((float)cpr) * (2*PI);

  sensor_velocity = vel_alpha * vel_pre_filter + (1- vel_alpha) * sensor_velocity;

  // save variables for next pass
  prev_timestamp_us = timestamp_us;
  prev_pulse_counter = (long)pulseCount;
}

/*
	Shaft angle calculation
*/
float EncoderPCNT::getSensorAngle(){
  return sensor_angle;
}

/*
  Shaft velocity calculation
  function using mixed time and frequency measurement technique
*/
float EncoderPCNT::getVelocity(){
  return sensor_velocity;
}

// getter for index pin
// return -1 if no index
int EncoderPCNT::needsSearch(){
  return 0;
}

// private function used to determine if encoder has index
int EncoderPCNT::hasIndex(){
  return 0;
}

// encoder initialisation of the hardware pins
// and calculation variables
void EncoderPCNT::init(){
  // velocity calculation variables
  pulse_per_second = 0;
  prev_pulse_counter = 0;
  prev_timestamp_us = micros();


  pcnt_unit_config(&pcnt_config);

  // Filter out short glitches
  pcnt_set_filter_value(pcnt_unit, 100);
  pcnt_filter_enable(pcnt_unit);

  // Enable events on limit values
  pcnt_event_enable(pcnt_unit, PCNT_EVT_H_LIM);
  pcnt_event_enable(pcnt_unit, PCNT_EVT_L_LIM);

  // Initialize counter
  pcnt_counter_pause(pcnt_unit);
  pcnt_counter_clear(pcnt_unit);

  // Register ISR handler
  pcnt_isr_register(pcnt_intr_handler, NULL, 0, NULL);

  // Enable interrupt for pulse counter
  pcnt_intr_enable(pcnt_unit);

  // Resume counting
  pcnt_counter_resume(pcnt_unit);

}

