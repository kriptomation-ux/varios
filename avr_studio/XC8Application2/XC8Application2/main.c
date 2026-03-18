/*
 * main.c
 *
 * Created: 3/18/2026 5:33:00 PM
 *  Author: dario1958
 */ 

#include <xc.h>
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// --- Configuración básica de LCD (Modo 4 bits) ---
#define LCD_PORT PORTA
#define LCD_DDR  DDRA
#define RS PA0
#define EN PA1

void lcd_comando(unsigned char cmd) {
	LCD_PORT = (LCD_PORT & 0x0F) | (cmd & 0xF0); // Enviar nible alto
	LCD_PORT &= ~(1 << RS);
	LCD_PORT |= (1 << EN); _delay_us(1); LCD_PORT &= ~(1 << EN);
	_delay_us(200);
	LCD_PORT = (LCD_PORT & 0x0F) | (cmd << 4);   // Enviar nible bajo
	LCD_PORT |= (1 << EN); _delay_us(1); LCD_PORT &= ~(1 << EN);
	_delay_ms(2);
}

void lcd_char(unsigned char data) {
	LCD_PORT = (LCD_PORT & 0x0F) | (data & 0xF0);
	LCD_PORT |= (1 << RS);
	LCD_PORT |= (1 << EN); _delay_us(1); LCD_PORT &= ~(1 << EN);
	_delay_us(200);
	LCD_PORT = (LCD_PORT & 0x0F) | (data << 4);
	LCD_PORT |= (1 << EN); _delay_us(1); LCD_PORT &= ~(1 << EN);
	_delay_ms(2);
}

void lcd_init() {
	LCD_DDR = 0xFF; // Todo el Puerto A como salida
	_delay_ms(20);
	lcd_comando(0x02); // Modo 4 bits
	lcd_comando(0x28); // 2 líneas, matriz 5x8
	lcd_comando(0x0C); // Display ON, Cursor OFF
	lcd_comando(0x06); // Incremento automático
	lcd_comando(0x01); // Limpiar pantalla
}

void lcd_print(char *str) {
	while(*str) lcd_char(*str++);
}

// --- Lógica del Contador (Timer 5) ---
volatile uint32_t desbordes = 0;

ISR(TIMER5_OVF_vect) {
	desbordes++;
}

void contador_init() {
	DDRL &= ~(1 << PL2); // Pin 47 entrada
	TCCR5B |= (1 << CS52) | (1 << CS51) | (1 << CS50); // Reloj ext flanco subida
	TIMSK5 |= (1 << TOIE5);
	sei();
}

int main(void) {
	char buffer[21];
	lcd_init();
	contador_init();
	
	//lcd_print("Contador Pulsos:");

	while (1) {
		uint32_t total = ((uint32_t)desbordes << 16) | TCNT5;
		
		// Posicionar cursor en la segunda línea (0x40)
		lcd_comando(0xC0);
		lcd_print("Contador Pulsos:");
		sprintf(buffer, "Total: %lu       ", total); // Espacios para limpiar restos
		lcd_print(buffer);
		
		_delay_ms(100);
	}
}
