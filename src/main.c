/* Copyright 2017, Esteban Volentini - Facet UNT, Fi UNER
 * Copyright 2014, 2015 Mariano Cerdeiro
 * Copyright 2014, Pablo Ridolfi
 * Copyright 2014, Juan Cecconi
 * Copyright 2014, Gustavo Muro
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file blinking.c
 **
 ** @brief Ejemplo de un led parpadeando
 **
 ** Ejemplo de un led parpadeando utilizando la capa de abstraccion de
 ** hardware y con sistema operativo FreeRTOS.
 **
 ** | RV | YYYY.MM.DD | Autor       | Descripción de los cambios              |
 ** |----|------------|-------------|-----------------------------------------|
 ** |  2 | 2017.10.16 | evolentini  | Correción en el formato del archivo     |
 ** |  1 | 2017.09.21 | evolentini  | Version inicial del archivo             |
 **
 ** @defgroup ejemplos Proyectos de ejemplo
 ** @brief Proyectos de ejemplo de la Especialización en Sistemas Embebidos
 ** @{
 */

/* === Inclusiones de cabeceras ============================================ */
#include "FreeRTOS.h"
#include "bsp.h"
#include "task.h"
#include <stdbool.h>
#include "event_groups.h"

/* === Definicion y Macros ================================================= */

#define EVENTO_TECLA_1_ON 	( 1 << 0 )
#define EVENTO_TECLA_2_ON 	( 1 << 1 )
#define EVENTO_TECLA_3_ON 	( 1 << 2 )
#define EVENTO_TECLA_4_ON 	( 1 << 3 )
#define EVENTO_TECLA_1_OFF ( 1 << 4 )
#define EVENTO_TECLA_2_OFF ( 1 << 5 )
#define EVENTO_TECLA_3_OFF ( 1 << 6 )
#define EVENTO_TECLA_4_OFF ( 1 << 7 )

/* === Declaraciones de tipos de datos internos ============================ */

/* === Declaraciones de funciones internas ================================= */

/* === Definiciones de variables internas ================================== */

static board_t board;

/* === Definiciones de variables externas ================================== */

EventGroupHandle_t eventos_teclas;


/* === Definiciones de funciones internas ================================== */

void Azul(void * parameters) {
	while (true) {
		EventBits_t eventos;
		eventos=xEventGroupWaitBits(eventos_teclas,EVENTO_TECLA_1_ON | EVENTO_TECLA_1_OFF,pdTRUE,pdFALSE,portMAX_DELAY);

		if (eventos & EVENTO_TECLA_1_ON ){
			DigitalOutputActivate(board->led_azul);
		}
		else if (eventos & EVENTO_TECLA_1_OFF ){
			DigitalOutputDeactivate(board->led_azul);

		}

		//vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void Amarillo(void * parameters) {
	while (true) {
		xEventGroupWaitBits(eventos_teclas,EVENTO_TECLA_2_ON,pdTRUE,pdTRUE,portMAX_DELAY);
		DigitalOutputToggle(board->led_amarillo);

	}
}

void Rojo(void * parameters) {
	while (true) {
		xEventGroupWaitBits(eventos_teclas,EVENTO_TECLA_3_ON,pdTRUE,pdTRUE,portMAX_DELAY);
		DigitalOutputActivate(board->led_rojo);
		xEventGroupWaitBits(eventos_teclas,EVENTO_TECLA_4_ON,pdTRUE,pdTRUE,portMAX_DELAY);
		DigitalOutputDeactivate(board->led_rojo);

		//vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void Teclas(void * parameters) {
	while (true) {

		if(DigitalInputHasActivated(board->boton_prueba)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_1_ON);

		}else if (DigitalInputHasDeactivated(board->boton_prueba)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_1_OFF);
		}

		//Al presionar la tecla 2 el led amarillo deberá cambiar de estado
		if(DigitalInputHasActivated(board-> boton_cambiar)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_2_ON);

		}else if (DigitalInputHasDeactivated(board-> boton_cambiar)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_2_OFF);
		}

		if(DigitalInputHasActivated(board->boton_prender)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_3_ON);

		}else if (DigitalInputHasDeactivated(board->boton_prender)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_3_OFF);
		}

		if(DigitalInputHasActivated(board->boton_apagar)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_4_ON);

		}else if (DigitalInputHasDeactivated(board->boton_apagar)){
			xEventGroupSetBits(eventos_teclas,EVENTO_TECLA_4_OFF);
		}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* === Definiciones de funciones externas ================================== */

/** @brief Función principal del programa
 **
 ** @returns 0 La función nunca debería termina
 **
 ** @remarks En un sistema embebido la función main() nunca debe terminar.
 **          El valor de retorno 0 es para evitar un error en el compilador.
 */
int main(void) {
	/* Inicializaciones y configuraciones de dispositivos */
	board = BoardCreate();

	eventos_teclas=xEventGroupCreate();

	/* Creación de las tareas */
	xTaskCreate(Azul, "azul", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Amarillo, "amarillo", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Rojo, "rojo", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Teclas, "Escanteclas", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	/* Arranque del sistema operativo */
	vTaskStartScheduler();

	/* vTaskStartScheduler solo retorna si se detiene el sistema operativo */
	while (1) {
	};

	/* El valor de retorno es solo para evitar errores en el compilador*/
	return 0;
}
/* === Ciere de documentacion ============================================== */
/** @} Final de la definición del modulo para doxygen */
