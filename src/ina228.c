// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Lars Görner

#include <stdint.h>
#include <system/stm32f0xx.h>
#include <ina228.h>

#define INA228_SADDR    0x40    /**< INA228 Slave Address (A1 = A0 = GND) */

//---| INA228 Registers
#define REG_CONFIG              0x0     /**< Configuration Register */
#define REG_ADC_CONFIG          0x1     /**< ADC Configuration Register */
#define REG_SHUNT_CAL           0x2     /**< Shunt Calibration Register */
#define REG_SHUNT_TEMPCO        0x3     /**< Shunt Temperature Coefficient Register */
#define REG_VSHUNT              0x4     /**< Shunt Voltage Measurement Register */
#define REG_VBUS                0x5     /**< Bus Voltage Measurement Register */
#define REG_DIETEMP             0x6     /**< Temperature Measurement Register */
#define REG_CURRENT             0x7     /**< Current Result Register */
#define REG_POWER               0x8     /**< Power Result Register */
#define REG_ENERGY              0x9     /**< Energy Result Register */
#define REG_CHARGE              0xA     /**< Charge Result Register */
#define REG_DIAG_ALRT           0xB     /**< Diagnostic Flags & Alert Register */
#define REG_SOVL                0xC     /**< Shunt Overvoltage Threshold Register */
#define REG_SUVL                0xD     /**< Shunt Undervoltage Threshold Register */
#define REG_BOVL                0xE     /**< Bus Overvoltage Threshold Register */
#define REG_BUVL                0xF     /**< Bus Undervoltage Threshold Register */
#define REG_TEMP_LIMIT          0x10    /**< Temperature Over-Limit Threshold Register */
#define REG_PWR_LIMIT           0x11    /**< Power Over-Limit Threshold Register */
#define REG_MANUFACTURER_ID     0x3E    /**< Manufacturer ID */
#define REG_DEVICE_ID           0x3F    /**< Device ID */

#define INA_BUS         I2C1

static char ina_bus_buf[4];

__STATIC_INLINE uint32_t ina228_rx_ready(void)
{
        return READ_BIT(INA_BUS->ISR, I2C_ISR_RXNE);
}

__STATIC_INLINE uint32_t ina228_tx_ready(void)
{
        return READ_BIT(INA_BUS->ISR, I2C_ISR_TXIS);
}

