#include <NRF905.h>
#include <SPI.h>

/** define a ultity */
NRF905 nrf905;

unsigned char config_info_buf[10]={
        0x76,                   //CH_NO,868.4MHZ
        0x0E,                   //output power 10db, resend disable, Current Normal operation
        0x44,                   //4-byte address
        0x20,0x20,              //receive or send data length 32 bytes
        0xCC,0xCC,0xCC,0xCC,    //receiving address
        0x58,                   //CRC enable,8bit CRC,external clock disable,16MHZ Oscillator
};

PROGMEM unsigned int freq_tab[10] = {
    0x13e,
    0x076,
    0x076,
    0x076,
    0x177,
    0x02b,
    0x17f,
    0x17f,
    0x17f,
    0x100,
};

//-------------------initial nRF905---------------------------------------------
NRF905::NRF905(void)
{
#if 1
    /** Adjust pin definition */
    TXEN=5;
    TRX_CE=6;
    PWR=A0;
    CSN=4;
    AM=7;
    DR=3;
    CD=2;
#else
    TXEN=5;
    TRX_CE=4;
    PWR=3;
    CSN=10;
    AM=9;
    DR=8;
    CD=7;
#endif
}
NRF905::NRF905(int css)
{
#if 1
    /** Adjust pin definition */
    TXEN=5;
    TRX_CE=6;
    PWR=A0;
    CSN=css;
    AM=7;
    DR=3;
    CD=2;
#else
    TXEN=5;
    TRX_CE=4;
    PWR=3;
    CSN=css;
    AM=9;
    DR=8;
    CD=7;
#endif
}

void NRF905::init(void)
{
    pinMode(CSN, OUTPUT);
	digitalWrite(CSN, HIGH); // Spi disable

	pinMode(DR, INPUT);	// Init DR for input

	pinMode(AM, INPUT);// Init AM for input

	pinMode(CD, INPUT);// Init CD for input

	pinMode(PWR, OUTPUT);
	digitalWrite(PWR, HIGH);// nRF905 power on

	pinMode(TRX_CE, OUTPUT);
	digitalWrite(TRX_CE, LOW);// Set nRF905 in standby mode

	pinMode(TXEN, OUTPUT);
	digitalWrite(TXEN, LOW);// set radio in Rx mode

	SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.begin();

    /** wait for nrf905 STANDBY */
    delay(3);
}

//---------------wite to configuration register-----------------
void NRF905::write_config(unsigned char *conf_buf)
{
	digitalWrite(CSN,LOW);						// Spi enable for write a spi command
	/** send write configuration command */
	SPI.transfer(WC);
	for (int i=0;i<10;i++)	// Write configration words
	{
	   SPI.transfer(conf_buf[i]);
	}
	digitalWrite(CSN,HIGH);					// Disable Spi
}

void NRF905::write_config(nrf905_freq_type freq_band)
{
    if(freq_band < 10){
        config_info_buf[0] = (unsigned char)freq_tab[freq_band];
        if(freq_tab[freq_band]&0x100){
            config_info_buf[1] |= 0x01;
        }else{
            config_info_buf[1] &= ~0x01;
        }
    }
    // Spi enable for write a spi command
	digitalWrite(CSN,LOW);
	/** send write configuration command */
	SPI.transfer(WC);
	for (int i=0;i<10;i++)	// Write configration words
	{
	   SPI.transfer(config_info_buf[i]);
	}
	digitalWrite(CSN,HIGH);					// Disable Spi
}

void NRF905::read_config(unsigned char *conf_buf)
{
    digitalWrite(CSN,LOW);						// Spi enable for write a spi command

    /** send read configuration command */
    SPI.transfer(RC);
	for (int i=0;i<10;i++)
	{
	   conf_buf[i] = SPI.transfer(0x00);
	}
	digitalWrite(CSN,HIGH);					// Disable Spi
}

void  NRF905::RX(unsigned char *TxRxBuffer)
{
    set_rx();			// Set nRF905 in Rx mode
    while (check_ready()==0);
    delay(1);
    RxPacket(TxRxBuffer);
    delay(1);
}

void NRF905::RX(unsigned char *TxRxBuf, unsigned char *RxAddress)
{
    if(config_info_buf[5] != RxAddress[0] ||\
       config_info_buf[6] != RxAddress[1] ||\
       config_info_buf[7] != RxAddress[2] ||\
       config_info_buf[8] != RxAddress[3]){

        config_info_buf[5] = RxAddress[0];
        config_info_buf[6] = RxAddress[1];
        config_info_buf[7] = RxAddress[2];
        config_info_buf[8] = RxAddress[3];

        write_config(config_info_buf);
    }

    set_rx();			// Set nRF905 in Rx mode
    while (check_ready()==0);
    delay(1);
    RxPacket(TxRxBuf);
    delay(1);
}

void NRF905::TX(unsigned char *TxRxBuf, unsigned char *TxAddress)
{
    set_tx();
    delay(1);
    // Send data by nRF905
    TxPacket(TxAddress, TxRxBuf);
}

void NRF905::TX(unsigned char *TxRxBuf)
{
    set_tx();
    delay(1);
    // Send data by nRF905
    TxPacket(config_info_buf+5, TxRxBuf);

}

void NRF905::TxPacket(unsigned char *TxAddress, unsigned char *TxRxBuf)
{
	int i;
	digitalWrite(CSN,LOW);
	// Write payload command
	SPI.transfer(WTP);
	for (i=0;i<32;i++){
	    // Write 32 bytes Tx data
		SPI.transfer(TxRxBuf[i]);
	}
	digitalWrite(CSN,HIGH);
	delay(1);

    // Spi enable for write a spi command
	digitalWrite(CSN,LOW);
	// Write address command
	SPI.transfer(WTA);
	// Write 4 bytes address
	for (i=0;i<4;i++){
		SPI.transfer(TxAddress[i]);
	}
	// Spi disable
	digitalWrite(CSN,HIGH);

	// Set TRX_CE high,start Tx data transmission, CE pulse
	digitalWrite(TRX_CE,HIGH);
	delay(1);
	digitalWrite(TRX_CE,LOW);
}

void NRF905::set_tx(void)
{
	digitalWrite(TRX_CE,LOW);
	digitalWrite(TXEN,HIGH);

	// delay for mode change(>=650us)
	delay(1);
}


void NRF905::set_rx(void)
{
	digitalWrite(TXEN, LOW);
	digitalWrite(TRX_CE, HIGH);

	// delay for mode change(>=650us)
	delayMicroseconds(800);
};

unsigned char NRF905::check_ready(void)
{
    if(digitalRead(DR) == HIGH)	{
		return 1;
	}
	else{
		return 0;
	}
}

void NRF905::RxPacket(unsigned char *TxRxBuffer)
{
	int i;
    digitalWrite(TRX_CE,LOW);
	digitalWrite(CSN,LOW);
    delay(1);
	SPI.transfer(RRP);
    delay(1);
	for (i = 0 ;i < 32 ;i++){
		TxRxBuffer[i]=SPI.transfer(NULL);;
        delay(1);
	}
	digitalWrite(CSN,HIGH);
    delay(1);
	digitalWrite(TRX_CE,HIGH);
	delay(1);
}





