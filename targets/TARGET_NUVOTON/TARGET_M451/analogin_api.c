/* mbed Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "analogin_api.h"

#if DEVICE_ANALOGIN

#include "cmsis.h"
#include "pinmap.h"
#include "PeripheralPins.h"
#include "nu_modutil.h"

struct nu_adc_var {
    uint32_t    en_msk;
};

static struct nu_adc_var adc0_var = {
    .en_msk = 0
};
static struct nu_adc_var adc1_var = {
    .en_msk = 0
};
static struct nu_adc_var adc2_var = {
    .en_msk = 0
};
static struct nu_adc_var adc3_var = {
    .en_msk = 0
};
static struct nu_adc_var adc4_var = {
    .en_msk = 0
};
static struct nu_adc_var adc5_var = {
    .en_msk = 0
};
static struct nu_adc_var adc6_var = {
    .en_msk = 0
};
static struct nu_adc_var adc7_var = {
    .en_msk = 0
};
static struct nu_adc_var adc8_var = {
    .en_msk = 0
};
static struct nu_adc_var adc9_var = {
    .en_msk = 0
};
static struct nu_adc_var adc10_var = {
    .en_msk = 0
};
static struct nu_adc_var adc11_var = {
    .en_msk = 0
};
static struct nu_adc_var adc12_var = {
    .en_msk = 0
};
static struct nu_adc_var adc13_var = {
    .en_msk = 0
};
static struct nu_adc_var adc14_var = {
    .en_msk = 0
};
static struct nu_adc_var adc15_var = {
    .en_msk = 0
};

static const struct nu_modinit_s adc_modinit_tab[] = {
    {ADC_0_0, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc0_var},
    {ADC_0_1, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc1_var},
    {ADC_0_2, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc2_var},
    {ADC_0_3, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc3_var},
    {ADC_0_4, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc4_var},
    {ADC_0_5, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc5_var},
    {ADC_0_6, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc6_var},
    {ADC_0_7, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc7_var},
    {ADC_0_8, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc8_var},
    {ADC_0_9, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc9_var},
    {ADC_0_10, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc10_var},
    {ADC_0_11, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc11_var},
    {ADC_0_12, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc12_var},
    {ADC_0_13, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc13_var},
    {ADC_0_14, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc14_var},
    {ADC_0_15, EADC_MODULE, 0, CLK_CLKDIV0_EADC(8), EADC_RST, ADC00_IRQn, &adc15_var},
};

void analogin_init(analogin_t *obj, PinName pin)
{   
    obj->adc = (ADCName) pinmap_peripheral(pin, PinMap_ADC);
    MBED_ASSERT(obj->adc != (ADCName) NC);

    const struct nu_modinit_s *modinit = get_modinit(obj->adc, adc_modinit_tab);
    MBED_ASSERT(modinit != NULL);
    MBED_ASSERT(modinit->modname == obj->adc);
    
    EADC_T *eadc_base = (EADC_T *) NU_MODBASE(obj->adc);
    
    // NOTE: All channels (identified by ADCName) share a ADC module. This reset will also affect other channels of the same ADC module.
    if (! ((struct nu_adc_var *) modinit->var)->en_msk) {
        // Reset this module if no channel enabled
        SYS_ResetModule(modinit->rsetidx);
        
        // Select clock source of paired channels
        CLK_SetModuleClock(modinit->clkidx, modinit->clksrc, modinit->clkdiv);
        // Enable clock of paired channels
        CLK_EnableModuleClock(modinit->clkidx);
        
        // Power on ADC
        //ADC_POWER_ON(ADC);
        
        // Set the ADC internal sampling time, input mode as single-end and enable the A/D converter
        EADC_Open(eadc_base, EADC_CTL_DIFFEN_SINGLE_END);
        EADC_SetInternalSampleTime(eadc_base, 6);
    }
    
    uint32_t chn =  NU_MODSUBINDEX(obj->adc);
    
    // Wire pinout
    pinmap_pinout(pin, PinMap_ADC);
    
    // Configure the sample module Nmod for analog input channel Nch and software trigger source
    EADC_ConfigSampleModule(EADC, chn, EADC_SOFTWARE_TRIGGER, chn);
    
    ((struct nu_adc_var *) modinit->var)->en_msk |= 1 << chn;
}

uint16_t analogin_read_u16(analogin_t *obj)
{
    EADC_T *eadc_base = (EADC_T *) NU_MODBASE(obj->adc);
    uint32_t chn =  NU_MODSUBINDEX(obj->adc);
    
    EADC_START_CONV(eadc_base, 1 << chn);
    while (EADC_GET_DATA_VALID_FLAG(eadc_base, 1 << chn) != (1 << chn));
    uint16_t conv_res_12 = EADC_GET_CONV_DATA(eadc_base, chn);
    // Just 12 bits are effective. Convert to 16 bits.
    // conv_res_12: 0000 b11b10b9b8 b7b6b5b4 b3b2b1b0
    // conv_res_16: b11b10b9b8 b7b6b5b4 b3b2b1b0 b11b10b9b8
    uint16_t conv_res_16 = (conv_res_12 << 4) | (conv_res_12 >> 8);
    
    return conv_res_16;
}

float analogin_read(analogin_t *obj)
{
    uint16_t value = analogin_read_u16(obj);
    return (float) value * (1.0f / (float) 0xFFFF);
}

#endif
