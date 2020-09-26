/* ECU_Diant_ADC.h
 *  Implementação do conversor AD, destinado à leitura da pressão de linha de freio.
 *  Autor: Everton A. Gomes
 *  Data: 19/09/2020
*/


//********************************************************************************
//                Informações sobre o ADC do Arduino DUE
//********************************************************************************

/* 

- Mapeamento de entradas analógicas:
    Analog In 0     -   PA16  -   AD7
    Analog In 1     -   PA24  -   AD6
    Analog In 2     -   PA23  -   AD5
    Analog In 3     -   PA22  -   AD4
    Analog In 4     -   PA6   -   AD3
    Analog In 5     -   PA4   -   AD2
    Analog In 6     -   PA3   -   AD1
    Analog In 7     -   PA2   -   AD0
    Analog In 8     -   PB17  -   AD10
    Analog In 9     -   PB18  -   AD11
    Analog In 10    -   PB19  -   AD12
    Analog In 11    -   PB20  -   AD13

- Resolução 12 bits, podendo ser configurada para 10 bits
- A conversão vai de 0V a ADVREF
- O valor de conversão é armazenado em um registrador comum a todos os canais, assim como no registrador do próprio canal
- Trigger configurável: ADTRG, podendo ser externo, via software ou proveniente de triggers internos dos contadores (timer counters)
- Um capacitor de desacoplamento externo é necessário para filtragem de ruído. Verificar se na placa do DUE existe esse capacitor
- O usuário pode configurar timings do ADC: startup time, tracking time, etc.
- Possível configurar entrada simples ou diferencial
- 16 entradas digitais
- Pinos relacionados ao ADC>
  - VDDANA -> Analog power supply
  - ADVREF -> tensão de referência
  - ADO - AD15 -> Canais de entrada analógicos; AD15 - Sensor de temperatura
  - ADTRG -> Trigger (externo ou interno - TIOA output ou Timer Counter Channels)
  
- Product Dependencies:
  - Power management: é necessário habilitar o clock pelo PMC (Power Management Controller) para o ADC operar. O clock do ADC
  é o mesmo clock dos periféricos: MCK. Se a aplicação não demandar operações de ADC, o clock desse periférico pode ser desabilitado
  e habilitado novamente quando necessário. Para configurar o controlador do AD não é necessário que o clock do ADC esteja habilitado.
  - Fontes de interrupção: se for usada alguma ISR para ADC, é necessário configurar o NVIC para tal. ID_ADC = 37
  - Entradas analógicas: são multiplexadas com as linhas PIO (Parallel Input/Output). A atribuição de entrada analógica é automaticamente
  feita assim que o canal analógico correspondente é habilitado escrevendo-se o registrador ADC_CHER.
  - Sensor de temperatura: é conectado ao canal 15 (AD15). Fornece uma tensão V_T proporcional à temperatura absoluta (PTAT). O sensor de
  temperatura é ativado setando-se o bit TSON no registrador ADC_ACR.
  - Linhas I/O: lembrar que o pino ADTRG é conectado ao PA11, periférico B. Podem existir outros periféricos que compartilham o PA11.
  - Timer triggers: os TC's podem ser usados como fonte de trigger de hardware. Nesta aplicação, como o trigger é feito via software, eles 
  podem ser ignorados.
  
- Functional description:
  - Configuração de tempo: tracking time & transfer time, dependentes do clock (dado em função do MCK). O valor de tracking e transfer
  é configurado no registrador ADC_MR (Mode Register). A seleção do clock é feita pelo prescaler - PRESCAL no registrador ADC_MR.
  PRESCAL = 0 -> ADC_CLK = MCK/2, PRESCAL = 0xFF (255) -> ADC_CLK = MCK/512.
    O tracking time pode ser maior ou menor que o tempo de conversão. Nesse caso, ocorrem os seguintes eventos:
      - Ocorre o trigger (software, hardware, externo, etc.);
      - ADC_ON = 1 no próximo ciclo de clock
      - Start up time: tracking do CH0
      - Finalizado o tracking do CH0, inicia-se o período de transferência (transfer period) e ADC_Start = 1 durante um pulso de clock
      - Fim do transfer time do CH0 -> inicia a conversão do CH0 e o tracking do CH1
      - A conversão do CH0 termina e seu valor é disponibilizado no registrador ADC_LCDR (Last Converted Data Register), mas o tracking
      do CH1 ainda não foi finalizado
      - Finaliza o tracking do CH1 -> Inicia transfer do CH1 e ADC_Start = 1 durante um pulso de clock
      - Transfer do CH1 termina -> Inicia a conversão do CH1 e o tracking do CH2
      ...
      - Finaliza a conversão do CH15
    Quando o tracking time < conversion time: conversão do CH0 ocorre ao mesmo tempo que o tracking do CH1, conversão do CH1 ocorre ao mesmo
    tempo que o tracking do CH2, e assim por diante.
  - Leitura do registrador ADC_LCDR -> apaga o flag DRDY. Esse flag está presente no registrador ADC_IER (Interrupt Enable Register)
  - Resolução do conversor: para selecionar 10 bits, deve-se setar o bit LOWRES no registrador ADC_MR (Mode Register).
  - Quando uma conversão é finalizada, o valor digital é armazenado no registrador ADC_CDRx do canal atual e no ADC_LCDR. Se o bit TAG no registrador
  ADC_EMR (Extended Mode Register) for setado, o ADC_LCDR apresenta o número do canal associado à última conversão. A finalização da conversão seta 
  o bit EOC no registrador de status (ADC_SR) e o bit DRDY; ler o ADC_CDRx apaga o flag EOC.
    Se o ADC_CDRx não é lido antes dos próximos dados serem convertidos, um erro de sobrecarga (Overrun Error) é gerado, setando-se o flag OVREx 
  no registrador ADC_OVER. Da mesma forma, o bit GOVRE é setado no registrador ADC_SR quando ocorre sobrecarga. O bit OVREx é automaticamente apagado
  após a leitura do registrador ADC_OVER, assim como o GOVRE é apagado quando o ADC_SR é lido.
  - Software trigger: escreve-se 1 no bit START no ADC_CR (Control Register).
  - Ganho de entrada: usar ganho unitário. O ganho é programável pelos bits GAIN no ADC_CGR (Channel Gain Register). O valor configurado em GAIN
  determina o valor do ganho a depender do parâmetro DIFF, que diz respeito à operação diferencial do AD. Nesta aplicação, o ADC não será diferencial
  (configuração single-ended). Cada canal possui um DIFF correspondente no ADC_COR (Channel Offset Register). DIFF = 0 será usado, e, como se deseja
  ganho unitário, GAIN = 00.
  - ADC Timings: configurar Startup Time, Tracking Time e Transfer Time para cada canal no registrador ADC_MR

*/ 

