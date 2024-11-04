#ifndef ENCODER_PCNT_H
#define ENCODER_PCNT_H

#include "Arduino.h"
#include <common/foc_utils.h>
#include <common/time_utils.h>
#include <common/base_classes/Sensor.h>

#include <driver/pcnt.h>
   // Pulse Counter unit and channel
extern pcnt_config_t pcnt_config;
extern pcnt_unit_t pcnt_unit;
extern pcnt_channel_t pcnt_channel;


class EncoderPCNT: public Sensor{
 public:
    /**
    Encoder class constructor
    @param encA  encoder B pin
    @param encB  encoder B pin
    @param ppr  impulses per rotation  (cpr=ppr*4)
        */
    EncoderPCNT(int encA, int encB , float ppr);

    /** encoder initialise pins */
    void init() override;
    /**
     *  function enabling hardware interrupts for the encoder channels with provided callback functions
     *  if callback is not provided then the interrupt is not enabled
     * 
     * @param doA pointer to the A channel interrupt handler function
     * @param doB pointer to the B channel interrupt handler function
     * @param doIndex pointer to the Index channel interrupt handler function
     * 
     */

    // pins A and B
    int pinA; //!< encoder hardware pin A
    int pinB; //!< encoder hardware pin B
    
    // Encoder configuration
    float cpr;//!< encoder cpr number

    // Abstract functions of the Sensor class implementation
    /** get current angle (rad) */
    float getSensorAngle() override;
    /**  get current angular velocity (rad/s) */
    float getVelocity() override;
    virtual void update() override;

    /**
     * returns 0 if it does need search for absolute zero
     * 0 - encoder without index 
     * 1 - ecoder with index
     */
    int needsSearch() override;

  private:
    int hasIndex(); //!< function returning 1 if encoder has index pin and 0 if not.
    volatile long pulse_counter;//!< current pulse counter
    volatile long pulse_timestamp;//!< last impulse timestamp in us
    volatile int A_active; //!< current active states of A channel
    volatile int B_active; //!< current active states of B channel
    volatile int I_active; //!< current active states of Index channel
    volatile bool index_found = false; //!< flag stating that the index has been found

    // velocity calculation variables
    float prev_Th, pulse_per_second;
    long prev_pulse_counter, prev_timestamp_us;

    float sensor_angle, sensor_velocity;
    float vel_alpha;

};

#endif // ENCODER_PCNT_H