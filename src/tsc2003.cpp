/*
 * Copyright (c) 2023, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "tsc2003/tsc2003.h"

namespace sixtron {

TSC2003::TSC2003(PinName sda, PinName scl, PinName irq, i2c_address i2c_address):
        _i2c(sda, scl), _i2c_address(i2c_address), _touch_irq(irq)
{
    _touch_irq.fall(callback(this, &TSC2003::touch_irq_handler));
    _i2c.frequency(400000);
}

int8_t TSC2003::i2c_write_command(function func, power_down power, mode mode)
{
    uint8_t cmd = (uint8_t)func << 4;
    cmd |= (uint8_t)power << 2;
    cmd |= (uint8_t)mode << 1;

    if (_i2c.write(_i2c_address << 1, (char *)&cmd, 1, true) != 0) {
        return -1;
    }

    return 0;
}

int8_t TSC2003::i2c_read_command(function func, power_down power, mode mode, uint16_t *ret)
{
    if (i2c_write_command(func, power, mode) != 0) {
        return -1;
    }

    wait_us(10);

    uint8_t data[2];
    if (_i2c.read(_i2c_address << 1, (char *)data, 2) != 0) {
        return -1;
    }

    *ret = (uint16_t)((data[0] << 4) | data[1] >> 4);

    return 0;
}

void TSC2003::set_resolution(mode mode)
{
    _mode = mode;
}

uint16_t TSC2003::temperature()
{
    uint16_t temperature = 0;

    i2c_read_command(MEASURE_TEMP0, POWERDOWN_IRQON, _mode, &temperature);

    return temperature;
}

int TSC2003::read_touch(uint16_t *x, uint16_t *y, uint16_t *z1, uint16_t *z2)
{
    _touch_irq.disable_irq();
    uint8_t ret = 0;

    ret |= i2c_read_command(MEASURE_X, POWERDOWN_IRQON, _mode, x);
    ret |= i2c_read_command(MEASURE_Y, POWERDOWN_IRQON, _mode, y);
    ret |= i2c_read_command(MEASURE_Z1, POWERDOWN_IRQON, _mode, z1);
    ret |= i2c_read_command(MEASURE_Z2, POWERDOWN_IRQON, _mode, z2);

    // i2c_read_command(MEASURE_TEMP0, POWERDOWN_IRQON, _mode, &temperature);

    _touch_irq.enable_irq();
    return ret;
}

void TSC2003::touch_irq_handler()
{
    if (_touch_callback) {
        _touch_callback.call();
    }
}

void TSC2003::attach_touch_irq(Callback<void()> func)
{
    uint16_t x, y, z1, z2;
    i2c_read_command(MEASURE_X, POWERDOWN_IRQON, _mode, &x);
    i2c_read_command(MEASURE_Y, POWERDOWN_IRQON, _mode, &y);
    i2c_read_command(MEASURE_Z1, POWERDOWN_IRQON, _mode, &z1);

    _touch_callback = func;
}

} // namespace sixtron
