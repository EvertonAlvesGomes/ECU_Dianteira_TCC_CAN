/* ECU_Diant_SensPressao.h
 *  Implementação do sensor de pressão.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/

#ifndef _ECU_DIANT_SENSPRESSAO_H
#define _ECU_DIANT_SENSPRESSAO_H

//********************************************************************************
//                             #defines
//********************************************************************************

#define ADC_RANGE             4096      //Range do ADC de 12 bits
#define ADC_PRESS_CHANNEL     0         //Canal do AD a ser utilizado pelo sensor de pressão

//********************************************************************************
//                             #includes
//********************************************************************************

#include "ECU_Diant_ADC.h"

//********************************************************************************
//                             Variáveis
//********************************************************************************

float pressao=0;

//********************************************************************************
//                             Funções
//********************************************************************************

void ecu_diant_sens_pressao_meas_config(){
  //adcConfig();
  ecu_diant_adc_enable_channel(ADC_PRESS_CHANNEL);
}

float ecu_diant_sens_pressao_calcula_pressao(){
  /*
   * Equação do sensor de pressão: Vout = Vcc*(0.08*P + 0.1), Vcc = 5V. Pressão P em MPa
   * Como Vcc = 3.3V neste trabalho, considerou-se a faixa de trabalho de tensão do sensor proporcional a 3.3V.
   * Dessa forma, se o sensor trabalha 0.5 V a 4.5 V com Vcc = 5V, foi considerada a faixa 0.33 a 2,97 V.
   * Multiplica-se a pressão por 10 para converter de MPa para bar.
   */
  pressao = 10*12.5*(((float)ecu_diant_adc_read_channel(ADC_PRESS_CHANNEL))/ADC_RANGE - 0.1);
  //pressao = 12.5*(((float)ecu_diant_adc_read_last_converted())/ADC_RANGE - 0.1);
  return pressao;
}

#endif
