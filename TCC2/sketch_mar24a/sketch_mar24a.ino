#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "fila1.h"

int flagConfV = 0;   // apos falha do envio do buff e consegue enviar
int flagReenvio = 0; // falha no 11 priemiro
int contEnvio = 0;
int flagThread = 0;
int flagEnvioRapido = 0;
int auxAtraso = 0;
int OldSizeBackup =0;
int  flagStartProd = 0;


uint8_t mydata[1];
uint8_t lastDataSend[1];
int flagFalhaBuff = 0; // falha do envio do buff
fila *buff = new PROGMEM fila;
fila *backup = new PROGMEM fila;
void carregaBUFF(fila *ptrbackup, fila *ptrbuff)
{
  Serial.println(F("**Carrega1** "));

  for (int i = 0; i < 5; i++)
  {
    uint8_t *ptrAuxDado = new uint8_t;

    *ptrAuxDado = ptrbackup->getDadoPosConf( (i+1) );
    Serial.println(i);
    Serial.println(*ptrAuxDado, HEX);
    ptrbuff->insereFinal(ptrAuxDado);
  }
  Serial.println(F("**Carrega2** "));
}


void printSet(fila *ptrbackup)
{
  Serial.println(F("**//printSet** "));

    uint8_t *ptrAuxDado = new uint8_t;
    *ptrAuxDado = ptrbackup->getDadoPosConf(0);
    Serial.println(*ptrAuxDado, HEX);    
  
  Serial.println(F("**//printSet** "));
}

