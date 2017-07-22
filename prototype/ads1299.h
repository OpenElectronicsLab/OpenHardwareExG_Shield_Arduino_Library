#ifndef ADS1299_H
#define ADS1299_H

#include <stdint.h>

#ifdef __cplusplus
namespace ADS1299 {
#endif

	static const float reference_voltage = 4.5;

	enum bit_number {
		bit15 = 1<<15,
		bit14 = 1<<14,
		bit13 = 1<<13,
		bit12 = 1<<12,
		bit11 = 1<<11,
		bit10 = 1<<10,
		bit9 = 1<<9,
		bit8 = 1<<8,
		bit7 = 1<<7,
		bit6 = 1<<6,
		bit5 = 1<<5,
		bit4 = 1<<4,
		bit3 = 1<<3,
		bit2 = 1<<2,
		bit1 = 1<<1,
		bit0 = 1<<0
	};

	enum spi_command {
		// system commands
		WAKEUP = 0x02,
		STANDBY = 0x04,
		RESET = 0x06,
		START = 0x08,
		STOP = 0x0a,

		// read commands
		RDATAC = 0x10,
		SDATAC = 0x11,
		RDATA = 0x12,

		// register commands
		RREG = 0x20,
		WREG = 0x40
	};

	enum reg {
		// device settings
		ID = 0x00,

		// global settings
		CONFIG1 = 0x01,
		CONFIG2 = 0x02,
		CONFIG3 = 0x03,
		LOFF = 0x04,

		// channel specific settings
		CHnSET = 0x04,
		CH1SET = CHnSET + 1,
		CH2SET = CHnSET + 2,
		CH3SET = CHnSET + 3,
		CH4SET = CHnSET + 4,
		CH5SET = CHnSET + 5,
		CH6SET = CHnSET + 6,
		CH7SET = CHnSET + 7,
		CH8SET = CHnSET + 8,
		BIAS_SENSP = 0x0d, //is RLD_SENSP in 1298
		BIAS_SENSN = 0x0e, //is RLD_SENSN in 1298
		LOFF_SENSP = 0x0f,
		LOFF_SENSN = 0x10,
		LOFF_FLIP = 0x11,

		// lead off status
		LOFF_STATP = 0x12,
		LOFF_STATN = 0x13,

		// other
		GPIO = 0x14,
		MISC1 = 0x15, //is PACE in 1298
		MISC2 = 0x16, //is RESP in 1298
		CONFIG4 = 0x17
		// WCT1 = 0x18,
		// WCT2 = 0x19
	};

	enum ID_bits {
		REV_ID3 = bit7,
		REV_ID2 = bit6,
		REV_ID1 = bit5,
		ID_4_on = bit4,
		DEV_ID2 = bit3,
		DEV_ID1 = bit2,
		NU_CH2 = bit1,
		NU_CH1 = bit0,

		ID_const = ID_4_on,

		// bit 4 (ID_const) + binary 1110
		ID_ADS1299_VAL = ID_const | 0x0E
	};

	enum CONFIG1_bits {
		CONFIG1_7_on = bit7,
		DAISY_EN = bit6, //inverted value
		CLK_EN = bit5,
		CONFIG1_4_on = bit4,
		CONFIG1_3_off = bit3,
		DR2 = bit2,
		DR1 = bit1,
		DR0 = bit0,

		CONFIG1_const = (CONFIG1_7_on | CONFIG1_4_on), //CONFIG1_3_off

		DAISY_EN_enabled = 0,
		DAISY_EN_disabled = DAISY_EN,

		CLK_EN_enabled = CLK_EN,
		CLK_EN_disabled = 0,

		DR_16k_SPS = 0x00,
		DR_8k_SPS = DR0,
		DR_4k_SPS = DR1,
		DR_2k_SPS = (DR1 | DR0),
		DR_1k_SPS = DR2,
		DR_500_SPS = (DR2 | DR0),
		DR_250_SPS = (DR2 | DR1)
	};

