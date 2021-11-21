#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "fila1.h"
#include <TinyGPS++.h>
#include <axp20x.h>


double latu, lon, alt;


TinyGPSPlus gps;
HardwareSerial GPS(1);
AXP20X_Class axp;
uint32_t LatitudeBinary, LongitudeBinary;
uint16_t altitudeGps, id = 0;
uint8_t hdopGps;


  double auxCoord[4]; 


# define AtiveInverse  1
# define EvitaEnvioVazio 2

int flagStartProd   = 0;
int contEnvio       = 0;
int flagThread      = 0;
int auxAtraso       = 0;
int flagReenvio     = 0;
int flagConfV       = 0;
int flagEnvioRapido = 0;
int OldSizeBackup   = 0;
int flagFalhaBuff   = 0;   // falha do envio do buff
int linkDead        = 0;

uint8_t mydata[11];
uint8_t lastDataSend[1];

fila *buff   = new PROGMEM fila;
fila *backup = new PROGMEM fila;

static osjob_t sendjob; // cria uma variavel de trabalho que é usado na função de agendamento da biblioteca.

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 10;

void do_send(osjob_t *j);
void do_sendRenv(osjob_t *j);
void tcc2();
void carregaBUFF(fila *ptrbackup, fila *ptrbuff);
void printSet(fila *ptrbackup);
void LoadBuffBigEnd(fila *ptrbackup, fila *ptrbuff);
void setPTRconfirmado(fila *ptrBackup);
void CatCoordGPS();

void setPTRconfirmado(fila *ptrBackup)
{
  if (AtiveInverse && flagEnvioRapido)
  {
    ptrBackup->setPTRconfirmadoMod(2);
    ptrBackup->setPTRconfirmado(3);
  }
  else
    ptrBackup->setPTRconfirmado(5);
}

void carregaBUFF(fila *ptrbackup, fila *ptrbuff)
{
  if (AtiveInverse && flagEnvioRapido)
    LoadBuffBigEnd(ptrbackup, ptrbuff);
  else
    LoadBuffLowEnd(ptrbackup, ptrbuff);
}

void LoadBuffLowEnd(fila *ptrbackup, fila *ptrbuff)
{
  Serial.println(F("**LoadBuffLowEnd** "));

  for (int i = 0; i < 5; i++)
  {
    double *ptrAuxDado;
    double *ptrAuxInsert = new double[4];

    ptrAuxDado = ptrbackup->getDadoPosConf( (i + 1) );
    
    for (int i =0 ; i < 4 ; i++)
   {Serial.print("teses lowendBuff ");
    Serial.println( i);
    ptrAuxInsert[i] = ptrAuxDado[i];
   }

    
    Serial.print("id: ");
    Serial.println(ptrAuxDado[3]);
    ptrbuff->insereFinal(ptrAuxInsert);
  }
  Serial.println(F("**LoadBuffLowEnd2** "));
}


void LoadBuffBigEnd(fila *ptrbackup, fila *ptrbuff)
{
  Serial.println(F("**LoadBuffBigEnd1** "));


  for (int i = 0; i < 2; i++)
  {
    double *ptrAuxDado;
    double *ptrAuxInsert = new double[4];
    
    ptrAuxDado = ptrbackup->getDadoPosConfBigEnd( (i) );

    for (int j =0 ; j < 4 ; j++)
      ptrAuxInsert[j] = ptrAuxDado[j];
      
    Serial.print("id: ");
    Serial.println(ptrAuxDado[3]);;
    ptrbuff->insereFinal(ptrAuxDado);
  }

  for (int i = 0; i < 3; i++)
  {
    double *ptrAuxDado;
    double *ptrAuxInsert = new double[4];

    ptrAuxDado = ptrbackup->getDadoPosConf( (i + 1) );
    
    for (int j =0 ; j < 4 ; j++)
     ptrAuxInsert[j] = ptrAuxDado[j];
    
    Serial.print("id: ");
    Serial.println(ptrAuxDado[3]);
    ptrbuff->insereFinal(ptrAuxInsert);
  }
  Serial.println(F("**LoadBuffBigEnd2** "));
}