static const u1_t PROGMEM APPEUI[8] = {0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void os_getArtEui(u1_t *buf)
{
  memcpy_P(buf, APPEUI, 8);
}

static const u1_t PROGMEM DEVEUI[8] = {0xA8, 0x5A, 0x04, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui(u1_t *buf)
{
  memcpy_P(buf, DEVEUI, 8);
}

static const u1_t PROGMEM APPKEY[16] = {0x45, 0x46, 0x06, 0xCB, 0xD8, 0xE5, 0xDB, 0x31, 0xFC, 0x20, 0xD8, 0xF0, 0x2B, 0xE9, 0x1D, 0xCE};
void os_getDevKey(u1_t *buf)
{
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

void onEvent(ev_t ev)
{
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev)
  {
  case EV_JOINING:
    Serial.println(F("EV_JOINING"));
    Serial.println(LMIC.rxDelay);
    break;
  case EV_JOINED:
    Serial.println(F("EV_JOINED"));
    {
      flagStartProd = 1;
      Serial.println(LMIC.rxDelay);
      u4_t netid = 0;
      devaddr_t devaddr = 0;
      u1_t nwkKey[16];
      u1_t artKey[16];
      LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
      Serial.print("netid: ");
      Serial.println(netid, DEC);
      Serial.print("devaddr: ");
      Serial.println(devaddr, HEX);
      Serial.print("artKey: ");
      for (int i = 0; i < sizeof(artKey); ++i)
      {
        Serial.print(artKey[i], HEX);
      }
      Serial.println("");
      Serial.print("nwkKey: ");
      for (int i = 0; i < sizeof(nwkKey); ++i)
      {
        Serial.print(nwkKey[i], HEX);
      }
      Serial.println("");
    }
    Serial.println(F("Successful OTAA Join..."));
    // Disable link check validation (automatically enabled
    // during join, but because slow data rates change max TX
    // size, we don't use it in this example.
    LMIC_setLinkCheckMode(0);
    break;

  case EV_JOIN_FAILED:
    Serial.println(F("EV_JOIN_FAILED"));
    break;
  case EV_REJOIN_FAILED:
    Serial.println(F("EV_REJOIN_FAILED"));
    break;
    break;
  case EV_TXCOMPLETE:

    LMIC_setAdrMode(0);
    Serial.println(LMIC.rxDelay);

    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    Serial.print(F("ContEnvio "));
    Serial.print(contEnvio);

    auxAtraso = backup->getQuantidade() - backup->getQuantidadeConfima();
    OldSizeBackup = backup->getQuantidade();
    Serial.println(F("Flags"));
    Serial.print(F("FlagReenvio "));
    Serial.println(flagReenvio);
    Serial.print(F("FlagFalhaBuff "));
    Serial.println(flagFalhaBuff);
    Serial.print(F("FlagConfV "));
    Serial.println(flagConfV);
    Serial.print(F("FlagThread "));
    Serial.println(flagThread);
    Serial.print(F("flagEnvioRapido "));
    Serial.println(flagEnvioRapido);
    Serial.print(F("TAMANHO - POSCONF "));
    Serial.println(auxAtraso);

    if (LMIC.txrxFlags & TXRX_ACK)
    {
      Serial.println(F("Received ack"));
      int auxTamBuff = buff->getQuantidade();
      Serial.print(buff->getQuantidade());
      Serial.println(F(" Tamanho buff")); // 10 minutos crash aqui mostars as flags depois.
      if (flagReenvio && !flagConfV)
      {

        if (buff->getQuantidade() > 0) // add 06/07

          Serial.println(F("Antes do remove  ack"));
        for (int i = 0; i < auxTamBuff; i++) //p
          buff->removeFila();
        Serial.println(F("Deposi do remove  ack"));

        flagReenvio = 0;
        flagFalhaBuff = 0;

        backup->setPTRconfirmado(5); // normal
        //printSet(backup);
        Serial.println(F("**SETCONF** "));
        /*if (auxAtraso >= 5)//
            {
            carregaBUFF(backup,buff);
            flagEnvioRapido = 1;
            Serial.println(F("&&"));
            Serial.print(buff->getQuantidade());
            Serial.println(F(" Tamanho buff apos recarregar"));
            backup->setPTRconfirmado(5);
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
            }*/
        flagThread = 0;

        Serial.println("Volta para envio normal");
        int schedulerTime =0;
        schedulerTime = OldSizeBackup == backup->getQuantidade() ? (TX_INTERVAL / 4 )+2: 1;  //arruamr o cont a mais.
        Serial.println(schedulerTime);
        Serial.println(" schedulerTime");
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
      }
      else if (!flagReenvio && flagFalhaBuff && !flagConfV)
      {
        flagFalhaBuff = 0;

        if (auxAtraso >= 5) //
        {
          carregaBUFF(backup, buff);
          flagEnvioRapido = 1;
          Serial.println(F("*"));
          Serial.print(F(" Tamanho buff apos recarregar "));
          Serial.println(buff->getQuantidade());

          backup->setPTRconfirmado(5);
          //printSet(backup);
          Serial.println(F("**SETCONF** "));
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }

        else
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
      }
      else if (flagConfV) // incluir um else if*
      {
        Serial.println(F("*************"));
        Serial.println(F("FLAGCONFV"));
        Serial.println(F("*************"));
        LMIC.rxDelay = 1;
        flagFalhaBuff = 0; // coloquei*
        flagConfV = 0;
        if (auxAtraso > 5)
        {
          Serial.println(F("*************"));
          Serial.println(F("Show"));
          Serial.println(F("*************"));
          carregaBUFF(backup, buff);
          Serial.println(F("$$$$$$$$$$$$$$$"));
          flagEnvioRapido = 1;
          backup->setPTRconfirmado(5);
          //printSet(backup);
          Serial.println(F("**SETCONF** "));
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }
      }

      else if (auxAtraso >= 5) //
      {

        carregaBUFF(backup, buff);
        flagEnvioRapido = 1;
        LMIC.rxDelay = 1;
        Serial.println(F("*"));
        Serial.print(F(" Tamanho buff apos recarregar "));
        Serial.println(buff->getQuantidade());

        backup->setPTRconfirmado(5);
        //printSet(backup);

        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
      }
      else
      {
        Serial.println("*****10***");
        flagConfV = 0;
        flagThread = 0;
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
      }
    }

    //flagFalhaBuff =0;
    //flagReenvio =0;

    //****************************************************************************
    else if (!(LMIC.txrxFlags & TXRX_ACK))
    {
      Serial.println(F("*************"));
      Serial.println(F("Dont Received ack"));
      Serial.print(F("Tamanho buff "));
      Serial.println(buff->getQuantidade());
      if (flagReenvio) //!flagFalhaBuff

      {
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv); /// desvio ?
        //break;
      }
      else if (flagFalhaBuff && buff->getQuantidade() < 5)
      {
        Serial.print(F("Tamanho buff apos carga via backup "));
        Serial.println(buff->getQuantidade());
        flagConfV = 1; // yflagEnvioRapido
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
      }
      else if (flagConfV)
      {

        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
      }

      else if (auxAtraso >= 5 && flagEnvioRapido && buff->getQuantidade() == 0) //p
      {
        carregaBUFF(backup, buff);
        LMIC.rxDelay = 0;
        backup->setPTRconfirmado(5);
        //printSet(backup);
        Serial.println(F("**SETCONF** "));
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
      }
      else if (auxAtraso > 0 && flagEnvioRapido && buff->getQuantidade() > 0)
      {
        LMIC.rxDelay = 0;
        Serial.print(F("Envio rapido "));
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
      }
      else
      {
        int auxB = buff->getQuantidade();
        if (auxB > 0 && flagThread)
          for (int i = 0; i < auxB; i++) //p
            buff->removeFila();

        Serial.println(buff->getQuantidade());
        flagEnvioRapido = 0;
        flagThread = 0; /*******/
        Serial.println("envio normal");
         int schedulerTime =0;
         schedulerTime = OldSizeBackup == backup->getQuantidade() ? TX_INTERVAL / 4 : 1;
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(schedulerTime), do_send);
      }
    }

    Serial.println("*****Delay***");
    Serial.println(LMIC.rxDelay);
    Serial.println("*****Delay***");
    LMIC.rxDelay = 5;

    //      // Schedule next transmission
    //            if( flagReenvio && !flagConfV)
    //            {
    //              flagReenvio =0;
    //              os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
    //            }
    //           else if (flagReenvio && flagFalhaBuff)
    //                  os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
    break;
  case EV_LOST_TSYNC:
    Serial.println(F("EV_LOST_TSYNC"));
    break;
  case EV_RESET:
    Serial.println(F("EV_RESET"));
    break;
  case EV_RXCOMPLETE:
    // data received in ping slot
    Serial.println(F("EV_RXCOMPLETE"));
    break;
  case EV_LINK_DEAD:
    Serial.println(F("EV_LINK_DEAD"));
    break;
  case EV_LINK_ALIVE:
    Serial.println(F("EV_LINK_ALIVE"));
    break;

  case EV_TXSTART:
    Serial.println(F("EV_TXSTART"));
    break;
  default:
    Serial.print(F("Unknown event: "));
    Serial.println((unsigned)ev);
    break;
  }
}