#ifndef _ECU_DIANT_ADC_H
#define _ECU_DIANT_ADC_H

//Adc myADC; //struct ADC


//********************************************************************************
//                             #defines
//********************************************************************************

#define ADC_WPKEY 0x414443  //Write Protection Key
#define SUT64     1         //Start Up Time
#define PRESCAL   0x0F      //Prescaler for ADC Clock
#define TRACKTIM  0x01      //Tracking time
#define TRANSFER  0x10      //Transfer time
#define ADC_ID    37

//********************************************************************************
//                             Registradores
//********************************************************************************
//Registers
#define ADC_CR      REG_ADC_CR      //Control Register
#define ADC_MR      REG_ADC_MR      //Mode Register
#define ADC_SEQR1   REG_ADC_SEQR1   //Channel Sequence Register 1
#define ADC_SEQR2   REG_ADC_SEQR2   //Channel Sequence Register 2
#define ADC_CHER    REG_ADC_CHER    //Channel Enable Register
#define ADC_CHDR    REG_ADC_CHDR    //Channel Disable Register
#define ADC_CHSR    REG_ADC_CHSR    //Channel Status Register
#define ADC_LCDR    REG_ADC_LCDR    //Last Converted Data Register
#define ADC_IER     REG_ADC_IER     //Interrupt Enable Register
#define ADC_IDR     REG_ADC_IDR     //Interrupt Disable Register
#define ADC_IMR     REG_ADC_IMR     //Interrupt Mask Register
#define ADC_ISR     REG_ADC_ISR     //Interrupt Status Register
#define ADC_OVER    REG_ADC_OVER    //Overrun Status Register
#define ADC_EMR     REG_ADC_EMR     //Extended Mode Register
#define ADC_CWR     REG_ADC_CWR     //Compare Window Register
#define ADC_CGR     REG_ADC_CGR     //Channel Gain Register
#define ADC_COR     REG_ADC_COR     //Channel Offset Register
#define ADC_CDR0    REG_ADC_CDR     //Channel Data Register 0
#define ADC_CDR1    myADC.ADC_CDR[1]    //Channel Data Register 1 
#define ADC_CDR2    REG_ADC_CDR2    //Channel Data Register 2
#define ADC_CDR3    REG_ADC_CDR3    //Channel Data Register 3
#define ADC_CDR4    REG_ADC_CDR4    //Channel Data Register 4
#define ADC_CDR5    REG_ADC_CDR5    //Channel Data Register 5
#define ADC_CDR6    REG_ADC_CDR6    //Channel Data Register 6
#define ADC_CDR7    REG_ADC_CDR7    //Channel Data Register 7
#define ADC_CDR8    REG_ADC_CDR8    //Channel Data Register 8
#define ADC_CDR9    REG_ADC_CDR9    //Channel Data Register 9
#define ADC_CDR10   REG_ADC_CDR10   //Channel Data Register 10
#define ADC_CDR11   REG_ADC_CDR11   //Channel Data Register 11
#define ADC_CDR12   REG_ADC_CDR12   //Channel Data Register 12
#define ADC_CDR13   REG_ADC_CDR13   //Channel Data Register 13
#define ADC_CDR14   REG_ADC_CDR14   //Channel Data Register 14
#define ADC_CDR15   REG_ADC_CDR15   //Channel Data Register 15*/
#define ADC_ACR     REG_ADC_ACR     //Analog Control Register
#define ADC_WPMR    REG_ADC_WPMR    //Write Protect Mode Register
#define ADC_WPSR    REG_ADC_WPSR    //Write Protect Status Register 
#define PIOA_PDR    REG_PIOA_PDR    //PIO Disable Register
#define PMC_PCER1   REG_PMC_PCER1   //PMC Peripheral Clock Enable Register 0


