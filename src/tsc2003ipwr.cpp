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

int TSC2003IPWR::i2c_write_command(function func, power_down power, mode mode)
{
    uint8_t cmd = (uint8_t)func << 4;
    cmd |= (uint8_t)power << 2;
    cmd |= (uint8_t)mode << 1;

    if (_i2c.write(_i2c_address, (char *)&mode, 1, true) != 0) {
        return -1;
    }
}

} // namespace sixtron
