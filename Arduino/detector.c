#define F_CPU 16000000UL
#define BAUD 9600

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

void AD_setup() {
  
  ADCSRA |= (1 << ADEN); // Enable ADC
  ADCSRA |= (1 << ADPS2); // Prescale - Division factor : 16
  ADMUX |= (1 << REFS0); // AV_cc with external capacitor at AREF pin
}

void SPI_init(void) {

  DDRB |= (1 << PB2); // Set SS (Pin PB2) as output
  DDRB |= (1 << PB3); // Set MOSI (Pin PB3) as output
  DDRB |= (0 << PB4); // Set MISO (Pin PB4) as input
  DDRB |= (1 << PB5); // Set SCK (Pin PB5) as output

  PORTB |= (1 << PB2); // Turn on SS

  SPCR |= (1 << SPE); // Enable SPI
  SPCR |= (1 << MSTR); // Master mode
  SPCR |= (1 << SPR0); // Frequency divided by 16
}

void SPI_send(char val) {
  
  PORTB ^= (1 << PB2); // Flip SS to indicate incoming transmission
  SPDR = val; // Put the value in the SPI Data Register
  while (!(SPSR & (1 << SPIF))) {} // Idle until transmission is complete 
  PORTB ^= (1 << PB2); // Flip SS after transmission is over
}

uint16_t Read_ADC(uint8_t adc_input) {

  uint8_t low_bit = 0;
  uint16_t sum = 0;
  ADMUX = (ADMUX & 0xf0) | adc_input; // Prepare for new conversion
  _delay_us(10);

  for (int i = 0; i <= 10; i++) {
    
    ADCSRA |= (1 << ADSC); // Start ADC conversion
    while (!(ADCSRA & (1 << ADIF))); // As long as we don't get the value, stall
    low_bit = ADCL; // Read data register ADCL
    sum += (uint16_t)((ADCH << 8) | low_bit); // Sum the ADC values
    ADCH = 0;
    ADCL = 0;
  }

  return (uint16_t)(sum / 10); // Return the average (for precision)
}

int main(void) {
  
  AD_setup(); // Call the ADC setup function
  SPI_init(); // Call the SPI-Master setup function to communicate with the other Arduino
  Serial_init(); // Call the the Serial setup to communicate with UART functions with the Beaglebone

  uint16_t ADC_result[3];
  int i = 0;

  while (1) {
   
    for(i = 0; i < 3; i++)
      ADC_result[i] = Read_ADC(i);

    char val;

    if (ADC_result[1] > 30)
      val = 'f'; // Forward
    
    else {
    
      if (ADC_result[0] > 35 || ADC_result[2] > 35) {
      
        if (ADC_result[0] >= ADC_result[2])
          val = 'r'; // Right
          
        else
          val = 'l'; // Left
      }
      
      else {
      
        val = 'b'; // Backward
      }
    }

    SPI_send(val); // Send the value to the other Arduino
    Serial_write(val); // Send the value to the Beaglebone
    
    _delay_ms(3000);
  }
}

/*
  Configuring UART Communication in order to send data to the serial monitor
*/
void Serial_init(void){
  
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~(_BV(U2X0));
#endif

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
}

/* Sends 1 byte of data to the serial Monitor
   Waits for USART Data Register to be empty by checking if the UDRE flag is set
*/
void Serial_write(char c){
  loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
  UDR0 = c;
}
