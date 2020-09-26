/* ECU_Diant_Timer.h
 *  Implementação dos timers em modo de captura, destinados à medição de velocidade.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/

#ifndef _ECU_DIANT_TIMER_H
#define _ECU_DIANT_TIMER_H

//********************************************************************************
//                             #defines
//********************************************************************************

#define TC0_ID        27  //ID do periférico
#define TC1_ID        28  //ID do periférico
#define TC2_ID        29  //ID do periférico
#define CLKEN         0x01 << 0   //bit CLEN no registrador TC_CCRx
#define CAPT          0x00 << 15  //bit ~WAVE (Capture Mode) no TC_CMRx
#define TIMER_CLOCK1  0           //Clock interno MCK/2
#define TIMER_CLOCK2  1           //Clock interno MCK/8
#define TIMER_CLOCK3  2           //Clock interno MCK/32
#define TIMER_CLOCK4  3           //Clock interno MCK/128
#define TCCLKS        0           //Posição do bit TCCLKS no registrador TC_CMRx
#define WPKEY         0x54494D    //PASSWD for Write Protection


//********************************************************************************
//                             Registradores
//********************************************************************************

//Registradores - Timer_0:
#define TC_CCR0   0x40080000  //TC Channel Control Register
#define TC_CMR0   0x40080004  //TC Channel Mode Register
#define TC_CV0    0x40080010  //TC Counter Value Register
#define TC_RA0    0x40080014  //TC Register A
#define TC_RB0    0x40080018  //TC Register B
#define TC_RC0    0x4008001C  //TC Register C
#define TC_SR0    0x40080020  //TC Status Register
#define TC_IER0   0x40080024  //TC Interrupt Enable Register
#define TC_IDR0   0x40080028  //TC Interrupt Disable Register
#define TC_IMR0   0x4008002C  //TC Interrupt Mask Register
#define TC_WPMR0  0x400800E4  //TC Write Protection Mode Register


//Registradores - Power Management Controller (PMC)
#define PMC_PCER0 0x400E0610  //PMC Peripheral Clock Enable Register 0
#define PMC_PCER1 0x400E0700  //PMC Peripheral Clock Enable Register 1
#define PMC_SR    0x400E0688  //PMC Status Register
#define PMC_PCSR0 0x400E0618  //PMC Peripheral Clock Status Register 0

//Registradores - PIO Controller
#define PIOA_PDR  0x400E0E04  //PIO Controller PIOA Disable Register
#define PIOA_ABSR 0x400E0E70  //PIO Peripheral PIOB AB Select Register
#define PIOA_ODR  0x400E0E14  //PIO Controller Output Disable Register
#define PIOB_PDR  0x400E1004  //PIO Controller PIOB Disable Register
#define PIOB_ABSR 0x400E1070  //PIO Peripheral PIOB AB Select Register
#define PIOB_ODR  0x400E1014  //PIO Controller Output Disable Register

//***********************************************************************//

//Ponteiros de registradores - Timer_0
uint32_t *pTC_CCR0 = (uint32_t*)(TC_CCR0);
uint32_t *pTC_CMR0 = (uint32_t*)(TC_CMR0);
uint32_t *pTC_CV0 = (uint32_t*)(TC_CV0);
uint32_t *pTC_RA0 = (uint32_t*)(TC_RA0);
uint32_t *pTC_RB0 = (uint32_t*)(TC_RB0);
uint32_t *pTC_RC0 = (uint32_t*)(TC_RC0);
uint32_t *pTC_SR0 = (uint32_t*)(TC_SR0);
uint32_t *pTC_IMR0 = (uint32_t*)(TC_IMR0);
uint32_t *pTC_IER0 = (uint32_t*)(TC_IER0);
uint32_t *pTC_IDR0 = (uint32_t*)(TC_IDR0);
uint32_t *pTC_WPMR0 = (uint32_t*)(TC_WPMR0);

