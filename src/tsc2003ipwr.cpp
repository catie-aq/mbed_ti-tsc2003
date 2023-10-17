/*
 * Copyright (c) 2023, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "tsc2003ipwr/tsc2003ipwr.h"

namespace sixtron {

TSC2003IPWR::TSC2003IPWR(PinName sda, PinName scl, PinName irq, i2c_address i2c_address):
        _i2c(sda, scl), _i2c_address(i2c_address)
{
}

int8_t TSC2003IPWR::i2c_write_command(function func, power_down power, mode mode)
{
    uint8_t cmd = (uint8_t)func << 4;
    cmd |= (uint8_t)power << 2;
    cmd |= (uint8_t)mode << 1;

    if (_i2c.write(_i2c_address << 1, (char *)&cmd, 1, true) != 0) {
        return -1;
    }

    return 0;
}

int8_t TSC2003IPWR::i2c_read_command(function func, power_down power, mode mode, uint16_t *ret)
{
    if (i2c_write_command(func, power, mode) != 0) {
        return -1;
    }

    ThisThread::sleep_for(1ms);

    uint8_t data[2];
    if (_i2c.read(_i2c_address << 1, (char *)data, 2) != 0) {
        return -1;
    }

    *ret = (uint16_t)((data[0] << 4) | data[1] >> 4);

    return 0;
}

void TSC2003IPWR::set_resolution(mode mode)
{
    _mode = mode;
}

uint16_t TSC2003IPWR::temperature()
{
    uint16_t temperature = 0;

    i2c_read_command(MEASURE_TEMP0, POWERDOWN_IRQON, _mode, &temperature);

    return temperature;
}

int TSC2003IPWR::read_touch(uint16_t *x, uint16_t *y, uint16_t *z1, uint16_t *z2)
{
    uint8_t ret = 0;

    ret |= i2c_read_command(MEASURE_X, INTREFON_ADCON_IRQOFF, _mode, x);
    ret |= i2c_read_command(MEASURE_Y, INTREFON_ADCON_IRQOFF, _mode, y);
    // ret |= i2c_read_command(MEASURE_Z1, INTREFON_ADCON_IRQOFF, _mode, z1);
    // ret |= i2c_read_command(MEASURE_Z2, INTREFON_ADCON_IRQOFF, _mode, z2);

    // ret |= i2c_write_command(MEASURE_TEMP0, POWERDOWN_IRQON, _mode);

    return ret;
}

} // namespace sixtron