	enum CONFIG2_bits {
		CONFIG2_7_on = bit7,
		CONFIG2_6_on = bit6,
		CONFIG2_5_off = bit5,
		INT_CAL = bit4,
		CONFIG2_3_off = bit3,
		CAL_AMP0 = bit2,
		CAL_FREQ1 = bit1,
		CAL_FREQ0 = bit0,

		CONFIG2_const = (CONFIG2_7_on | CONFIG2_6_on),
		// CONFIG2_5_off, CONFIG2_3_off

		INT_CAL_external = 0x00,
		INT_CAL_internal = INT_CAL,

		// TODO check values in datasheet against real volts
		CAL_AMP0_low = 0x00, // 0.8mV ?
		CAL_AMP0_high = CAL_AMP0, // 1.6mV ?

		// (2_048_000 Hz)/(2^21) == 0.9765625 Hz
		CAL_FREQ_one_Hz = 0x00,
		// (2_048_000 Hz)/(2^20) == 1.953125 Hz
		CAL_FREQ_two_Hz = CAL_FREQ0,

		CAL_FREQ_DC = (CAL_FREQ1 | CAL_FREQ0)
	};


	enum CONFIG3_bits {
		PD_REFBUF = bit7,
		CONFIG3_6_on = bit6,
		CONFIG3_5_on = bit5,
		BIAS_MEAS = bit4,
		BIASREF_INT = bit3,
		PD_BIAS = bit2, // inverted value
		BIAS_LOFF_SENS = bit1,
		BIAS_STAT = bit0,

		// TODO improve names; ~PD~ name consistency?
		PD_REFBUF_reference_buffer_powered_down = 0,
		PD_REFBUF_reference_buffer_enabled = PD_REFBUF,

		PD_BIAS_bias_buffer_powered_down = 0,
		PD_BIAS_bias_buffer_enabled = PD_BIAS,

		CONFIG3_const = (CONFIG3_6_on | CONFIG3_5_on),
	};

	enum LOFF_bits {
		COMP_TH2 = bit7,
		COMP_TH1 = bit6,
		COMP_TH0 = bit5,
		LOFF_4_off = bit4,
		ILEAD_OFF1 = bit3,
		ILEAD_OFF0 = bit2,
		FLEAD_OFF1 = bit1,
		FLEAD_OFF0 = bit0,

		LOFF_const = 0x00, // LOFF_4_off

		COMP_TH_95 = 0x00,
		COMP_TH_92_5 = COMP_TH0,
		COMP_TH_90 = COMP_TH1,
		COMP_TH_87_5 = (COMP_TH1 | COMP_TH0),
		COMP_TH_85 = COMP_TH2,
		COMP_TH_80 = (COMP_TH2 | COMP_TH0),
		COMP_TH_75 = (COMP_TH2 | COMP_TH1),
		COMP_TH_70 = (COMP_TH2 | COMP_TH1 | COMP_TH0),

		ILEAD_OFF_6nA = 0x00,
		ILEAD_OFF_24nA = ILEAD_OFF0,
		ILEAD_OFF_6uA = ILEAD_OFF1,
		ILEAD_OFF_24uA = (ILEAD_OFF1 | ILEAD_OFF0),

		FLEAD_OFF_DC = 0x00,
		FLEAD_OFF_AC_7_8Hz = FLEAD_OFF0, // (SYS_CLK / (2^18))
		FLEAD_OFF_AC_31_2Hz = FLEAD_OFF1, // (SYS_CLK / (2^16))
		FLEAD_OFF_AC_DR_DIV_4 = (FLEAD_OFF1 | FLEAD_OFF0) // (DR / 4)
	};

	// CHnSET represents CH1SET, CH2SET, CH3SET, etc.
	enum CHnSET_bits {
		PDn = bit7, // e.g. PD1
		PD_n = bit7,
		GAINn2 = bit6,
		GAINn1 = bit5,
		GAINn0 = bit4, // e.g. GAIN10
		SRB2 = bit3,	// actually ADS1299 specific
		MUXn2 = bit2,
		MUXn1 = bit1,
		MUXn0 = bit0, // e.g. MUX10

