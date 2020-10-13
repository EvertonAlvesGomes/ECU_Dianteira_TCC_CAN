/* ECU_Dianteira.ino
 *  Firmware da ECU Dianteira do projeto CAN desenvolvido para a equipe CEFAST Baja.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/



//********************************************************************************
//                             #includes
//********************************************************************************

#include "ECU_Diant_Defines.h"
#include "ECU_Diant_CAN.h"
#include "ECU_Diant_SensPressao.h"
#include "ECU_Diant_SensVelocidade.h"
#include "ECU_Diant_CAN_Deploying.h"
#include "ECU_Diant_Peripheral_Deploying.h"
#include "ECU_Diant_Temp.h"


//********************************************************************************
//                             Variáveis
//********************************************************************************

uint16_t velD = 0;      //velocidade dianteira
//uint16_t tempD = 0 ;    //temperatura disco dianteiro
float pressD = 0;       //pressão freio dianteiro

uint8_t tick = 0;       //temporizador de inicialização do CAN


void setup(){
  #ifdef ENABLE_SERIAL_COMMUNICATION
    Serial.begin(9600);
  #endif

  //Configura medição das grandezas se ENABLE_PERIPHERAL_COMMUNICATION estiver habilitado
  #ifdef ENABLE_PERIPHERAL_COMMUNICATION 
    ecu_diant_adc_config();   //configura o conversor AD
    ecu_diant_sens_velocidade_meas_config();     //configura medição de velocidade
    ecu_diant_sens_pressao_meas_config();   //configura medição de pressão
    ecu_diant_temp_meas_config();   //configura medição de temperatura
  #endif
  
  #ifdef ENABLE_CAN_COMMUNICATION
    startCan();   //inicializa interface CAN
    
    canConfigMailboxTx(&can_mailbox_tx, MB_TX_INDEX);   //configura mailbox tx
    canConfigMailboxRx(&can_mailbox_rx_a, MB_RX_A_INDEX, ID_MB_RX_A);   //configura mailbox rx A
    canConfigMailboxRx(&can_mailbox_rx_b, MB_RX_B_INDEX, ID_MB_RX_B);   //configura mailbox rx B

    //Aguarda inicialização do CAN
    while(!(*pCAN_SR & (1 << 21))){
      tick++;
      if(tick >= TIMEOUT_CAN){
        #ifdef ENABLE_SERIAL_COMMUNICATION
          Serial.println("Falha na inicialização do CAN.\n");
        #endif
        return;
      }
    }
    #ifdef ENABLE_SERIAL_COMMUNICATION
      Serial.println("CAN Inicializado.\n\n");
    #endif
    
    can_enable_interrupts(ENABLE_MB1_INTERRUPT | ENABLE_MB2_INTERRUPT); //Habilita interrupção das mailboxes de recepção
  #endif
  
}

void loop(){
  #ifdef ENABLE_CAN_COMMUNICATION
    tick = 0; //reinicia temporizador

    //Se o ENABLE_CAN_DEPLOYING estiver habilitado, usam-se valores aleatórios para as grandezas medidas
    #ifdef ENABLE_CAN_DEPLOYING
      velD = velocidade_diant[random(0,100)];
      tempD = temp_diant[random(0,100)];
      pressD = pressao_diant[random(0,10)];
    #endif

    //Se CAN_DEPLOYING não estiver habilitado, usam-se valores calculados pelos periféricos para as grandezas medidas
    #ifndef ENABLE_CAN_DEPLOYING
      velD = ecu_diant_sens_velocidade_calcula_velocidade();
      pressD = ecu_diant_sens_pressao_calcula_pressao();
      ecu_diant_temp_calcula_temp();
    #endif

    /* Formatação da mensagem: 
     *  CAN_DATA_HIGH: MSB = 0, LSB = pressD
     *  CAN_DATA_LOW: MSB = tempD, LSB = velD 
    */
    send_status = canSend(&can_mailbox_tx, ID_MB_TX, ecu_diant_can_join_to_send(0,pressD), ecu_diant_can_join_to_send(tempD,velD));
    can_global_send_transfer_cmd(CAN0, (0x1u << MB_TX_INDEX));  //envia comando para a mensagem ser inserida no barramento

    //Se a comunicação serial estiver habilitada, os dados são escritos na serial
    #ifdef ENABLE_SERIAL_COMMUNICATION
      ecu_diant_can_can_monitoring();
    #endif
    
  #endif

  #ifdef ENABLE_PERIPHERAL_DEPLOYING
    //Leitura dos periféricos
    #ifdef ENABLE_SERIAL_COMMUNICATION
      //Serial.print("V: ");
      Serial.println(ecu_diant_sens_velocidade_calcula_velocidade());
      //Serial.print("P: ");
      Serial.println(ecu_diant_sens_pressao_calcula_pressao());
      Serial.println("");
      /*Serial.print("ADC CH0: ");
      Serial.println(ecu_diant_adc_read_channel(ADC_CH0));
      Serial.print("ADC CH10: ");
      Serial.println(ecu_diant_adc_read_channel(ADC_CH10));*/
      /*Serial.print("Status: ");
      Serial.println(ADC_ISR, HEX);*/

    #endif

  #endif
  
}



//********************************************************************************
//                             ISRs
//********************************************************************************

//Timer0_ISR:
void TC0_Handler(void){
  ra_atual = ecu_diant_timer0_captureRA(); //leitura do registrador RA
  tc_sr = *pTC_SR0; //leitura do registrador de status para permitir o processador retornara pra thread mode
}


//CAN_ISR:
void CAN0_Handler(void){

  //RX mailbox A
  if((*pCAN_MSR_1 & 0xFFFFFF) >> 23){
    //se o flag MDRY estiver setado
    rec_status_1 = can_mailbox_read(CAN0, &can_mailbox_rx_a); //pressT-tempT-rpm-velT
  }
  
  //RX mailbox B
  if((*pCAN_MSR_2 & 0xFFFFFF) >> 23){
    rec_status_2 = can_mailbox_read(CAN0, &can_mailbox_rx_b); //comb
  }
  
  can_sr = *pCAN_SR; //leitura do registrador de status (libera thread mode)
}
