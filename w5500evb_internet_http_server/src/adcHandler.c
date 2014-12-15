#include "board.h"

static uint8_t enabled_adc_channel[AINn] = {Off, };

// param: adc_sel - Bit enable for each ADC channel
// 0000 0001 - A0_PIN
// 0000 0010 - A1_PIN
// 0000 0100 - A2_PIN
// 0000 1000 - A3_PIN
// 0001 0000 - A4_PIN
// 0010 0000 - A5_PIN
// 0100 0000 - AIN_PTM_TEMP
void ADC_Init(uint8_t ain_sel)
{
	static ADC_CLOCK_SETUP_T ADCSetup;

	if(!ain_sel) return;

	/* Init ADC Pin MUX */
	if((ain_sel & 0x01) == 0x01)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PORT, ain_pins[A0], FUNC2);
		enabled_adc_channel[A0] = On;
	}

	if((ain_sel & 0x02) == 0x02)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PORT, ain_pins[A1], FUNC2);
		enabled_adc_channel[A1] = On;
	}

	if((ain_sel & 0x04) == 0x04)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PORT, ain_pins[A2], FUNC2);
		enabled_adc_channel[A2] = On;
	}

	if((ain_sel & 0x08) == 0x08)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PORT, ain_pins[A3], FUNC2);
		enabled_adc_channel[A3] = On;
	}

	if((ain_sel & 0x10) == 0x10)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PORT, ain_pins[A4], FUNC2);
		enabled_adc_channel[A4] = On;
	}

	if((ain_sel & 0x20) == 0x20)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PORT, ain_pins[A5], FUNC2);
		enabled_adc_channel[A5] = On;
	}

	if((ain_sel & 0x40) == 0x40)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_PORT, ain_pins[AIN], FUNC2); // AIN_PTM_TEMP
		enabled_adc_channel[AIN] = On;
	}

	/* Init ADC */
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
}

uint16_t get_ADC_val(uint8_t ain_idx)
{
	uint8_t i;
	uint16_t dataADC;

	if(enabled_adc_channel[ain_idx] == Off) return 0;

	/* ADC Channel Enable */
	for(i = 0; i < AINn; i++)
	{
		Chip_ADC_EnableChannel(LPC_ADC, adc_channels[i], DISABLE);
	}
	Chip_ADC_EnableChannel(LPC_ADC, adc_channels[ain_idx], ENABLE);

	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

	/* Waiting for A/D conversion complete */
	while (Chip_ADC_ReadStatus(LPC_ADC, adc_channels[ain_idx], ADC_DR_DONE_STAT) != SET) {}

	/* Read ADC value */
	Chip_ADC_ReadValue(LPC_ADC, adc_channels[ain_idx], &dataADC);

	return dataADC;
}
