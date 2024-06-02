/**
 * Nicolas Bourlet
 * June, 2024
 *
 *
 * Adapted from:
 * V. Hunter Adams
 * https://github.com/vha3/Hunter-Adams-RP2040-Demos/tree/master/Stepper_Motors
 * September, 2021
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "RP2040_StepDir_stepper.pio.h"


// Some macros for motor direction
#define COUNTERCLOCKWISE 2
#define CLOCKWISE 1
#define STOPPED 0

// Forward, reverse, and stopped pulse sequences
unsigned char pulse_sequence_forward [8]  = {0x8, 0xc, 0x4, 0x6, 0x2, 0x3, 0x1, 0x9} ;
unsigned char pulse_sequence_backward [8] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xc, 0x8, 0x9} ;
unsigned char pulse_sequence_stationary [8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0} ;

// Pointers to the addresses of the above objects (motor 1)
unsigned char * address_pointer_motor1 = &pulse_sequence_forward[0] ;
unsigned char * address_pointer_motor2 = &pulse_sequence_forward[0] ;
unsigned char * address_pointer_motor3 = &pulse_sequence_forward[0] ;
unsigned char * address_pointer_motor4 = &pulse_sequence_forward[0] ;

// Macros for setting motor steps, speed, and direction (motor1)
#define SET_DIRECTION_MOTOR_1(a) address_pointer_motor1 = (a==2) ? &pulse_sequence_forward[0] : (a==1) ? &pulse_sequence_backward[0] : &pulse_sequence_stationary[0]
#define SET_DIRECTION_MOTOR_2(a) address_pointer_motor2 = (a==2) ? &pulse_sequence_forward[0] : (a==1) ? &pulse_sequence_backward[0] : &pulse_sequence_stationary[0]
#define SET_DIRECTION_MOTOR_3(a) address_pointer_motor3 = (a==2) ? &pulse_sequence_forward[0] : (a==1) ? &pulse_sequence_backward[0] : &pulse_sequence_stationary[0]
#define SET_DIRECTION_MOTOR_4(a) address_pointer_motor4 = (a==2) ? &pulse_sequence_forward[0] : (a==1) ? &pulse_sequence_backward[0] : &pulse_sequence_stationary[0]

// Choose pio0 or pio1 (we'll have a motor on each)
PIO pio_0 = pio0;
PIO pio_1 = pio1;

// Select state machines
int pulse_sm_0 = 0;
int pulse_sm_1 = 1;
int pulse_sm_2 = 2;
int pulse_sm_3 = 3;

// DMA channels
// 0, 2, 4 sends pulse train data to motor 1, 2, 3
// 1, 3, 5 reconfigures and restarts 0, 2, 4

// Motor 1
int dma_chan_0 = 0;
int dma_chan_1 = 1;
// Motor 2
int dma_chan_2 = 2;
int dma_chan_3 = 3;
// Motor 3
int dma_chan_4 = 4;
int dma_chan_5 = 5;
// Motor 4
int dma_chan_6 = 6;
int dma_chan_7 = 7;

void setupMotor1(unsigned int in) {
    // Load PIO programs onto PIO0
    uint pio0_offset_0 = pio_add_program(pio_0, &stepper_program);
    // Initialize PIO programs
    stepper_program_init(pio_0, pulse_sm_0, pio0_offset_0, in);
    // Start the PIO programs
    pio_sm_set_enabled(pio_0, pulse_sm_0, true);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ===========================-== DMA Data Channels =================================================
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Channel Zero (sends pulse train data to PIO stepper machine)
    dma_channel_config c0 = dma_channel_get_default_config(dma_chan_0);  // default configs
    channel_config_set_transfer_data_size(&c0, DMA_SIZE_8);              // 32-bit txfers
    channel_config_set_read_increment(&c0, true);                        // no read incrementing
    channel_config_set_write_increment(&c0, false);                      // no write incrementing
    channel_config_set_dreq(&c0, DREQ_PIO0_TX0) ;                        // DREQ_PIO0_TX0 pacing (FIFO)
    channel_config_set_chain_to(&c0, dma_chan_1);                        // chain to other channel
    dma_channel_configure(
        dma_chan_0,                 // Channel to be configured
        &c0,                        // The configuration we just created
        &pio_0->txf[pulse_sm_0],        // write address (stepper PIO TX FIFO)
        address_pointer_motor1,
        8,                          // Number of transfers; in this case each is 4 byte.
        false                       // Don't start immediately.
    );

    // Channel One (reconfigures the first channel)
    dma_channel_config c1 = dma_channel_get_default_config(dma_chan_1);   // default configs
    channel_config_set_transfer_data_size(&c1, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c1, false);                        // no read incrementing
    channel_config_set_write_increment(&c1, false);                       // no write incrementing
    channel_config_set_chain_to(&c1, dma_chan_0);                         // chain to other channel
    dma_channel_configure(
        dma_chan_1,                         // Channel to be configured
        &c1,                                // The configuration we just created
        &dma_hw->ch[dma_chan_0].read_addr,  // Write address (channel 0 read address)
        &address_pointer_motor1,                   // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );

    // Start the two data channels
    dma_start_channel_mask((1u << dma_chan_0));
}

void setupMotor2(unsigned int in) {
    // Load PIO programs onto PIO0
    uint pio0_offset_1 = pio_add_program(pio_0, &stepper_program);
    // Initialize PIO programs
    stepper_program_init(pio_0, pulse_sm_1, pio0_offset_1, in);
    // Start the PIO programs
    pio_sm_set_enabled(pio_0, pulse_sm_1, true);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ===========================-== DMA Data Channels =================================================
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Channel Zero (sends pulse train data to PIO stepper machine)
    dma_channel_config c2 = dma_channel_get_default_config(dma_chan_2);  // default configs
    channel_config_set_transfer_data_size(&c2, DMA_SIZE_8);              // 32-bit txfers
    channel_config_set_read_increment(&c2, true);                        // no read incrementing
    channel_config_set_write_increment(&c2, false);                      // no write incrementing
    channel_config_set_dreq(&c2, DREQ_PIO0_TX1) ;                        // DREQ_PIO0_TX0 pacing (FIFO)
    channel_config_set_chain_to(&c2, dma_chan_3);                        // chain to other channel
    dma_channel_configure(
        dma_chan_2,                 // Channel to be configured
        &c2,                        // The configuration we just created
        &pio_0->txf[pulse_sm_1],        // write address (stepper PIO TX FIFO)
        address_pointer_motor2,
        8,                          // Number of transfers; in this case each is 4 byte.
        false                       // Don't start immediately.
    );

    // Channel One (reconfigures the first channel)
    dma_channel_config c3 = dma_channel_get_default_config(dma_chan_3);   // default configs
    channel_config_set_transfer_data_size(&c3, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c3, false);                        // no read incrementing
    channel_config_set_write_increment(&c3, false);                       // no write incrementing
    channel_config_set_chain_to(&c3, dma_chan_2);                         // chain to other channel
    dma_channel_configure(
        dma_chan_3,                         // Channel to be configured
        &c3,                                // The configuration we just created
        &dma_hw->ch[dma_chan_2].read_addr,  // Write address (channel 0 read address)
        &address_pointer_motor2,                   // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );

    // Start the two data channels
    dma_start_channel_mask((1u << dma_chan_2));
}

void setupMotor3(unsigned int in) {
    // Load PIO programs onto PIO0
    uint pio0_offset_2 = pio_add_program(pio_0, &stepper_program);
    // Initialize PIO programs
    stepper_program_init(pio_0, pulse_sm_2, pio0_offset_2, in);
    // Start the PIO programs
    pio_sm_set_enabled(pio_0, pulse_sm_2, true);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ===========================-== DMA Data Channels =================================================
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Channel Zero (sends pulse train data to PIO stepper machine)
    dma_channel_config c4 = dma_channel_get_default_config(dma_chan_4);  // default configs
    channel_config_set_transfer_data_size(&c4, DMA_SIZE_8);              // 32-bit txfers
    channel_config_set_read_increment(&c4, true);                        // no read incrementing
    channel_config_set_write_increment(&c4, false);                      // no write incrementing
    channel_config_set_dreq(&c4, DREQ_PIO0_TX2) ;                        // DREQ_PIO0_TX0 pacing (FIFO)
    channel_config_set_chain_to(&c4, dma_chan_5);                        // chain to other channel
    dma_channel_configure(
        dma_chan_4,                 // Channel to be configured
        &c4,                        // The configuration we just created
        &pio_0->txf[pulse_sm_2],        // write address (stepper PIO TX FIFO)
        address_pointer_motor3,
        8,                          // Number of transfers; in this case each is 4 byte.
        false                       // Don't start immediately.
    );

    // Channel One (reconfigures the first channel)
    dma_channel_config c5 = dma_channel_get_default_config(dma_chan_5);   // default configs
    channel_config_set_transfer_data_size(&c5, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c5, false);                        // no read incrementing
    channel_config_set_write_increment(&c5, false);                       // no write incrementing
    channel_config_set_chain_to(&c5, dma_chan_4);                         // chain to other channel
    dma_channel_configure(
        dma_chan_5,                         // Channel to be configured
        &c5,                                // The configuration we just created
        &dma_hw->ch[dma_chan_4].read_addr,  // Write address (channel 0 read address)
        &address_pointer_motor3,                   // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );

    // Start the two data channels
    dma_start_channel_mask((1u << dma_chan_4));
}

void setupMotor4(unsigned int in) {
    // Load PIO programs onto PIO0
    uint pio0_offset_3 = pio_add_program(pio_0, &stepper_program);
    // Initialize PIO programs
    stepper_program_init(pio_0, pulse_sm_3, pio0_offset_3, in);
    // Start the PIO programs
    pio_sm_set_enabled(pio_0, pulse_sm_3, true);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ===========================-== DMA Data Channels =================================================
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Channel Zero (sends pulse train data to PIO stepper machine)
    dma_channel_config c6 = dma_channel_get_default_config(dma_chan_6);  // default configs
    channel_config_set_transfer_data_size(&c6, DMA_SIZE_8);              // 32-bit txfers
    channel_config_set_read_increment(&c6, true);                        // no read incrementing
    channel_config_set_write_increment(&c6, false);                      // no write incrementing
    channel_config_set_dreq(&c6, DREQ_PIO0_TX3) ;                        // DREQ_PIO0_TX0 pacing (FIFO)
    channel_config_set_chain_to(&c6, dma_chan_7);                        // chain to other channel
    dma_channel_configure(
        dma_chan_6,                 // Channel to be configured
        &c6,                        // The configuration we just created
        &pio_0->txf[pulse_sm_3],        // write address (stepper PIO TX FIFO)
        address_pointer_motor4,
        8,                          // Number of transfers; in this case each is 4 byte.
        false                       // Don't start immediately.
    );

    // Channel One (reconfigures the first channel)
    dma_channel_config c7 = dma_channel_get_default_config(dma_chan_7);   // default configs
    channel_config_set_transfer_data_size(&c7, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c7, false);                        // no read incrementing
    channel_config_set_write_increment(&c7, false);                       // no write incrementing
    channel_config_set_chain_to(&c7, dma_chan_6);                         // chain to other channel
    dma_channel_configure(
        dma_chan_7,                         // Channel to be configured
        &c7,                                // The configuration we just created
        &dma_hw->ch[dma_chan_6].read_addr,  // Write address (channel 0 read address)
        &address_pointer_motor4,                   // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );

    // Start the two data channels
    dma_start_channel_mask((1u << dma_chan_6));
}
