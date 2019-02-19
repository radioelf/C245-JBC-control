#include <avr/io.h>
#define USE_OPENSMART_SHIELD_PINOUT 1

//#define I2CTFT 1
#define HC595_DAT 4
#define HC595_SHCP 6
#define HC595_STCP 7

#define DELAY7        \
  asm volatile(       \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "rjmp .+0" "\n\t" \
    "nop"      "\n"   \
    ::);

//#define USE_SPECIAL             //check for custom drivers
#if defined(USE_SPECIAL)
#include "mcufriend_special.h"
#if !defined(USE_SPECIAL_FAIL)
#warning WE ARE USING A SPECIAL CUSTOM DRIVER
#endif
#endif
#if !defined(USE_SPECIAL) || defined (USE_SPECIAL_FAIL)



#if 0
#elif defined(__AVR_ATmega328P__)       //regular UNO shield on UNO
#define RD_PORT PORTC
#define RD_PIN  0
#define WR_PORT PORTC
#define WR_PIN  1
#define CD_PORT PORTC
#define CD_PIN  2
#define CS_PORT PORTC
#define CS_PIN  3
#define RESET_PORT PORTC
#define RESET_PIN  4

#define DMASK         0x03
#define NMASK         ~DMASK
#ifdef USE_OPENSMART_SHIELD_PINOUT

// These are macros for I/O operations...
#ifdef I2CTFT
/*inline void waitTransmissionI2C()   
{  
  uint8_t count = 250;  
  while (!(TWCR & (1<<TWINT)))  
  {  
    count--;  
    if (count==0)     
    {                
      TWCR = 0;                 

      break;  
    }  
  }  
}  */
#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#define DAT_PORT PORTD
#define DAT_PIN 4
#define SHCP_PORT PORTD
#define SHCP_PIN 6
#define STCP_PORT PORTD
#define STCP_PIN 7

#define DAT_LOW  PIN_LOW(DAT_PORT, DAT_PIN)
#define DAT_HIGH  PIN_HIGH(DAT_PORT, DAT_PIN)
#define SHCP_LOW  PIN_LOW(SHCP_PORT, SHCP_PIN)
#define SHCP_HIGH PIN_HIGH(SHCP_PORT, SHCP_PIN)
#define STCP_LOW  PIN_LOW(STCP_PORT, STCP_PIN)
#define STCP_HIGH PIN_HIGH(STCP_PORT, STCP_PIN)

inline void HC595Write(uint8_t dat)
{
  uint8_t bitNum;
 // digitalWrite(HC595_STCP,LOW);
 STCP_LOW;
  for(uint8_t i=0;i < 8;i++)
  {
    SHCP_LOW;
    bitNum = dat & 0x80;
    if(bitNum) DAT_HIGH;
	else DAT_LOW;
    dat <<= 1;
	SHCP_HIGH;
  }
  STCP_HIGH;
 }
#define write_8(x) HC595Write(x)
  /*#define write_8(x) { \
    Wire.beginTransmission(0x21);       \
	Wire.write(x);                      \
	Wire.endTransmission();}*/
  /* #define write_8(x) {                          \
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); \ 
  while ((TWCR & (1<<TWINT)) == 0);  \                      
  TWDR = 0x42;            \                
  TWCR = (1<<TWINT) | (1<<TWEN);\                  
	TWDR = x;                \               
    TWCR = (1<<TWINT) | (1<<TWEN);\
    while (!(TWCR & (1<<TWINT))); \
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}*/ // STROBEs are defined later
#else
  // Write 8-bit value to LCD data lines
  #define write_8(x) {                          \
    PORTD = (PORTD & B00101111) | ((x) & B11010000); \
    PORTB = (PORTB & B11010000) | ((x) & B00101111);} // STROBEs are defined later
#endif
  // Read 8-bit value from LCD data lines.  The signle argument
  // is a destination variable; this isn't a function and doesn't
  // return a value in the conventional sense.
  #define read_8() (PIND & B11010000) | (PINB & B00101111)

  // These set the PORT directions as required before the write and read
  // operations.  Because write operations are much more common than reads,
  // the data-reading functions in the library code set the PORT(s) to
  // input before a read, and restore them back to the write state before
  // returning.  This avoids having to set it for output inside every
  // drawing method.  The default state has them initialized for writes.
  #define setWriteDir() { DDRD |=  B11010000; DDRB |=  B00101111; }
  #define setReadDir()  { DDRD &= ~B11010000; DDRB &= ~B00101111; }

#else
#define write_8(x)    { PORTB = (PORTB & NMASK) | ((x) & DMASK); PORTD = (PORTD & DMASK) | ((x) & NMASK); }
#define read_8()      ( (PINB & DMASK) | (PIND & NMASK) )
#define setWriteDir() { DDRB = (DDRB & NMASK) | DMASK; DDRD = (DDRD & DMASK) | NMASK;  }
#define setReadDir()  { DDRB = (DDRB & NMASK) & NMASK; DDRD = (DDRD & DMASK) & DMASK;  }
#endif 