void printSet(fila *ptrbackup)
{
  Serial.println(F("**//printSet** "));

  double *ptrAuxDado;
  ptrAuxDado = ptrbackup->getDadoPosConf(0);
  Serial.println(*ptrAuxDado, HEX);

  Serial.println(F("**//printSet** "));
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

    double *ptrAuxDate;
    ptrAuxDate = buff->getDado();



    
    Serial.print(F("Enviando o Buffer "));
    Serial.print(*ptrAuxDate, HEX);
    Serial.println(F(" dado"));
    uint8_t myaux[11];
  
   uint32_t LatitudeBinaryAux, LongitudeBinaryAux;
   uint16_t altitudeGpsAux, AuxId; 
   uint8_t hdopGpsAux;

  
  LatitudeBinaryAux = (( ptrAuxDate[0] + 90) / 180) * 16777215;
  LongitudeBinaryAux = (( ptrAuxDate[1] + 180) / 360) * 16777215;
  altitudeGpsAux=  ptrAuxDate[2];
  AuxId = (uint16_t)ptrAuxDate[3];
  hdopGpsAux = 2.5;
    
  myaux[0] = ( LatitudeBinaryAux >> 16 ) & 0xFF;
  myaux[1] = ( LatitudeBinaryAux >> 8 ) & 0xFF;
  myaux[2] = LatitudeBinaryAux & 0xFF;

  myaux[3] = ( LongitudeBinaryAux >> 16 ) & 0xFF;
  myaux[4] = ( LongitudeBinaryAux >> 8 ) & 0xFF;
  myaux[5] = LongitudeBinaryAux & 0xFF;

  altitudeGps = altitudeGpsAux;
  myaux[6] = ( altitudeGpsAux >> 8 ) & 0xFF;
  myaux[7] = altitudeGpsAux & 0xFF;

  myaux[8] = ( AuxId >> 8 ) & 0xFF;
  myaux[9] = AuxId & 0xFF;

  hdopGpsAux = hdopGpsAux * 10;
  myaux[10] = hdopGpsAux & 0xFF;


    if (buff->getQuantidade() == 1 && !flagConfV && !flagEnvioRapido) // !flagConfV !flagEnvioRapido
    {

      flagFalhaBuff = 1;
      flagReenvio = 0;

      LMIC.rxDelay = 5;
      LMIC_setTxData2(1, myaux, sizeof(myaux), 1);
    }
    else
    {
      LMIC.rxDelay = 1;
      LMIC_setTxData2(1, myaux, sizeof(myaux), 0);
    }

    buff->removeFila();
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

    if (contEnvio <= 4 && !flagFalhaBuff && !flagConfV)
    {
      Serial.println(F("!Envio padrão do Buffer!!"));
      Serial.print(F("Valor a ser enviado  "));
      Serial.println(mydata[0], HEX);

      Serial.print(F("Valor do ultimo dado enviado "));
      Serial.println(lastDataSend[0], HEX);

      contEnvio = (lastDataSend[0] != mydata[0])  && (contEnvio == 0 ) && (buff->getQuantidade() == 0) ? ++contEnvio : contEnvio ;

      LMIC.rxDelay = 1;
      LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    }

    else if (contEnvio > 4)
    {
      contEnvio = 0;
      flagReenvio = 1;
      flagThread = 1;

      Serial.println(F("!! Enviado solicitando confirmação  !!"));
      Serial.println(mydata[0], HEX);
      Serial.println(F("!!  Enviado solicitando confirmação !!"));
      lastDataSend[0] = mydata[0];

      LMIC.rxDelay = 5;
      LMIC_setTxData2(1, mydata, sizeof(mydata), 1);
    }

    else if (flagFalhaBuff)
    {
      flagFalhaBuff = 0; //y
      LMIC_setTxData2(1, mydata, sizeof(mydata), 1);
    }

    else if (flagConfV)
    {
      LMIC.rxDelay = 5;
      LMIC_setTxData2(1, mydata, sizeof(mydata), 1);
    }

    Serial.print(F("VALOR DO pacote enviado "));
    Serial.println(mydata[0], HEX);
    Serial.print(F("Valor Contador "));
    Serial.println(contEnvio);
    Serial.println(F("Packet queued"));

    Serial.print(F("Sending packet on frequency: "));
    Serial.println(LMIC.freq);

  }

  // Next TX is scheduled after TX_COMPLETE event.
}


