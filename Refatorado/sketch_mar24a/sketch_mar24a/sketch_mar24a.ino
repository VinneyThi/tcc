#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "fila1.h"

static const u1_t PROGMEM APPEUI[8] = {0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

static const u1_t PROGMEM DEVEUI[8] = {0xA8, 0x5A, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}


static const u1_t PROGMEM APPKEY[16] = {0x45, 0x46, 0x06, 0xCB, 0xD8, 0xE5, 0xDB, 0x31, 0xFC, 0x20, 0xD8, 0xF0, 0x2B, 0xE9, 0x1D, 0xCE};
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}


static osjob_t sendjob; // cria uma variavel de trabalho que é usado na função de agendamento da biblioteca.

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

// Pin mapping for Esp32
const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = {26, 33, 32}, // 26,35,34 | 26,33,32
};


void tcc2();
void do_sendRenv(osjob_t* j);
void do_send(osjob_t* j);

int flagEnvioRapido = 0;
int flagFalhaBuff   = 0; // falha do envio do buff
int flagReenvio     = 0; // falha no 11 priemiro
int flagThread      = 0;
int flagConfV       = 0; // apos falha do envio do buff e consegue enviar
int auxAtraso       = 0;
int contEnvio       = 0;


uint8_t mydata[1];

fila  *buff   = new PROGMEM fila;
fila  *backup = new PROGMEM fila;

void carregaBUFF(fila* ptrbackup, fila *ptrbuff)
{
  

  for (int i = 0; i < 5 ; i++)
  { uint8_t* ptrAuxDado = new uint8_t;

    *ptrAuxDado = ptrbackup->getDadoPosConf(i);
    
    
    ptrbuff->insereFinal(ptrAuxDado);
  }
  
}


void tcc2(){

  auxAtraso = backup->getQuantidade() - backup->getQuantidadeConfima();

  if (LMIC.txrxFlags & TXRX_ACK)
  { 
   int auxTamBuff = buff->getQuantidade();
    
    if (flagReenvio && !flagConfV)
    {
      if(buff->getQuantidade()> 0) // add 06/07
        for (int i = 0 ; i < auxTamBuff  ; i++) //p
          buff->removeFila();
      
      flagReenvio = 0;
      flagFalhaBuff = 0;
      backup->setPTRconfirmado(5);
      
      /*if (auxAtraso >= 5)//
        {
        carregaBUFF(backup,buff);
        flagEnvioRapido = 1
        backup->setPTRconfirmado(5);
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }*/
      flagThread = 0;
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
    }
    else if (!flagReenvio && flagFalhaBuff && !flagConfV)
    {   
      flagFalhaBuff = 0;
      if (auxAtraso >= 5)//
      {
        carregaBUFF(backup, buff);
        flagEnvioRapido = 1;
      
        backup->setPTRconfirmado(5);
        
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
      }
      else
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
    }
    else if (flagConfV) // incluir um else if*
    {
      
      flagFalhaBuff = 0; // coloquei*
      flagConfV = 0;
      if (auxAtraso > 5 )
      {           
        carregaBUFF(backup, buff);
        
        flagEnvioRapido = 1;
        backup->setPTRconfirmado(5);
                  
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
      }
    }
    else if (auxAtraso >= 5)//
    {          
      carregaBUFF(backup, buff);
      flagEnvioRapido = 1;
  
      backup->setPTRconfirmado(5);    
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
    }
    else
    {
      
      flagConfV = 0;
      flagThread = 0;
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
    
  
      //flagFalhaBuff =0;
      //flagReenvio =0;
    }
  }
//****************************************************************************                                      
      else if (!(LMIC.txrxFlags & TXRX_ACK))
      {        
        if (flagReenvio) //!flagFalhaBuff
        
        {
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);/// desvio ?
          //break;
        }
        else if (flagFalhaBuff && buff->getQuantidade() < 5)
        {
          flagConfV = 1 ; // yflagEnvioRapido
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
        }
        else if (flagConfV )
        {

          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
        }

        else if (auxAtraso >= 5 && flagEnvioRapido && buff->getQuantidade() == 0) //p
        {
          carregaBUFF(backup, buff);
          backup->setPTRconfirmado(5);
                    
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }
        else if (auxAtraso > 0 && flagEnvioRapido && buff->getQuantidade() > 0)
        {
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }
        else
        { 
          int auxB =  buff->getQuantidade();
          if ( auxB > 0 && flagThread)
            for (int i = 0 ; i < auxB  ; i++) //p
              buff->removeFila();

          flagEnvioRapido = 0;
          flagThread = 0; /*******/

          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
        }
      }


      //      // Schedule next transmission
      //            if( flagReenvio && !flagConfV)
      //            {
      //              flagReenvio =0;
      //              os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      //            }
      //           else if (flagReenvio && flagFalhaBuff)
      //                  os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
}


