/**
  @brief nRF905 TX Demo, by elechouse.
*/

#include <NRF905.h>
#include <SPI.h>

#define BUF_LEN          32
#define CONF_LEN         10

#define NRF905_CSN       4

unsigned char tx_buf[BUF_LEN]= "Hi, Arduino   \r\n";
unsigned char read_config_buf[CONF_LEN];

byte tx_address[4]= {0xcc,0xcc,0xcc,0xcc};

void setup()
{
    unsigned char i;
    
    pinMode(10, OUTPUT);        // make sure SPI works
    pinMode(NRF905_CSN,OUTPUT); 
    nrf905=NRF905(NRF905_CSN);
    
    nrf905.init();
    /** register configuration */
    // user self-defined configuration
//    nrf905.write_config(write_config_buf);

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
    nrf905.read_config(read_config_buf);

    Serial.begin(9600);

    for(i=0; i<10; i++)
    {
        Serial.print(read_config_buf[i],HEX);
        Serial.print(' ');
    }
    
    tx_buf[12] = '0';
}

void loop()
{
    /** transmit data packet with default TX Address */
    nrf905.TX(tx_buf);
    
    /** transmit data packet with specified TX Address */
//    nrf905.TX(tx_buf, tx_address);
    
    // NOTE: TX_Address and RX_Address must be the same
    
    /** Count Sending times */
    tx_buf[12]++;
    if(tx_buf[12] > '9'){
      tx_buf[12] = '0';
    }
    delay(500);
}