#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)       //regular UNO shield on MEGA2560
#define RD_PORT PORTF
#define RD_PIN  0
#define WR_PORT PORTF
#define WR_PIN  1
#define CD_PORT PORTF
#define CD_PIN  2
#define CS_PORT PORTF
#define CS_PIN  3
#define RESET_PORT PORTF
#define RESET_PIN  4

#ifdef USE_OPENSMART_SHIELD_PINOUT

  #define write_8(x) {                                              \
    PORTH = (PORTH&B10000111)|(((x)&B11000000)>>3)|(((x)&B00000011)<<5); \
    PORTB = (PORTB&B01001111)|(((x)&B00101100)<<2);                      \
    PORTG = (PORTG&B11011111)|(((x)&B00010000)<<1);                      \
    }
  #define read_8()(                                      \ 
  	((PINH & B00011000) << 3) | ((PINB & B10110000) >> 2) | \
  	((PING & B00100000) >> 1) | ((PINH & B01100000) >> 5) )
  #define setWriteDir() {                                   \
    DDRH |=  B01111000; DDRB |=  B10110000; DDRG |=  B00100000; }
  #define setReadDir()  {                                   \
    DDRH &= ~B01111000; DDRB &= ~B10110000; DDRG &= ~B00100000; }

 #else 


#define EMASK         0x38
#define GMASK         0x20
#define HMASK         0x78
#define write_8(x)   {  PORTH &= ~HMASK; PORTG &= ~GMASK; PORTE &= ~EMASK; \
                        PORTH |= (((x) & (3<<0)) << 5); \
                        PORTE |= (((x) & (3<<2)) << 2); \
                        PORTG |= (((x) & (1<<4)) << 1); \
                        PORTE |= (((x) & (1<<5)) >> 2); \
                        PORTH |= (((x) & (3<<6)) >> 3); \
					 }
//
#define read_8()      ( ((PINH & (3<<5)) >> 5)\
                      | ((PINE & (3<<4)) >> 2)\
                      | ((PING & (1<<5)) >> 1)\
                      | ((PINE & (1<<3)) << 2)\
                      | ((PINH & (3<<3)) << 3)\
                      )
#define setWriteDir() { DDRH |=  HMASK; DDRG |=  GMASK; DDRE |=  EMASK;  }
#define setReadDir()  { DDRH &= ~HMASK; DDRG &= ~GMASK; DDRE &= ~EMASK;  }
#endif

#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))


#elif defined(__SAMD21G18A__)   //regular UNO shield on ZERO or M0_PRO
#include "sam.h"
 // configure macros for the control pins
#define RD_PORT PORT->Group[0]
#define RD_PIN  2
#define WR_PORT PORT->Group[1]
#define WR_PIN  8
#define CD_PORT PORT->Group[1]
#define CD_PIN  9
#define CS_PORT PORT->Group[0]
#define CS_PIN  4
#define RESET_PORT PORT->Group[0]
#define RESET_PIN  5
 // configure macros for data bus
#define DMASK 0x0030C3C0
 //  #define write_8(x) PORT->Group[0].OUT.reg = (PORT->Group[0].OUT.reg & ~DMASK)|(((x) & 0x0F) << 6)|(((x) & 0x30) << 10)|(((x) & 0xC0)<<14)
#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_ZERO)   // American ZERO
#define write_8(x) {\
	PORT->Group[0].OUTCLR.reg = DMASK;\
	PORT->Group[0].OUTSET.reg = (((x) & 0x0B) << 6)\
                               |(((x) & (1<<2)) << 12)\
	                           |(((x) & (1<<4)) << 4)\
	                           |(((x) & (1<<5)) << 10)\
	                           |(((x) & 0xC0) << 14);\
                   }
#define read_8()   (((PORT->Group[0].IN.reg >> 6) & 0x0B)\
                   |((PORT->Group[0].IN.reg >> 12) & (1<<2))\
                   |((PORT->Group[0].IN.reg >> 4) &  (1<<4))\
                   |((PORT->Group[0].IN.reg >> 10) & (1<<5))\
                   |((PORT->Group[0].IN.reg >> 14) & 0xC0))
#else   //default to an M0_PRO on v1.6.5 or 1.7.6
#define write_8(x) {\
	PORT->Group[0].OUTCLR.reg = DMASK;\
	PORT->Group[0].OUTSET.reg = (((x) & 0x0F) << 6)\
                               |(((x) & 0x30) << 10)\
                               |(((x) & 0xC0) << 14);\
                   }