//********************************************************************************
//                             Funções
//********************************************************************************


void pioAdcConfig(){
  PIOA_PDR |= 0x01 << 2; //Habilita o pino PA2 para ser controlado pelo periférico (ADC)
  //PIOB_ABSR |= 0x01 << 25;  //Configura a função periférica do pino
}

void adcConfig(){
  pioAdcConfig();

  PMC_PCER1 |= (0x01 << ADC_ID); //habilita o clock do ADC no controlador PMC
  
  //ADC_WPMR = (ADC_WPKEY << 8) | (0x00 << 0); //Disables write protect

  //Setting ADC_MR Register:
  ADC_MR |= 0x00 << 0; //Hardware triggers are disabled, trigger can be made only by software
  ADC_MR |= 0x00 << 4; //12-bit resolution
  ADC_MR |= 0x00 << 5; //Normal mode is selected
  ADC_MR |= PRESCAL << 8; //Setting ADC clock prescaler
  ADC_MR |= SUT64 << 16;  //Setting startup time
  ADC_MR |= TRACKTIM << 24; //Setting tracking time
  ADC_MR |= TRANSFER << 28; //Setting transfer time

  ADC_CHER |= 0x01 << 0;  //Enabling channel 0

  ADC_CGR = 0; //All channels have gain 1
  ADC_COR |= 0x00 << 16; //Enable single ended mode for channel 0

  ADC_EMR |= 0x01 << 24; //Enables TAG option 
  
  //ADC_CR = 1; //Resets the ADC simulating a hardware reset
  ADC_CR |= 1 << 1; //Begins ADC conversion
}

uint16_t adcRead(){
  uint16_t current_value = 0;
  ADC_CR |= 1 << 1; //Begins ADC conversion
  //If the current channel in ADC_LCDR is CH0, the ADC_LCDR value is returned
  if((ADC_LCDR >> 12) == 0){
    return ADC_LCDR;
    current_value = ADC_LCDR;
  }
  else {
    return current_value;
  }
  //return ADC_LCDR;
  //return ADC_CDR0;
}

#endif