		CHnSET_const = 0x00,

		GAIN_1X = 0x00,
		GAIN_2X = GAINn0,
		GAIN_4X = GAINn1,
		GAIN_6X = (GAINn1 | GAINn0),
		GAIN_8X = GAINn2,
		GAIN_12X = (GAINn2 | GAINn0),
		GAIN_24X = (GAINn2 | GAINn1),

		ELECTRODE_INPUT = 0x00,
		SHORTED = MUXn0,
		BIAS_INPUT = MUXn1,
		MVDD = (MUXn1 | MUXn0),
		TEMPERATURE = MUXn2,
		TEST_SIGNAL = (MUXn2 | MUXn0),
		BIAS_DRP = (MUXn2 | MUXn1),
		BIAS_DRN = (MUXn2 | MUXn1 | MUXn0)
	};

	enum BIAS_SENSP_bits {
		BIASP8 = bit7,
		BIASP7 = bit6,
		BIASP6 = bit5,
		BIASP5 = bit4,
		BIASP4 = bit3,
		BIASP3 = bit2,
		BIASP2 = bit1,
		BIASP1 = bit0,

		BIAS_SENSP_const = 0x00
	};

	enum BIAS_SENSN_bits {
		BIASN8 = bit7,
		BIASN7 = bit6,
		BIASN6 = bit5,
		BIASN5 = bit4,
		BIASN4 = bit3,
		BIASN3 = bit2,
		BIASN2 = bit1,
		BIASN1 = bit0,

		BIAS_SENSN_const = 0x00
	};

	enum LOFF_SENSP_bits {
		LOFFP8 = bit7,
		LOFFP7 = bit6,
		LOFFP6 = bit5,
		LOFFP5 = bit4,
		LOFFP4 = bit3,
		LOFFP3 = bit2,
		LOFFP2 = bit1,
		LOFFP1 = bit0,

		LOFF_SENSP_const = 0x00
	};

	enum LOFF_SENSN_bits {
		LOFFN8 = bit7,
		LOFFN7 = bit6,
		LOFFN6 = bit5,
		LOFFN5 = bit4,
		LOFFN4 = bit3,
		LOFFN3 = bit2,
		LOFFN2 = bit1,
		LOFFN1 = bit0,

		LOFF_SENSN_const = 0x00
	};

	enum LOFF_FLIP_bits {
		LOFF_FLIP8 = bit7,
		LOFF_FLIP7 = bit6,
		LOFF_FLIP6 = bit5,
		LOFF_FLIP5 = bit4,
		LOFF_FLIP4 = bit3,
		LOFF_FLIP3 = bit2,
		LOFF_FLIP2 = bit1,
		LOFF_FLIP1 = bit0,

		LOFF_FLIP_const = 0x00
	};

	enum LOFF_STATP_bits {
		IN8P_OFF = bit7, // LOFFM8 in datasheet SBAS499A Aug 2012
		IN7P_OFF = bit6,
		IN6P_OFF = bit5,
		IN5P_OFF = bit4,
		IN4P_OFF = bit3,
		IN3P_OFF = bit2,
		IN2P_OFF = bit1,
		IN1P_OFF = bit0, // LOFFM1 in datasheet

		LOFF_STATP_const = 0x00
	};

	enum LOFF_STATN_bits {
		IN8N_OFF = bit7, // IN8M_OFF in datasheet
		IN7N_OFF = bit6,
		IN6N_OFF = bit5,
		IN5N_OFF = bit4,
		IN4N_OFF = bit3,
		IN3N_OFF = bit2,
		IN2N_OFF = bit1,
		IN1N_OFF = bit0, // IN1M_OFF in datasheet

		LOFF_STATN_const = 0x00
	};

