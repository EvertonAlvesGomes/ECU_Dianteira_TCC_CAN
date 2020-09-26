/* ECU_Diant_Defines.h
 *  Conjunto de defines da ECU_Dianteira. Este arquivo deve 
 *  estar incluso em todos os demais arquivos do firmware.
 *  Autor: Everton A. Gomes
 *  Data: 18/09/2020
*/


//********************************************************************************
//                             #defines
//********************************************************************************

#ifndef _ECU_DIANT_DEFINES_H
#define _ECU_DIANT_DEFINES_H

//Defines de comunicação:
#define ENABLE_CAN_COMMUNICATION          //Habilita comunicação CAN
#define ENABLE_PERIPHERAL_COMMUNICATION   //Habilita comunicação com periféricos
#define ENABLE_SERIAL_COMMUNICATION       //Habilita comunicação serial



//********************************************************************************
//                             #ifdefs
//********************************************************************************

#ifdef ENABLE_CAN_COMMUNICATION
  #define ENABLE_CAN_DEPLOYING        //Utilizar apenas durante o uso de valores aleatórios para testar o CAN
#endif

#ifdef ENABLE_PERIPHERAL_COMMUNICATION
  //#define ENABLE_PERIPHERAL_DEPLOYING //Habilita a implementação dos periféricos. Utilizar apenas durante o desenvolvimento dos códigos para essa implementação
#endif

//Se a implementação dos periféricos estiver habilitada, desabilita a comunicação CAN
#ifdef ENABLE_PERIPHERAL_DEPLOYING
  #ifdef ENABLE_CAN_COMMUNICATION
    #undef ENABLE_CAN_COMMUNICATION
  #endif
#endif


#endif
