/**
  @brief nRF905 RX Demo, by elechouse.
*/
#include <NRF905.h>
#include <SPI.h>
#define BUF_LEN          32
#define CONF_LEN         10

#define NRF905_CSN       4

unsigned char write_config_buf[CONF_LEN]={
    0x76,                   //CH_NO,868.4MHZ
    0x0E,                   //output power 10db, resend disable, Current Normal operation
    0x44,                   //4-byte address
    0x20,0x20,              //receive or send data length 32 bytes
    0xCC,0xCC,0xCC,0xCC,    //receiving address
    0x58,                   //CRC enable,8bit CRC,external clock disable,16MHZ Oscillator
};

unsigned char rx_buf[BUF_LEN]= {0};
unsigned char read_config_buf[CONF_LEN];
unsigned char rx_address[4]= {0xcc,0xcc,0xcc,0xcc};

void putstring(unsigned char *str)
{
    while(*str){
        Serial.write(*str++);
    }
}

void setup()
{
    char i;
    
    pinMode(10, OUTPUT);        // make sure SPI works
    pinMode(NRF905_CSN,OUTPUT);
    nrf905=NRF905(NRF905_CSN);
    
    /** pin/port configuration */
    nrf905.init();
    
    /** register configuration */
    // user self-defined configuration
//  nrf905.write_config(write_config_buf);      

    /**
        default configuration, need to specify frequency
        choose Z-Wave frequency band, support :
        US		908.42Mhz
        EUROPE		868.42MHz
        AFRICA		868.42MHz
        CHINA		868.42MHz
        HK		919.82MHz
        JAPAN		853.42MHz
        AUSTRALIA	921.42MHz
        NEW_ZEALAND	921.42MHz
        BRASIL		921.42MHz
        Russia		896MHz
    */
    nrf905.write_config(US);                     
    
    /** read register configuration, check register value written */
    nrf905.read_config(read_config_buf);
    
    /** serial communication configurate */
    Serial.begin(9600);
    
    /** test configuration */
    for(i=0; i<CONF_LEN; i++){
        Serial.print(read_config_buf[i],HEX);
        Serial.print(' ');
    }
    
}

void loop()
{
    /** recieve data packet with default RX address */
    nrf905.RX(rx_buf);
    
    /** recieve data packet with specified RX address */
//    nrf905.RX(rx_buf, rx_address );
    
    // NOTE: TX_Address and RX_Address must be the same
    
    /** send recieved data to PC through serial port */
    putstring(rx_buf);
    delay(1);
}