//Ponteiros de registradores - PMC
uint32_t *pPMC_PCER0 = (uint32_t*)(PMC_PCER0);
uint32_t *pPMC_PCER1 = (uint32_t*)(PMC_PCER1);
uint32_t *pPMC_PCSR0 = (uint32_t*)(PMC_PCSR0);
uint32_t *pPMC_SR    = (uint32_t*)(PMC_SR);

uint32_t *pPIOB_PDR = (uint32_t*)(PIOB_PDR);
uint32_t *pPIOB_ABSR = (uint32_t*)(PIOB_ABSR);
uint32_t *pPIOB_ODR = (uint32_t*)(PIOB_ODR);



//********************************************************************************
//                             Variáveis
//********************************************************************************

uint32_t raValue = 0, rbValue = 0; //variáveis de leitura dos registradores RA e RB


//********************************************************************************
//                             Funções
//********************************************************************************

void unableWriteProtection_timer0(){
  //Timer 0:
  *pTC_WPMR0 |= (WPKEY << 8); //insere a chave de proteção de escrita
  *pTC_WPMR0 &= ~(0x01 << 0); //desabilita a proteção de escrita
}


/* void pioConfig()
 *  Configuração do pino de entrada do sinal (PB25)
*/
void pioConfig(){
  
  //Configuração de portas I/O para o timer 0
  
  *pPIOB_PDR |= 0x01 << 25; //Habilita o pino PB25 (pino digital 2) para ser controlado pelo periférico (Timer0)
  *pPIOB_ABSR |= 0x01 << 25; //Configura o pino PB25 para a função periférica B
  *pPIOB_ODR |= 0x01 << 25; //Configura o pino como entrada
}


/* void enablePMCclock(uint8_t id)
 *  Habililta o clock do periférico pelo PMC (Power Management Controller), sendo "id" o ID
 *  do periférico.
*/
void enablePMCclock(uint8_t id){
  if(id <= 31)
    *pPMC_PCER0 |= 0x01 << id;  //habilita o clock pelo registrador PMC_PCER0 se o ID for até 31
  else
    if(id >= 32 && id <= 44)
      *pPMC_PCER1 |= 0x01 << id; //habilita o clock pelo registrador PMC_PCER0 se o ID for de 32 até 44
}


/* void tc0config()
 *  Configuração do Timer_0
*/
void tc0config(){
  //*pPMC_PCER0 |= TC0_ID; //habilita o clock do Timer Counter 0 no controlador PMC
  enablePMCclock(TC0_ID);
  pioConfig(); //configura pino de entrada do sinal
  
  //Configurando canal 0:
  *pTC_CCR0 |= CLKEN; //seta o bit CLKEN para habilitar o clock
  *pTC_CCR0 |= 0x01 << 2; //software trigger
  unableWriteProtection_timer0();
  *pTC_CMR0 |= CAPT; //configura o TC para modo captura
  *pTC_CMR0 |= TIMER_CLOCK1 << TCCLKS; //seleciona prescaler

  //Configurando interrupção para leitura do sinal:
  NVIC_EnableIRQ(TC0_IRQn); //habilita interrupção do timer0
  NVIC_SetPriority(TC0_IRQn, 15); //configura nível de prioridade
  *pTC_IER0 |= 1 << 5; //Habilitando interrupção por RA loading
  *pTC_IDR0 &= ~(1 << 5); //apaga o bit que desabilita a interrupção por RA loading - canal 0
}

/* void captureConfig()
 *  Configuração do modo de captura. O registrador RA é carregado em nível alto,
 *  enquanto RB é carregado em nível baixo.
*/
void captureConfig(){
  //Timer_0
  *pTC_CMR0 |= 0x01 << 16; //RA é carregado quando é detectada uma borda de subida no sinal em TIOA
  *pTC_CMR0 |= 0x02 << 18; //RB é carregado quando é detectada uma borda de descida no sinal em TIOA
}


/* void captureRA()
 *  Retorna o valor de RA.
*/
uint32_t captureRA(){
  return *pTC_RA0;
}



#endif
