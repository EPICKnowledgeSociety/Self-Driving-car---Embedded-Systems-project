#define F_CPU 16000000UL
#define BAUD 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <util/delay.h>

#define forward_time 500
#define turning_90_time 800
#define turning_180_time 1500

void SPI_init(void) {
  
  DDRB |= (0 << PB2); // Set SS (Pin PB2) as input
  DDRB |= (0 << PB3); // Set MOSI (Pin PB3) as input
  DDRB |= (1 << PB4); // Set MISO (Pin PB4) as output
  DDRB |= (0 << PB5); // Set SCK (Pin PB5) as input
  
  SPCR |= (1 << SPE); // Enable SPI
  SPCR |= (1 << SPR0); // Enable SPI
  SPCR |= (0 << MSTR); // Slave mode
  SPCR |= (1 << SPIE); // Enable SPI interrupts
}

void HBridges_init(void) {
  
  // Enable H-Bridges
  DDRC |= (1 << PC0);
  DDRC |= (1 << PC1);
  DDRC |= (1 << PC2);
  DDRC |= (1 << PC3);

  PORTC |= (1 << PC0);
  PORTC |= (1 << PC1);
  PORTC |= (1 << PC2);
  PORTC |= (1 << PC3);

  // Back left wheel
  DDRD |= (1 << PD2); // +
  DDRD |= (1 << PD3); // -

  // Front left wheel
  DDRD |= (1 << PD4); // +
  DDRD |= (1 << PD5); // -

  // Back right wheel
  DDRD |= (1 << PD7); // +
  DDRD |= (1 << PD6); // -

  // Front right wheel
  DDRB |= (1 << PB1); // +
  DDRB |= (1 << PB0); // -

}

void Stop(){
  
  // Back left wheel
  PORTD &= (0 << PD2); // +
  PORTD &= (0 << PD3); // -

  // Front left wheel
  PORTD &= (0 << PD4); // +
  PORTD &= (0 << PD5); // -

  // Back right wheel
  PORTD &= (0 << PD7); // +
  PORTD &= (0 << PD6); // -


  // Front right wheel
  PORTB &= (0 << PB1); // +
  PORTB &= (0 << PB0); // -

}

void Forward(void){

  // Back left wheel
  PORTD |= (1 << PD2); // +
  PORTD &= (0 << PD3); // -

  // Front left wheel
  PORTD |= (1 << PD4); // +
  PORTD &= (0 << PD5); // -

  // Back right wheel
  PORTD |= (1 << PD7); // +
  PORTD &= (0 << PD6); // -

  // Front right wheel
  PORTB |= (1 << PB1); // +
  PORTB &= (0 << PB0); // -
}
 
void turnLeft(void){

  // Back left wheel
  PORTD &= (0 << PD2); // +
  PORTD |= (1 << PD3); // -

  // Front left wheel
  PORTD &= (0 << PD4); // +
  PORTD |= (1 << PD5); // -

  // Back right wheel
  PORTD |= (1 << PD7); // +
  PORTD &= (0 << PD6); // -

  // Front right wheel
  PORTB |= (1 << PB1); // +
  PORTB &= (0 << PB0); // -
}

void turnRight(void){
  
  // Back left wheel
  PORTD |= (1 << PD2); // +
  PORTD &= (0 << PD3); // -

  // Front left wheel
  PORTD |= (1 << PD4); // +
  PORTD &= (0 << PD5); // -

  // Back right wheel
  PORTD &= (0 << PD7); // +
  PORTD |= (1 << PD6); // -

  // Front right wheel
  PORTB &= (0 << PB1); // +
  PORTB |= (1 << PB0); // -
}

int main() {
  
  HBridges_init();
  SPI_init();
  
  sei();
 
  while(1){}
  
}

ISR(SPI_STC_vect){
  
  cli(); // Disable interrupts

  char val = SPDR; // Retrieve value
  
  if (val =='f'){

    // Go forward ...
    Forward();
    _delay_ms(forward_time);

    // ... and stop until next instruction
    Stop();

  } else if (val == 'r'){

    // Turn car 90 degrees to the right ...
    turnRight();
    _delay_ms(turning_90_time);

    // ... go forward ...
    Forward();
    _delay_ms(forward_time);

    // ... and stop until next instruction
    Stop();
  
  } else if (val == 'l'){

    // Turn car 90 degrees to the left ...
    turnLeft();
    _delay_ms(turning_90_time);

    // ... go forward ...
    Forward();
    _delay_ms(forward_time);

    // ... and stop until next instruction    
    Stop();
    
  } else if (val == 'b'){

    // Turn car 180 degrees to the left ...
    turnLeft();
    _delay_ms(turning_180_time);

    // ... go forward ...
    Forward();
    _delay_ms(forward_time);

    // ... and stop until next instruction    
    Stop();
  }

  sei();
}
