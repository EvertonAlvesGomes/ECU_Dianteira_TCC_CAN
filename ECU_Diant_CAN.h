/* ECU_Diant_CAN.h
 *  Implementação do CAN para ECU Dianteira.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/

#ifndef _ECU_DIANT_CAN_H
#define _ECU_DIANT_CAN_H

//********************************************************************************
//                             #includes
//********************************************************************************

#include "D:\Documentos\CEFET-MG\Disciplinas\9 Periodo\TCC2\Projeto\Firmware\Final\CanCefast\CanCefast.h"

//********************************************************************************
//                             #defines
//********************************************************************************

//IDs das mensagens:
/*#define ID_MSG_1            0x2702        //pressD | tempD | velD
#define ID_MSG_2            0x3066        //pressT | tempT | rpm | velT
#define ID_MSG_3            0xEA27        //com*/

//IDs das mailboxes:
#define ID_MB_TX            ID_MSG_1      //ID mailbox tx
#define ID_MB_RX_A          ID_MSG_2      //ID mailbox rx A
#define ID_MB_RX_B          ID_MSG_3      //ID mailbox rx B

//Índices das mailboxes: 0x00 a 0x07 para microcontroladores SAM3X8E
#define MB_TX_INDEX         0x00        //Índice Mailbox tx
#define MB_RX_A_INDEX       0x01        //Indice mailbox rx A
#define MB_RX_B_INDEX       0x02        //Indice mailbox rx B

//Masks para habilitar interrupção das mailboxes
#define ENABLE_MB0_INTERRUPT  (1 << 0)      //Habilita interrupção da mailbox 0
#define ENABLE_MB1_INTERRUPT  (1 << 1)      //Habilita interrupção da mailbox 1
#define ENABLE_MB2_INTERRUPT  (1 << 2)      //Habilita interrupção da mailbox 2
#define ENABLE_MB3_INTERRUPT  (1 << 3)      //Habilita interrupção da mailbox 3
#define ENABLE_MB4_INTERRUPT  (1 << 4)      //Habilita interrupção da mailbox 4
#define ENABLE_MB5_INTERRUPT  (1 << 5)      //Habilita interrupção da mailbox 5
#define ENABLE_MB7_INTERRUPT  (1 << 6)      //Habilita interrupção da mailbox 6
#define ENABLE_MB8_INTERRUPT  (1 << 7)      //Habilita interrupção da mailbox 7


//********************************************************************************
//                             Variáveis
//********************************************************************************

//Variáveis de monitoramento
uint8_t send_status, rec_status_1, rec_status_2;
uint32_t can_sr=0;

//Mailboxes:
can_mb_conf_t can_mailbox_tx;                  //mailbox tx
can_mb_conf_t can_mailbox_rx_a;                //mailbox rx_a
can_mb_conf_t can_mailbox_rx_b;                //mailbox rx_b


//********************************************************************************
//                             Funções
//********************************************************************************

/* uint32_t ecu_diant_can_join_to_send
 *  Agrupa duas variaveis de 16 bits em uma de 32 bits e retorna o valor desta variavel
*/
uint32_t ecu_diant_can_join_to_send(uint16_t var_2, uint16_t var_1){
  uint32_t retval;
  retval = (uint32_t)(var_2);
  retval = (retval << 16) | (var_1);
  return retval;
}

/* uint16_t ecu_diant_can_split_to_read
 *  Seleciona o dado nos registradores de dados das mailboxes de recepcao.
 *  uint8_t pos = varia de 0 a 2 e indica qual dado deve ser selecionado
*/
uint16_t ecu_diant_can_split_to_read(uint8_t pos){
  uint16_t retval;

  switch(pos) {
    //case 0 -> velocidade traseira
    case 0 : {
      retval = ((can_mailbox_rx_a.ul_datal) & (0x0000FFFF));
      break;
    }
    //case 1 -> rpm
    case 1 : {
      retval = (uint16_t)((can_mailbox_rx_a.ul_datal & 0xFFFF0000) >> 16);
      break;
    }
    //case 2 -> temperatura traseira
    case 2 : {
      retval = ((can_mailbox_rx_a.ul_datah) & (0x0000FFFF));
      break;
    }
    //case 3 -> pressao traseira
    case 3 : {
      retval = (uint16_t)(((can_mailbox_rx_a.ul_datah) & (0xFFFF0000)) >> 16);
      break;
    }
    //case 4 -> combustivel
    case 4 : {
      retval = ((can_mailbox_rx_b.ul_datal) & (0x0000FFFF));
      break;
    }
  }
  return retval;
  
}


void ecu_diant_can_data_monitoring(){
  //Mailbox de transmissão:
  Serial.print("TX: ");
  Serial.println(send_status);

  //Mailboxes de recepção:
  Serial.print("VT: ");
  Serial.println(ecu_diant_can_split_to_read(0));
  Serial.print("RPM: ");
  Serial.println(ecu_diant_can_split_to_read(1));
  Serial.print("TEMPT: ");
  Serial.println(ecu_diant_can_split_to_read(2));
  Serial.print("PRESST: ");
  Serial.println(ecu_diant_can_split_to_read(3));
  Serial.print("COMB: ");
  Serial.println(ecu_diant_can_split_to_read(4));
  Serial.println("");

}

void ecu_diant_can_can_monitoring(){
  //Mailbox de transmissão:
  Serial.print("TX: ");
  Serial.println(send_status);

  //Mailboxes de recepção:
  Serial.print("RX_A: ");
  Serial.print(can_mailbox_rx_a.ul_datah, HEX);
  Serial.print(" ");
  Serial.println(can_mailbox_rx_a.ul_datal, HEX);
  //Serial.println(rec_status_a);

  Serial.print("RX_B: ");
  Serial.print(can_mailbox_rx_b.ul_datah, HEX);
  Serial.print(" ");
  Serial.println(can_mailbox_rx_b.ul_datal, HEX);
  //Serial.println(rec_status_b);
  Serial.println("");
}

void ecu_diant_can_status(){
  Serial.println(*pCAN_SR, HEX);
}


#endif
