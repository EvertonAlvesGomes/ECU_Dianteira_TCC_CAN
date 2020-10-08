/* ECU_Diant_PIO.h
 *  Configuração das portas I/O do microcontrolador para funcionamento dos periféricos e sensores.
 *  Autor: Everton A. Gomes
 *  Data: 04/10/2020
*/

#ifndef _ECU_DIANT_PIO_H
#define _ECU_DIANT_PIO_H


//********************************************************************************
//                             #includes
//********************************************************************************

//#include "ECU_Tras_PMC.h"

//********************************************************************************
//                             #defines
//********************************************************************************

//Defines gerais:
#define PIOC_PID                          13                //PIO C peripheral identifier

//Endereços de registradores:
#define PIOB_PER                          0x400E1000        //PIO B enable register
#define PIOB_PDR                          0x400E1004        //PIO B disable register
#define PIOC_PER                          0x400E1200        //PIO C enable register
#define PIOC_ODR                          0x400E1214        //PIO C output disable register
#define PIOC_PUDR                         0x400E1260        //PIO C Pull up disable register
#define PIOC_PUER                         0x400E1264        //PIO C Pull up enable register
#define PIOC_PUSR                         0x400E1268        //PIO C Pull up status register
#define PIOC_PDSR                         0x400E123C        //PIO C data status register


//********************************************************************************
//                             Registradores
//********************************************************************************

uint32_t *pPIOB_PER = (uint32_t*)(PIOB_PER);
uint32_t *pPIOB_PDR = (uint32_t*)(PIOB_PDR);
uint32_t *pPIOC_PER = (uint32_t*)(PIOC_PER);
uint32_t *pPIOC_ODR = (uint32_t*)(PIOC_ODR);
uint32_t *pPIOC_PUDR = (uint32_t*)(PIOC_PUDR);
uint32_t *pPIOC_PUER = (uint32_t*)(PIOC_PUER);
uint32_t *pPIOC_PUSR = (uint32_t*)(PIOC_PUSR);
uint32_t *pPIOC_PDSR = (uint32_t*)(PIOC_PDSR);


//********************************************************************************
//                             Funções
//********************************************************************************

/*
void ecu_diant_pio_init(){
  ecu_tras_pmc_enable_periph_clock(PIOC_PID);
}*/

/* ecu_diant_piob_enable_pin_controlling(uint8_t pin)
 *  Habilita o controlador PIOB controlar o pino "pin", desabilitando o controle de periféricos sobre esse pino.
*/
void ecu_diant_piob_enable_pin_controlling(uint8_t pin){
  *pPIOB_PER |= 1 << pin;
}

/* ecu_diant_pioc_enable_pin_controlling(uint8_t pin)
 *  Habilita o controlador PIOC controlar o pino "pin", desabilitando o controle de periféricos sobre esse pino.
*/
void ecu_diant_pioc_enable_pin_controlling(uint8_t pin){
  *pPIOC_PER |= 1 << pin;
}


/* ecu_diant_piob_disable_pin_controlling(uint8_t pin)
 *  Desabilita o controlador PIOB controlar o pino "pin", habilitando o controle de periféricos sobre esse pino.
*/
void ecu_diant_piob_disable_pin_controlling(uint8_t pin){
  *pPIOB_PDR |= 1 << pin;
}



/* ecu_tras_pio_config_input(uint8_t pin)
 *  Configura o pino <pin> com entrada.
*/
void ecu_tras_pio_config_input(uint8_t pin){
  *pPIOC_ODR |= 1 << pin;
}


/* pio_enable_pin_controlling(uint8_t pin)
 *  Habilita o resistor pull-up interno do pino "pin".
*/
void ecu_tras_pio_enable_pullup_resistor(uint8_t pin){
  *pPIOC_PUER |= 1 << pin;
}

/*
 * ecu_tras_pio_read_all_pins()
 * Reads all pin status (LOW = 0, HIGH = 1).
 */
uint8_t ecu_tras_pio_read_all_pins(){
  uint32_t pin_levels = *pPIOC_PDSR;
  return pin_levels;
}


/*
 * ecu_tras_pio_read_pin(uint8_t pin)
 * Reads pin status (LOW = 0, HIGH = 1).
 */
uint8_t ecu_tras_pio_read_pin(uint8_t pin){
  uint32_t pin_levels = *pPIOC_PDSR;
  return ( ( pin_levels & ( 1 << pin )) >> pin );
}

#endif
