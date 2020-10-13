/* ECU_Diant_SensVelocidade.h
 *  Implementação da leitura do sensor de velocidade.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/


#ifndef _ECU_DIANT_SENSVELOCIDADE_H
#define _ECU_DIANT_SENSVELOCIDADE_H


//********************************************************************************
//                             #includes
//********************************************************************************

#include "ECU_Diant_Timer.h"

//********************************************************************************
//                             Variáveis
//********************************************************************************

uint32_t ra_atual=0, ra_prev=0; //variaveis para leitura do registrador RA do timer_0
uint32_t tc_sr=0; //variavel para leitura do registrador de status do timer_0
//float veld = 0; //variavel para receber o resultado da velocidade dianteira
uint32_t result=0;


//********************************************************************************
//                             Funções
//********************************************************************************

/* void ecu_diant_sens_velocidade_meas_config()
 *  Configuração do timer para medição da velocidade.
*/
void ecu_diant_sens_velocidade_meas_config(){
  ecu_diant_timer0_config();    //configuração do timer_0 para medição de velocidade
  ecu_diant_timer0_capturemode_config();    //modo de captura do timer_0
}

/* uint16_t calculaVelocidade()
 *  Retorna o valor da velocidade atual.
*/
uint16_t ecu_diant_sens_velocidade_calcula_velocidade(){
  
  if(ra_atual != ra_prev){
    result = ra_atual - ra_prev;
    ra_prev = ra_atual;
  }
  return 42e6/result;
  //return 1/(result*2/84e6); //retorna o valor da velocidade
  
}






#endif