void tcc2 () {
  LMIC_setAdrMode(0);
  Serial.println(LMIC.rxDelay);

  Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));

  auxAtraso =  AtiveInverse == 1 && flagEnvioRapido == 1 ?  backup->getQuantidade() - ( backup->getQuantidadeConfima() + 2*backup->getQuantidadeConfima()/3 ) :  backup->getQuantidade() - backup->getQuantidadeConfima();
  OldSizeBackup = backup->getQuantidade();
  Serial.println(F("********Flags********"));
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
  Serial.print(F("Posição de inicio da HeadBig "));
  Serial.println(backup->getStartPosConfBigEnd());

  if (LMIC.txrxFlags & TXRX_ACK)
  {
    Serial.println(F("Received ack"));
    int auxTamBuff = buff->getQuantidade();

    Serial.print(F("Tamanho Buffer "));
    Serial.println(buff->getQuantidade());
    if (flagReenvio && !flagConfV)
    {
      if (buff->getQuantidade() > 0) // add 06/07
        Serial.println(F("Antes do remove  ack"));

      for (int i = 0; i < auxTamBuff; i++) //p
      { Serial.println(i);
      Serial.println("remove");
        buff->removeFila();
      }

      Serial.println(F("Deposi do remove  ack"));

      flagReenvio   = 0;
      flagFalhaBuff = 0;

      setPTRconfirmado(backup);
      Serial.println(F("**setPTRconfirmado** "));
      flagThread = 0;

      Serial.println("Volta para envio normal");
      int schedulerTime = 0;
      schedulerTime = OldSizeBackup == backup->getQuantidade() ? (TX_INTERVAL / 4 ) + 2 : 1; //arruamr o cont a mais.
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

        setPTRconfirmado(backup);
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

      if (flagConfV > 1)
      {
        Serial.println(F("ANTE VENTO"));
        flagConfV--;
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
        return ;
      }
      else
      {
        Serial.println(F("VOLTA MEMO VENTO"));
        flagConfV = 0;
        LMIC.rxDelay = 1;
        flagFalhaBuff = 0; // coloquei*
      }


      if (auxAtraso > 5)
      {
        Serial.println(F("*************"));
        Serial.println(F("Atrasso > 5"));
        Serial.println(F("*************"));
        carregaBUFF(backup, buff);

        flagEnvioRapido = 1;
        setPTRconfirmado(backup);
        Serial.println(F("**setPTRconfirmado** "));
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
      }
    }
    else if (auxAtraso >= 5)
    {
      Serial.println(F("Atrasso > 5"));
      carregaBUFF(backup, buff);
      flagEnvioRapido = 1;
      LMIC.rxDelay = 1;
      Serial.println(F("*"));
      Serial.print(F(" Tamanho buff apos recarregar "));
      Serial.println(buff->getQuantidade());

      setPTRconfirmado(backup);
      Serial.println(F("**setPTRconfirmado** "));

      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
    }
    else
    {
      flagConfV = 0;
      flagThread = 0;
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
    }
  }


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
      flagConfV = EvitaEnvioVazio; // yflagEnvioRapido
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
      setPTRconfirmado(backup);
      //printSet(backup);
      Serial.println(F("**SETCONF** "));
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
    }
    else if (auxAtraso > 0  && flagEnvioRapido && buff->getQuantidade() > 0)
    {
      LMIC.rxDelay = 0;
      Serial.print(F("Envio rapido inutil"));
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
    }
    else
    {
      int auxB = buff->getQuantidade();

      if (auxB > 0 && flagThread)
        for (int i = 0; i < auxB; i++) //p
          buff->removeFila();

      Serial.print("Tamanho do buff ");
      Serial.println(buff->getQuantidade());
      flagEnvioRapido = 0;
      flagThread = 0; /*******/
      Serial.println("envio normal Default");
      int schedulerTime = 0;
      schedulerTime = OldSizeBackup == backup->getQuantidade() ? TX_INTERVAL / 4 : 1;
      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(schedulerTime), do_send);
    }
  }

  Serial.println("*****Delay***");
  Serial.println(LMIC.rxDelay);
  Serial.println("*****Delay***");
  LMIC.rxDelay = 5;
}