#define read_8()   (((PORT->Group[0].IN.reg >> 6) & 0x0F)|((PORT->Group[0].IN.reg >> 10) & 0x30)|((PORT->Group[0].IN.reg >> 14) & 0xC0))
#endif
#define setWriteDir() { PORT->Group[0].DIRSET.reg = DMASK; \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK & 0xFFFF) | (0<<22) | (1<<28) | (1<<30); \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK>>16) | (0<<22) | (1<<28) | (1<<30) | (1<<31); \
                        }
#define setReadDir()  { PORT->Group[0].DIRCLR.reg = DMASK; \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK & 0xFFFF) | (1<<17) | (1<<28) | (1<<30); \
	                  PORT->Group[0].WRCONFIG.reg = (DMASK>>16) | (1<<17) | (1<<28) | (1<<30) | (1<<31); \
                        }
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }
 // Shield Control macros.
#define PIN_LOW(port, pin)    (port).OUTCLR.reg = (1<<(pin))
#define PIN_HIGH(port, pin)   (port).OUTSET.reg = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port).DIR.reg |= (1<<(pin))

#elif defined(__SAM3X8E__)      //regular UNO shield on DUE
 // configure macros for the control pins
#define RD_PORT PIOA
#define RD_PIN  16
#define WR_PORT PIOA
#define WR_PIN  24
#define CD_PORT PIOA
#define CD_PIN  23
#define CS_PORT PIOA
#define CS_PIN  22
#define RESET_PORT PIOA
#define RESET_PIN  6
 // configure macros for data bus
#define BMASK         (1<<25)
#define CMASK         (0xBF << 21)
#define write_8(x)   {  PIOB->PIO_CODR = BMASK; PIOC->PIO_CODR = CMASK; \
                        PIOC->PIO_SODR = (((x) & (1<<0)) << 22); \
                        PIOC->PIO_SODR = (((x) & (1<<1)) << 20); \
                        PIOB->PIO_SODR = (((x) & (1<<2)) << 23); \
                        PIOC->PIO_SODR = (((x) & (1<<3)) << 25); \
                        PIOC->PIO_SODR = (((x) & (1<<4)) << 22); \
                        PIOC->PIO_SODR = (((x) & (1<<5)) << 20); \
                        PIOC->PIO_SODR = (((x) & (1<<6)) << 18); \
                        PIOC->PIO_SODR = (((x) & (1<<7)) << 16); \
					 }

#define read_8()      ( ((PIOC->PIO_PDSR & (1<<22)) >> 22)\
                      | ((PIOC->PIO_PDSR & (1<<21)) >> 20)\
                      | ((PIOB->PIO_PDSR & (1<<25)) >> 23)\
                      | ((PIOC->PIO_PDSR & (1<<28)) >> 25)\
                      | ((PIOC->PIO_PDSR & (1<<26)) >> 22)\
                      | ((PIOC->PIO_PDSR & (1<<25)) >> 20)\
                      | ((PIOC->PIO_PDSR & (1<<24)) >> 18)\
                      | ((PIOC->PIO_PDSR & (1<<23)) >> 16)\
                      )
#define setWriteDir() { PIOB->PIO_OER = BMASK; PIOC->PIO_OER = CMASK; }
#define setReadDir()  { \
                          PMC->PMC_PCER0 = (1 << ID_PIOB)|(1 << ID_PIOC);\
						  PIOB->PIO_ODR = BMASK; PIOC->PIO_ODR = CMASK;\
						}
#define write8(x)     { write_8(x); WR_ACTIVE; WR_STROBE; }
//#define write8(x)     { write_8(x); WR_ACTIVE; WR_STROBE; WR_IDLE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; RD_ACTIVE; dst = read_8(); RD_IDLE; RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }
 // Shield Control macros.
#define PIN_LOW(port, pin)    (port)->PIO_CODR = (1<<(pin))
#define PIN_HIGH(port, pin)   (port)->PIO_SODR = (1<<(pin))
#define PIN_OUTPUT(port, pin) (port)->PIO_OER = (1<<(pin))

#elif defined(__AVR_ATmega32U4__)       //regular UNO shield on Leonardo
#define RD_PORT PORTF
#define RD_PIN  7
#define WR_PORT PORTF
#define WR_PIN  6
#define CD_PORT PORTF
#define CD_PIN  5
#define CS_PORT PORTF
#define CS_PIN  4
#define RESET_PORT PORTF
#define RESET_PIN  1

#ifdef USE_OPENSMART_SHIELD_PINOUT
#define write_8(d) {                                                   \
    PORTE = (PORTE & B10111111) | (((d) & B10000000)>>1);                     \
    PORTD = (PORTD & B01101111) | (((d) & B01000000)<<1) | ((d) & B00010000); \
    PORTC = (PORTC & B01111111) | (((d) & B00100000)<<2);                     \
    PORTB = (PORTB & B00001111) | (((d) & B00001111)<<4);                     \
    }
