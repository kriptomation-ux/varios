#include <avr/io.h>

int main(void) {
	// 1. Configurar el Pin PB7 (LED_BUILTIN en Mega) como salida
	DDRB |= (1 << PB7);

	// 2. Configurar el Comparador Analógico (Registro ACSR)
	// ACBG: Conecta la referencia interna de 1.1V a la entrada positiva (AIN0)
	// ACD:  Se mantiene en 0 para que el comparador esté encendido
	ACSR = (1 << ACBG);

	// 3. Desactivar el buffer digital en AIN1 (PE3 / Digital 5) para ahorrar energía
	DIDR1 = (1 << AIN1D);

	while (1) {
		// El bit ACO (Analog Comparator Output) cambia según la comparación:
		// Si 1.1V > AIN1 (Pin 5) -> ACO = 1
		// Si 1.1V < AIN1 (Pin 5) -> ACO = 0
		
		if (ACSR & (1 << ACO)) {
			PORTB |= (1 << PB7);  // Encender LED
			} else {
			PORTB &= ~(1 << PB7); // Apagar LED
		}
	}
}