void CatCoordGPS()
{
  if (gps.location.isUpdated())
  {
    latu = gps.location.lat();
    lon = gps.location.lng();
  }
//  if (gps.altitude.isUpdated())
  
    LatitudeBinary = ((gps.location.lat() + 90) / 180) * 16777215;
  LongitudeBinary = ((gps.location.lng () + 180) / 360) * 16777215;

  id++;
  uint16_t AuxInt = id;

  
  double TESTE3[3];
  auxCoord[0]= gps.location.lat();
   auxCoord[1] = gps.location.lng();
   auxCoord[2] = gps.altitude.meters();
   auxCoord[3] = id;

Serial.println("tEST AUX COORD");
  Serial.println(TESTE3[0] ,5);
  Serial.println(TESTE3[1] ,5);
  Serial.println(TESTE3[2] ,5);
  Serial.println("tEST AUX COORD");
  
  mydata[0] = ( LatitudeBinary >> 16 ) & 0xFF;
  mydata[1] = ( LatitudeBinary >> 8 ) & 0xFF;
  mydata[2] = LatitudeBinary & 0xFF;

  mydata[3] = ( LongitudeBinary >> 16 ) & 0xFF;
  mydata[4] = ( LongitudeBinary >> 8 ) & 0xFF;
  mydata[5] = LongitudeBinary & 0xFF;

  altitudeGps = gps.altitude.meters();
  mydata[6] = ( altitudeGps >> 8 ) & 0xFF;
  mydata[7] = altitudeGps & 0xFF;

  mydata[8] = ( AuxInt >> 8 ) & 0xFF;
  mydata[9] = AuxInt & 0xFF;

  hdopGps = gps.hdop.hdop() * 10;
  mydata[10] = AuxInt & 0xFF;

  Serial.println(gps.location.lat(), 5);
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Altitude  : ");
  Serial.println(gps.altitude.feet() / 3.2808);

  int  idteste = ((mydata[8] << 8) ) + mydata[9];
  Serial.print("ID**&  : ");
  Serial.println(id);
  Serial.print(F("Valor id decode "));
  Serial.println(idteste);

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
      LMIC.rxDelay = 5;
      break;
    case EV_JOINED:
      LMIC.rxDelay = 5;
      Serial.println(F("EV_JOINED"));
      {
        if (AtiveInverse)
          Serial.println(F("Ativado modo centopeia"));
        else
          Serial.println(F("Legacy"));

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
      if (linkDead) {
        Serial.println(" Volta do link dead");
        linkDead = 0;
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
      }
      break;

    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
      break;
    case EV_TXCOMPLETE:
      tcc2();
      LMIC.rxDelay = 5;

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
      LMIC.rxDelay = 5;
      linkDead = 1;
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

void setup()
{
  Serial.begin(115200);
  xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0); //Cria a tarefa "loop2()" com prioridade 1, atribuída ao core 0
  Serial.print(sizeof(node));
  Serial.println(F("Starting"));
  mydata[0] = 0x00;

  Wire.begin(21, 22); // configurado a comunicação com o axp
  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
    Serial.println("AXP192 Begin PASS");
  } else {
    Serial.println("AXP192 Begin FAIL");
  }
  axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
  axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
  GPS.begin(9600, SERIAL_8N1, 34, 12);  // configurando comunicação com o NEO6.
  Serial.println(F("Starting"));
  
    double *ptrAuxInsert = new double[4];
    
    for (int i =0 ; i < 4 ; i++)
      ptrAuxInsert[i] = 0.0;
 

  backup->insereFinal(ptrAuxInsert);
  buff->insereFinal(ptrAuxInsert);
  contEnvio++;
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC_setDrTxpow(DR_SF9, 14);
  LMIC_selectSubBand(1);                         //ativa um conjunto especifico de canais
  LMIC_setClockError(MAX_CLOCK_ERROR * 0 / 100); // compensação de atrasso no recebimento de janela de donwlink( necessario no otta) configurado para  um erro de 1%

  // Start job (sending automatically starts OTAA too)
  Serial.print(F("testesd123 "));
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
    double *ptrAuxDate;

    Serial.println("!!!!!!!!!!!");
    Serial.print("Memo " );
    Serial.println(heap_caps_get_free_size(MALLOC_CAP_8BIT));
    Serial.println("!!!!!!!!!!!");
    if ( flagStartProd)
    {
      //  mydata[0]++;

      while (GPS.available())
        gps.encode(GPS.read());

      CatCoordGPS();
      
      double *ptrAuxInsert = new double[4];
      
      for (int i =0 ; i < 4 ; i++)
        ptrAuxInsert[i] = auxCoord[i];
        
      backup->insereFinal(ptrAuxInsert);
      ptrAuxDate = backup->getDado();
    
    Serial.print(F(" Novo dado TESTE AUX LAT  " ));
    Serial.println(ptrAuxDate[0] ,5);

        Serial.println(auxCoord[0] ,5);

            
    Serial.print(F(" Novo dado TESTE AUX LAT  " ));

      Serial.print(backup->getQuantidade());
      Serial.println(F(" Tamanho backup"));
      Serial.print(backup->getQuantidadeConfima());
      Serial.println(F(" Confirmados"));

      Serial.print(F(" Novo dado gerado"));
      Serial.println(mydata[0]);

      if (buff->getQuantidade() <= 4 && !flagThread)
      {
        double *ptrAuxInsert = new double[4];
        for (int i =0 ; i < 4 ; i++)
          ptrAuxInsert[i] = auxCoord[i];
          
        buff->insereFinal(ptrAuxInsert);
        Serial.println(F(" Tamanho buff "));
        Serial.print(buff->getQuantidade());
        contEnvio++;
      }
    }
    delay(10000);

  }
}
