/*
 * Copyright (c) 2023, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CATIE_SIXTRON_TSC2003_H_
#define CATIE_SIXTRON_TSC2003_H_

#include "mbed.h"

/**
 * @brief Namespace for all classes and functions related to the 6TRON project.
 */
namespace sixtron {

class TSC2003 {
public:
    typedef enum {
        ADDRESS1 = (0x48), // A0 and A1 connected to GND
        ADDRESS2 = (0x49), // A0 connected to VDD and A1 connected to GND
        ADDRESS3 = (0x4A), // A0 connected to GND and A1 connected to VCC
        ADDRESS4 = (0x4B), // A0 and A1 connected to VDD
    } i2c_address;

    typedef enum {
        MEASURE_TEMP0 = 0,
        MEASURE_BAT1 = 1,
        MEASURE_IN1 = 2,
        MEASURE_TEMP1 = 4,
        MEASURE_BAT2 = 5,
        MEASURE_IN2 = 6,
        ACTIVATE_X = 8,
        ACTIVATE_Y = 9,
        ACTIVATE_YPLUS_X = 10,
        MEASURE_X = 12,
        MEASURE_Y = 13,
        MEASURE_Z1 = 14,
        MEASURE_Z2 = 15
    } function;

    typedef enum {
        POWERDOWN_IRQON = 0,
        INTREFOFF_ADCON_IRQOFF = 1,
        INTREFON_ADCOFF_IRQON = 2,
        INTREFON_ADCON_IRQOFF = 3
    } power_down;

    typedef enum {
        _12BITS = 0,
        _8BITS = 1
    } mode;

    TSC2003(PinName sda, PinName scl, PinName irq, i2c_address i2c_address);

    int read_touch(uint16_t *x, uint16_t *y, uint16_t *z1, uint16_t *z2);

    void set_resolution(mode mode);

    uint16_t temperature();

    void attach_touch_irq(Callback<void()> func);

private:
    I2C _i2c;
    i2c_address _i2c_address;
    InterruptIn _touch_irq;

    Callback<void()> _touch_callback;

    mode _mode;

    int8_t i2c_write_command(function func, power_down power, mode mode);

    int8_t i2c_read_command(function func, power_down power, mode mode, uint16_t *ret);

    void touch_irq_handler();
};

} // namespace sixtron

#endif // CATIE_SIXTRON_TSC2003_H_