void onEvent (ev_t ev) {
  
  
  switch (ev) {
    case EV_JOINING:
      
      break;
    case EV_JOINED:
      
      {
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
        LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
        
        
        
        
        
        for (int i = 0; i < sizeof(artKey); ++i) {
          
        }
        
        
        for (int i = 0; i < sizeof(nwkKey); ++i) {
          
        }
        
      }
      
      // Disable link check validation (automatically enabled
      // during join, but because slow data rates change max TX
      // size, we don't use it in this example.
      LMIC_setLinkCheckMode(0);
      break;

    case EV_JOIN_FAILED:
      
      break;
    case EV_REJOIN_FAILED:
      
      break;
      break;
    case EV_TXCOMPLETE:
     

      tcc2();
     
      break;
    case EV_LOST_TSYNC:
      
      break;
    case EV_RESET:
      
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      
      break;
    case EV_LINK_DEAD:
      
      break;
    case EV_LINK_ALIVE:
      
      break;

    case EV_TXSTART:
      
      break;
    default:
      
      
      break;
  }
}

void do_sendRenv(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    
  } 
  else {
      
    uint8_t *ptrAuxDate = new uint8_t;
    *ptrAuxDate = buff->getDado();
    
    uint8_t myaux[1];
    myaux[0] = *ptrAuxDate;

    if (buff->getQuantidade() == 1 && !flagConfV && !flagEnvioRapido) // !flagConfV !flagEnvioRapido
    {
      flagFalhaBuff = 1;
      flagReenvio = 0;
      LMIC_setTxData2(1, myaux, sizeof(myaux), 1);
    }
    else
    {
      LMIC_setTxData2(1, myaux, sizeof(myaux), 0);
    }
    buff->removeFila();
   
  }

}
// Next TX is scheduled after TX_COMPLETE event.


void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {

  } 
  else {

    if (contEnvio < 5 && !flagFalhaBuff && !flagConfV)
    {
      LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    }
    else if ( contEnvio == 5)
    { contEnvio = 0;
      flagReenvio = 1;
      flagThread = 1;
      LMIC_setTxData2(1, mydata, sizeof(mydata), 1);
    }
    else if (flagFalhaBuff)
    {
      flagFalhaBuff = 0; //y
      LMIC_setTxData2(1, mydata, sizeof(mydata), 1);
    }
    else if (flagConfV)
    {
      LMIC_setTxData2(1, mydata, sizeof(mydata), 1);
    }

  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0); //Cria a tarefa "loop2()" com prioridade 1, atribuída ao core 0
  
  mydata[0] = 0x00;

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC_setDrTxpow(DR_SF9, 14);
  LMIC_selectSubBand(1); //ativa um conjunto especifico de canais
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100); // compensação de atrasso no recebimento de janela de donwlink( necessario no otta) configurado para  um erro de 1% 

  // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}

void loop2 (void*z)
{
  while (1)
  {
    mydata[0]++;

    backup->insereFinal(mydata);

    if (buff->getQuantidade() < 5 && !flagThread )
    {
      buff->insereFinal(mydata);     
      contEnvio++;
    }
    delay(10000);
  }
}