#define read_8()      ( ((PINE & B01000000) << 1) | ((PIND & B10000000) >> 1) | \
             ((PINC & B10000000) >> 2) | ((PINB & B11110000) >> 4) | \
              (PIND & B00010000) \
)
  #define setWriteDir() {               \
    DDRE |=  B01000000; DDRD |=  B10010000;   \
    DDRC |=  B10000000; DDRB |=  B11110000; }
  #define setReadDir() {                \
    DDRE &= ~B01000000; DDRD &= ~B10010000;   \
    DDRC &= ~B10000000; DDRB &= ~B11110000; }

#else
#define BMASK         (3<<4)
#define CMASK         (1<<6)
#define DMASK         ((1<<7)|(1<<4)|(3<<0))
#define EMASK         (1<<6)
static inline                   //hope we use r24
void write_8(uint8_t x)
{
    PORTB &= ~BMASK;
    PORTC &= ~CMASK;
    PORTD &= ~DMASK;
    PORTE &= ~EMASK;
    PORTB |= (((x) & (3 << 0)) << 4);
    PORTD |= (((x) & (1 << 2)) >> 1);
    PORTD |= (((x) & (1 << 3)) >> 3);
    PORTD |= (((x) & (1 << 4)) << 0);
    PORTC |= (((x) & (1 << 5)) << 1);
    PORTD |= (((x) & (1 << 6)) << 1);
    PORTE |= (((x) & (1 << 7)) >> 1);
}

#define read_8()      ( ((PINB & (3<<4)) >> 4)\
| ((PIND & (1<<1)) << 1)\
| ((PIND & (1<<0)) << 3)\
| ((PIND & (1<<4)) >> 0)\
| ((PINC & (1<<6)) >> 1)\
| ((PIND & (1<<7)) >> 1)\
| ((PINE & (1<<6)) << 1)\
)
#define setWriteDir() { DDRB |=  BMASK; DDRC |=  CMASK; DDRD |=  DMASK; DDRE |=  EMASK;  }
#define setReadDir()  { DDRB &= ~BMASK; DDRC &= ~CMASK; DDRD &= ~DMASK; DDRE &= ~EMASK;  }
#endif

#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        (p) &= ~(1<<(b))
#define PIN_HIGH(p, b)       (p) |= (1<<(b))
#define PIN_OUTPUT(p, b)     *(&p-1) |= (1<<(b))

#else
#error MCU unsupported
#endif                          // regular UNO shields on Arduino boards

#endif                          //!defined(USE_SPECIAL) || defined (USE_SPECIAL_FAIL)

#define RD_ACTIVE  PIN_LOW(RD_PORT, RD_PIN)
#define RD_IDLE    PIN_HIGH(RD_PORT, RD_PIN)
#define RD_OUTPUT  PIN_OUTPUT(RD_PORT, RD_PIN)
#define WR_ACTIVE  PIN_LOW(WR_PORT, WR_PIN)
#define WR_IDLE    PIN_HIGH(WR_PORT, WR_PIN)
#define WR_OUTPUT  PIN_OUTPUT(WR_PORT, WR_PIN)
#define CD_COMMAND PIN_LOW(CD_PORT, CD_PIN)
#define CD_DATA    PIN_HIGH(CD_PORT, CD_PIN)
#define CD_OUTPUT  PIN_OUTPUT(CD_PORT, CD_PIN)
#define CS_ACTIVE  PIN_LOW(CS_PORT, CS_PIN)
#define CS_IDLE    PIN_HIGH(CS_PORT, CS_PIN)
#define CS_OUTPUT  PIN_OUTPUT(CS_PORT, CS_PIN)
#define RESET_ACTIVE  PIN_LOW(RESET_PORT, RESET_PIN)
#define RESET_IDLE    PIN_HIGH(RESET_PORT, RESET_PIN)
#define RESET_OUTPUT  PIN_OUTPUT(RESET_PORT, RESET_PIN)

 // General macros.   IOCLR registers are 1 cycle when optimised.
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }       //PWLW=TWRL=50ns
#define RD_STROBE RD_IDLE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE      //PWLR=TRDL=150ns, tDDR=100ns

#ifdef USE_OPENSMART_SHIELD_PINOUT
  #define CTL_INIT()   { RD_OUTPUT; WR_OUTPUT; CD_OUTPUT; CS_OUTPUT; }
#else  
  #define CTL_INIT()   { RD_OUTPUT; WR_OUTPUT; CD_OUTPUT; CS_OUTPUT; RESET_OUTPUT;  }
#endif

#define WriteCmd(x)  { CD_COMMAND; write16(x); }
#define WriteData(x) { CD_DATA; write16(x); }