	enum GPIO_bits {
		GPIOD4 = bit7,
		GPIOD3 = bit6,
		GPIOD2 = bit5,
		GPIOD1 = bit4,
		GPIOC4 = bit3,
		GPIOC3 = bit2,
		GPIOC2 = bit1,
		GPIOC1 = bit0,

		GPIO_const = 0x00
	};

	enum MISC1_bits {
		MISC1_7_off = bit7,
		MISC1_6_off = bit6,
		MISC1_SRB1 = bit5,
		MISC1_4_off = bit4,
		MISC1_3_off = bit3,
		MISC1_2_off = bit2,
		MISC1_1_off = bit1,
		MISC1_0_off = bit0,

		MISC1_const = 0x00,
	};

	enum MISC2_bits {
		MISC2_7_off = bit7,
		MISC2_6_off = bit6,
		MISC2_5_off = bit5,
		MISC2_4_off = bit4,
		MISC2_3_off = bit3,
		MISC2_2_off = bit2,
		MISC2_1_off = bit1,
		MISC2_0_off = bit0,

		MISC2_const = 0x00,
	};

	enum CONFIG4_bits {
		CONFIG4_7_off = bit7,
		CONFIG4_6_off = bit6,
		CONFIG4_5_off = bit5,
		CONFIG4_4_off = bit4,
		SINGLE_SHOT = bit3,
		CONFIG4_2_off = bit2,
		PD_LOFF_COMP = bit1,
		CONFIG4_0_off = bit0,

		CONFIG4_const = 0x00,

		PD_LOFF_COMP_comparators_disabled = 0,
		PD_LOFF_COMP_comparators_enabled = PD_LOFF_COMP,
	};

	struct Data_frame {
		/*
		   // format of the data frame:
		   unsigned magic : 4;
		   unsigned loff_statp : 8;
		   unsigned loff_statn : 8;
		   unsigned gpio : 4;
		   unsigned ch1 : 24;
		   unsigned ch2 : 24;
		   unsigned ch3 : 24;
		   unsigned ch4 : 24;
		   unsigned ch5 : 24;
		   unsigned ch6 : 24;
		   unsigned ch7 : 24;
		   unsigned ch8 : 24;
		 */
		enum { size = 3 + 3 * 8 };
		uint8_t data[size];

#ifdef __cplusplus
		size_t dump(char *buf, size_t buf_len) {
			size_t i;
			if (!buf) {
				return 0;
			}
			for (i=0; i < size && (i*2) < (buf_len-3); ++i) {
				uint8_t v = data[i];
				uint8_t h = (v & 0xF0) >> 4;
				uint8_t l = (v & 0x0F);
				char basec;
				basec = (h < 10) ? '0' : ((char)('A'-10));
				buf[2*i] = basec + h;
				basec = (l < 10) ? '0' : ((char)('A'-10));
				buf[2*i + 1] = basec + ((char)l);
			}
			buf[2*i] = '\0';
			return 2*i;
		}
		uint8_t loff_statp() const {
			return ((data[0] << 4) | (data[1] >> 4));
		}
		uint8_t loff_statn() const {
			return ((data[1] << 4) | (data[2] >> 4));
		}
		bool loff_statp(int i)const {
			return ((loff_statp() >> i) & 1);
		}
		bool loff_statn(int i)const {
			return ((loff_statn() >> i) & 1);
		}
		bool magic_ok() const {
			return 0xC0 == (data[0] & 0xF0);
		}
		long channel_value(unsigned channel) const {
			unsigned num = channel - 1;

			// first byte is signed
			signed long bits_17_24 = (((signed long)((int8_t)
				data[3 + (num * 3)])) << 16);

			unsigned long bits_9_16 = (((unsigned long)
				data[4 + (num * 3)]) << 8);

			unsigned long bits_0_8 = (((unsigned long)
				data[5 + (num * 3)]) << 0);

			signed long val = bits_17_24 | bits_9_16 | bits_0_8;
			return val;
		}
#endif
	};
#ifdef __cplusplus
}
#endif				/* namespace ADS1299 */
#endif				/* ADS1299_H */
