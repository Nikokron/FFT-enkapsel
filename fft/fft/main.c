/*
ATmega8, 48, 88, 168, 328

    /Reset PC6|1   28|PC5		SCL
Nokia.SCL  PD0|2   27|PC4		SDA
Nokia.SDIN PD1|3   26|PC3     
Nokia.DC   PD2|4   25|PC2     
Nokia.SCE  PD3|5   24|PC1
Nokia.RST  PD4|6   23|PC0		Signal input
           Vcc|7   22|Gnd
           Gnd|8   21|Aref
Xtal       PB6|9   20|AVcc
Xtal       PB7|10  19|PB5 SCK  
           PD5|11  18|PB4 MISO 
           PD6|12  17|PB3 MOSI 
           PD7|13  16|PB2      
           PB0|14  15|PB1      
*/

#define F_CPU 20000000UL
#define FFT_SIZE	128
#define M			7

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


// display routines for the graphics LCD
#include "nokia5110.h"    
#include "fix_fft.h"


volatile uint8_t signal;
volatile int8_t* buff;
volatile int8_t adc_buff1[FFT_SIZE];
volatile int8_t adc_buff2[FFT_SIZE];
uint8_t buff_flag = 0;
uint8_t count = 0;
volatile uint8_t buff_ready = 0;
uint8_t prev_ADC = 0;
volatile uint8_t trigval=0;

typedef struct {
	uint8_t bar;
	uint8_t max;
}Bars;

ISR(ADC_vect) {
	//PORTB |= (1 << PB1);
	//ADCSRA |= (1 << ADSC);
	//PORTB ^= (1 << PB1);
	signal = ADCH;
	
	*(buff+count) = signal-128;
	count++;
	if (count >= FFT_SIZE) {
		PORTB ^= (1 << PB1);
		if(buff_flag) { //adc_buff2 ready
			buff = adc_buff1;
			buff_flag = 0;
		}
		else {			//adc_buff1 ready
			buff = adc_buff2;
			buff_flag = 1;
		}
		if(buff_ready != 0) {
			//PORTB |= (1 << PB1);
		}	
		buff_ready = 1;
		count = 0; 
	}
	//PORTB &= ~(1 << PB1);
	
}

void adc_init(void) {
	
	
	// ADC init
	//  reference voltage: supply AVCC
	//  channel ADC0
	//  clock: f_cpu/d 
	//  Left-aligned result
	ADMUX  = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR)
		   | (0 << MUX3)  | (0 << MUX2)  | (0 << MUX1) | (0 << MUX0);
	ADCSRA = (1 << ADEN)  | (1 << ADSC)  | (1 << ADATE) | (1 << ADIE)
		   | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRB = (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0); // Free running mode
}
  
int main(void) {
	buff = adc_buff1;
	//uint8_t fft_acc[FFT_SIZE/2];
	//uint8_t abs_fft[FFT_SIZE/2];
	Bars bars[FFT_SIZE/2];
	int8_t img_fft[FFT_SIZE];
	//DDRB |= (1 << PB1);
	NOKIA_init(0);
	NOKIA_LED_ENABLE();
	NOKIA_setVop(0x45);		// 0x40, (0x35)
	//NOKIA_LED_PORT ^= (1 << NOKIA_LED);
	adc_init();	
	uint8_t clear = 0;
	DDRB |= (1 << PB0) | (1 << PB1);
	sei();				// Global interrupt flag
	while (1) {
		
		while (!buff_ready); 
		//cli();
		
		PORTB |= (1 << PB0);
		if (buff_flag) {
			/*
			fix_fftr(adc_buff1,M,0);
			split(adc_buff1, img_fft, FFT_SIZE);
			for(uint8_t i=0; i<FFT_SIZE/2; i++) {
				bars[i].bar = ((img_fft[i]*img_fft[i])>>7) + ((img_fft[i+FFT_SIZE/2]*img_fft[i+FFT_SIZE/2])>>7);
				if(bars[i].bar > bars[i].max) { 
					bars[i].max = bars[i].bar;
				}
				if(bars[i].max > 0) {
					bars[i].max--;
				}
			}
			*/
			fix_fft(adc_buff1, img_fft, M, 0);
			for(uint8_t i=0; i<(FFT_SIZE/2); i++){
				bars[i].bar = (((adc_buff1[i]*adc_buff1[i])>>3) + ((img_fft[i]*img_fft[i])>>3));
				/*
				if(bars[i].bar > bars[i].max) {
					bars[i].max = bars[i].bar;
				}
				if(bars[i].max >= 3) {
					bars[i].max -= 3;
				}
				else bars[i].max = 0;
				*/
			}
		} else if (!buff_flag) {
			/*
			fix_fftr(adc_buff2,M,0);
			split(adc_buff2, img_fft, FFT_SIZE);
			for(uint8_t i=0; i<FFT_SIZE/2; i++) {
				bars[i].bar = ((img_fft[i]*img_fft[i])>>7) + ((img_fft[i+FFT_SIZE/2]*img_fft[i+FFT_SIZE/2])>>7);
				if(bars[i].bar > bars[i].max) {
					bars[i].max = bars[i].bar;
				}
				if(bars[i].max > 0) {
					bars[i].max--;
				}
			}
			*/
			fix_fft(adc_buff2, img_fft, M, 0);
			for(uint8_t i=0; i<(FFT_SIZE/2); i++){
				bars[i].bar = (((adc_buff2[i]*adc_buff2[i])>>3) + ((img_fft[i]*img_fft[i])>>3));
				/*
				if(bars[i].bar > bars[i].max) {
					bars[i].max = bars[i].bar;
				}
				if(bars[i].max >= 3) {
					bars[i].max -= 3;
				}
				else bars[i].max = 0;
				*/
			}
			
			
			
		}
		
		NOKIA_clear();
		for(uint8_t i=0; i<(FFT_SIZE/2); i++){
			//NOKIA_setpixel(i, abs_fft[i]);
			
			for(uint8_t j=0; j<=(bars[i].bar); j++){
				NOKIA_setpixel(i, 47-j);
			}
			
			//fft_acc[i] = abs_fft[i];
				
		}
		NOKIA_update();
		
		/*
		if(clear >= 5) {
			NOKIA_clear();
			for(uint8_t i=0; i<FFT_SIZE/2; i++) {
				for(uint8_t j=0; j<=(fft_acc[i]); j++){
					NOKIA_setpixel(i, 47-j);
				}
				fft_acc[i] = 0;
			}
			NOKIA_update();
			clear = 0;
		}
		clear++;
		*/
		
		PORTB &= ~(1 << PB0);
		buff_ready = 0;
		
		//sei();
	}
}