__STATIC_INLINE void ina228_write(const void * dat, unsigned dat_len)
{
        const uint8_t * write_buf = dat;

        // set slave address
        CLEAR_BIT(INA_BUS->CR2, I2C_CR2_SADD);
        SET_BIT(INA_BUS->CR2, ((INA228_SADDR << (I2C_CR2_SADD_Pos + 1)) & I2C_CR2_SADD_Msk));

        // set direction
        CLEAR_BIT(INA_BUS->CR2, I2C_CR2_RD_WRN); // write transfer

        // set number of bytes
        CLEAR_BIT(INA_BUS->CR2, I2C_CR2_NBYTES);
        SET_BIT(INA_BUS->CR2, ((dat_len << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES_Msk));

        // start
        SET_BIT(INA_BUS->CR2, I2C_CR2_START);

        // send data
        while (dat_len--) {
                while (!ina228_tx_ready());
                WRITE_REG(INA_BUS->TXDR, *write_buf++);
        }

        while (!READ_BIT(INA_BUS->ISR, I2C_ISR_TC)); // Wait tf complete

        // stop
        SET_BIT(INA_BUS->CR2, I2C_CR2_STOP);
}

__STATIC_INLINE void ina228_read(void * dat, unsigned dat_len)
{
        uint8_t * read_buf = dat;

        // set slave address
        CLEAR_BIT(INA_BUS->CR2, I2C_CR2_SADD);
        SET_BIT(INA_BUS->CR2, ((INA228_SADDR << (I2C_CR2_SADD_Pos + 1)) & I2C_CR2_SADD_Msk));

        // set direction
        SET_BIT(INA_BUS->CR2, I2C_CR2_RD_WRN); // read transfer

        // set number of bytes
        CLEAR_BIT(INA_BUS->CR2, I2C_CR2_NBYTES);
        SET_BIT(INA_BUS->CR2, ((dat_len << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES_Msk));

        // start
        SET_BIT(INA_BUS->CR2, I2C_CR2_START);

        while (dat_len--) {
                while (!ina228_rx_ready());
                *read_buf++ = READ_REG(INA_BUS->RXDR);
        }

        while (!READ_BIT(INA_BUS->ISR, I2C_ISR_TC)); // Wait tf complete

        // stop
        SET_BIT(INA_BUS->CR2, I2C_CR2_STOP);
}

void ina228_init(void)
{
        const uint16_t shunt_cal_val = 1500; // max I = 1A

        // init i2c
        CLEAR_BIT(INA_BUS->CR1, I2C_CR1_PE);
        CLEAR_BIT(INA_BUS->CR1, I2C_CR1_ANFOFF);
        // CR1 -> ANFOFF, DNF
        // Timing: Standard-mode (100kHz)
        WRITE_REG(INA_BUS->TIMINGR, 
                ((0x1 << I2C_TIMINGR_PRESC_Pos) & I2C_TIMINGR_PRESC_Msk) |
                ((0xC7 << I2C_TIMINGR_SCLL_Pos) & I2C_TIMINGR_SCLL_Msk) |
                ((0xC3 << I2C_TIMINGR_SCLH_Pos) & I2C_TIMINGR_SCLH_Msk) |
                ((0x2 << I2C_TIMINGR_SDADEL_Pos) & I2C_TIMINGR_SDADEL_Msk) |
                ((0x4 << I2C_TIMINGR_SCLDEL_Pos) & I2C_TIMINGR_SCLDEL_Msk));
        // CR1 -> NOSTRECH
        SET_BIT(INA_BUS->CR1, I2C_CR1_PE);

        // config device
        // reset device
        ina_bus_buf[0] = REG_CONFIG;
        ina_bus_buf[1] = 0x80;
        ina_bus_buf[2] = 0x00;
        ina228_write(ina_bus_buf, 3);

        ina_bus_buf[0] = REG_CONFIG;
        ina_bus_buf[1] = 0;
        ina_bus_buf[2] = 1 << 4; // ADCRANGE +-40.96mV
        ina228_write(ina_bus_buf, 3);

        ina_bus_buf[0] = REG_ADC_CONFIG;
        ina_bus_buf[1] = 
                1 | // VSHCT = 2074us
                (0x6 << 1) | // VBUSCT = 2074us
                (0xB << 4); // continous shunt & bus voltage
        ina_bus_buf[2] =
                0x3 | // AVG = 64
                (0x2 << 6); // VSHCT = 2074us
        ina228_write(ina_bus_buf, 3);

        ina_bus_buf[0] = REG_SHUNT_CAL;
        ina_bus_buf[1] = (shunt_cal_val >> 8) & 0xFF;
        ina_bus_buf[2] = shunt_cal_val & 0xFF;
        ina228_write(ina_bus_buf, 3);

        // shunt temp co necessary?

        // setup alerts?
}

const char * ina228_get_manufacturer_id(void)
{
        ina_bus_buf[0] = REG_MANUFACTURER_ID;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 2);
        ina_bus_buf[2] = '\0';
        return ina_bus_buf;
}

uint16_t ina228_get_device_id(void)
{
        uint16_t dev_id;

        ina_bus_buf[0] = REG_DEVICE_ID;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 2);
        dev_id = ina_bus_buf[0] >> 4;
        dev_id |= ina_bus_buf[1] << 4;
        return dev_id;
}

int ina228_get_current(void)
{
        int ret_val;

        ina_bus_buf[0] = REG_CURRENT;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 3);
        ret_val =
                (ina_bus_buf[2] >> 4) |
                (ina_bus_buf[1] << 4) |
                (ina_bus_buf[0] << 12);
        
        // check if negative
        if (ret_val & 0x00080000) {
                ret_val |= 0xFFF00000;
        }
        return (ret_val * 1000 * 1000) >> 19; // return as uA
}

uint32_t ina228_get_voltage(void)
{
        uint32_t ret;

        ina_bus_buf[0] = REG_VBUS;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 3);
        ret = ina_bus_buf[2] >> 4;
        ret |= ina_bus_buf[1] << 4;
        ret |= ina_bus_buf[0] << 12;
        return ret;
}

uint16_t ina228_get_temperature(void)
{
        uint16_t ret;

        ina_bus_buf[0] = REG_DIETEMP;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 2);
        ret = ina_bus_buf[0] >> 4;
        ret |= ina_bus_buf[1] << 4;
        return ret;
}

uint32_t ina228_get_power(void)
{
        uint32_t ret;

        ina_bus_buf[0] = REG_POWER;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 3);
        ret = ina_bus_buf[0] >> 4;
        ret |= ina_bus_buf[1] << 4;
        ret |= ina_bus_buf[2] << 12;
        return ret;
}

uint16_t ina228_get_energy(void)
{
        ina_bus_buf[0] = REG_CURRENT;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 3);
        return (ina_bus_buf[2] << 8) | ina_bus_buf[1];
}

uint16_t ina228_get_charge(void)
{
        ina_bus_buf[0] = REG_CURRENT;
        ina228_write(ina_bus_buf, 1);
        ina228_read(ina_bus_buf, 3);
        return (ina_bus_buf[2] << 8) | ina_bus_buf[1];
}

