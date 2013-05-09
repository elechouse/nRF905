#ifndef NRF905_h
#define NRF905_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


/** nrf905 instruction set */
#define WC		0x00
#define RC		0x10
#define WTP		0x20
#define RTP		0x21
#define WTA		0x22
#define RTA		0x23
#define RRP		0x24

typedef enum
{
    US = 0,             /** 908.42Mhz */
    EUROPE = 1,         /** 868.42MHz */
    AFRICA  = 2,         /** 868.42MHz */
    CHINA = 3,          /** 868.42MHz */
    HK = 4,             /** 919.82MHz */
    JAPAN = 5,          /** 853.42MHz */
    AUSTRALIA = 6,      /** 921.42MHz */
    NEW_ZEALAND = 7,    /** 921.42MHz */
    BRASIL = 8,         /** 921.42MHz */
    Russia = 9,         /** 896.00MHz */
} nrf905_freq_type;


class NRF905
{
public:
    NRF905(void);
    NRF905(int css);

    void init();
    void write_config(unsigned char *conf_buf);
    void write_config(nrf905_freq_type freq_band);
    void read_config(unsigned char *conf_buf);
    void RX(unsigned char *TxRxBuffer);
    void RX(unsigned char *TxRxBuf, unsigned char *RxAddress);
    void TX(unsigned char *TxRxBuf, unsigned char *TxAddress);
    void TX(unsigned char *TxRxBuf);

private:
    void set_rx(void);
    void set_tx(void);
    unsigned char check_ready(void);

    void RxPacket(unsigned char *TxRxBuffer);
    void TxPacket(unsigned char *TxAddress, unsigned char *TxRxBuf);

    //----------------------------------------------NRF905 IO------------------------------------------------------
    int TXEN;
    int TRX_CE;
    int PWR;
    //----------------------------------------------NRF905 SPI---------------------------------------------------
    int	CSN;
    //----------------------------------------nrf905 status IO---------------------------------------------------------
    int	AM;
    int	DR;
    int	CD;
};

extern NRF905 nrf905;

#endif