void do_sendRenv(osjob_t *j)
{
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    Serial.println(F("**do_sendRenv** "));

    uint8_t *ptrAuxDate = new uint8_t;
    *ptrAuxDate = buff->getDado();
    Serial.println(F("ENVIAR o BUFF"));
    Serial.print(*ptrAuxDate, HEX);
    Serial.println(F(" dado buff"));
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
    Serial.println(F("QTN BUFF APOS REMOVER"));
    Serial.println(buff->getQuantidade());
  }

  Serial.println(F("Packet queued"));
  Serial.print(F("Sending packet on frequency: "));
  Serial.println(LMIC.freq);
}
// Next TX is scheduled after TX_COMPLETE event.

void do_send(osjob_t *j)
{
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    
    Serial.println(F("**do_send** "));

    if (contEnvio < 5 && !flagFalhaBuff && !flagConfV)
    {        
            Serial.println(F("!dasdasdasad1 !!"));
      Serial.println(mydata[0], HEX);
      
       //contEnvio = (lastDataSend[0] == mydata[0])  && (contEnvio == 0 ) ? contEnvio: contEnvio +1 ;  // 
     
       Serial.println(contEnvio);
      Serial.println(F("!dasd21dsadas!!"));
      LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    }

    else if (contEnvio >= 5)
    {
      contEnvio = 0;
      flagReenvio = 1;
      flagThread = 1;
      Serial.println(F("!! Enviado com a flag 1 !!"));
      Serial.println(mydata[0], HEX);
      Serial.println(F("!! Enviado com a flag 1 !!"));
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

    Serial.print(F("VALOR DO pacote enviado "));
    Serial.println(mydata[0], HEX);
    Serial.print(contEnvio);
    Serial.println(F(" Valor Contador"));
    Serial.println(F("Packet queued"));

    Serial.print(F("Sending packet on frequency: "));
    Serial.println(LMIC.freq);
   lastDataSend[0] = mydata[0];
  }
  
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup()
{
  Serial.begin(115200);
  xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0); //Cria a tarefa "loop2()" com prioridade 1, atribuída ao core 0
  Serial.print(sizeof(node));
  Serial.println(F("Starting"));
  mydata[0] = 0x00;

  backup->insereFinal(mydata);
  buff->insereFinal(mydata);
  contEnvio++;
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC_setDrTxpow(DR_SF9, 14);
  LMIC_selectSubBand(1);                         //ativa um conjunto especifico de canais
  LMIC_setClockError(MAX_CLOCK_ERROR * 0 / 100); // compensação de atrasso no recebimento de janela de donwlink( necessario no otta) configurado para  um erro de 1%

  // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
}

void loop()
{

  os_runloop_once();
}

void loop2(void *z)
{
  Serial.printf("\n**loop2() em core: %d /n", xPortGetCoreID()); //Mostra no monitor em qual core o loop2() foi chamado
  while (1)
  {
    if( flagStartProd)
    {
       mydata[0]++;

      backup->insereFinal(mydata);
  
      Serial.print(backup->getQuantidade());
      Serial.println(F(" Tamanho backup"));
      Serial.print(backup->getQuantidadeConfima());
      Serial.println(F(" Confirmados"));

      Serial.print(F(" Novo dado gerado"));
      Serial.println(mydata[0], HEX);

      if (buff->getQuantidade() < 5 && !flagThread)
     {
        buff->insereFinal(mydata);
        Serial.println(F(" Tamanho buff "));
        Serial.print(buff->getQuantidade());
        contEnvio++;
      }
    }
      delay(10000);
  }
}
