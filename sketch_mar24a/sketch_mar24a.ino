
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "fila1.h" //s
#include "tcc2.h"


uint8_t mydata[1];

fila  *buff = new PROGMEM fila;
fila  *backup = new PROGMEM fila;



    int BufferSetData_Aux(uint8_t *aux){
        return buff->insereFinal(aux);
        }

int BuffetGetSize_Aux(uint8_t *aux){
  return buff->getQuantidade();
}



uint8_t BackupGetConf(int pos){
  return backup->getDadoPosConf(pos);
}



uint8_t BufferGetdData_Aux(){
  return buff->getDado();
}


int BufferGetSizeConf(){
  return buff->getQuantidadeConfima();
}

int RemoveBuff(){
  return buff->removeFila();
}

uint8_t BackupGetData(){
  return backup->getDado();
}

uint8_t BackupSetData(uint8_t *aux){
  return backup->insereFinal(aux);
}


void BackupSetConf(int aux){
  return backup->setPTRconfirmado(aux);
}


int BackupGetSize(){
  return backup->getQuantidade();
}



pBufferSetdData = &BufferSetData_Aux;
pBuffetGetSize =  &BuffetGetSize_Aux;
pBufferGetdData = &BufferGetdData_Aux;
pBufferGetSizeConf = &BufferGetSizeConf;
pRemoveBuff = &RemoveBuff;

pBackupGetSize = &BackupGetSize;
pBackupGetConf = &BackupGetConf;
pBackupGetData = &BackupGetData;
pBackupSetData = &BackupSetData;
pBackupSetConf = &BackupSetConf;


static const u1_t PROGMEM APPEUI[8] ={ 0x4C, 0x84, 0x03, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

static const u1_t PROGMEM DEVEUI[8] = { 0x04, 0x7C, 0x89, 0x32, 0x09, 0xA2, 0xB9, 0x00 };
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}


static const u1_t PROGMEM APPKEY[16] ={ 0xF4, 0xBD, 0x6C, 0x66, 0x65, 0xC2, 0x7B, 0x92, 0xA5, 0x7A, 0x39, 0x4D, 0x28, 0x2B, 0x38, 0x1C };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}


static osjob_t sendjob; // cria uma variavel de trabalho que é usado na função de agendamento da biblioteca.

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 1;

// Pin mapping for Esp32
const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = {26, 33, 32}, // 26,35,34 | 26,33,32
};

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev) {
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      {
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
        for (int i = 0; i < sizeof(artKey); ++i) {
          Serial.print(artKey[i], HEX);
        }
        Serial.println("");
        Serial.print("nwkKey: ");
        for (int i = 0; i < sizeof(nwkKey); ++i) {
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




      tcc2(&flagThread, &sendjob);








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
      Serial.println((unsigned) ev);
      break;
  }
}

void do_sendRenv(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
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


void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    Serial.println(F("**do_send** "));


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



    Serial.println(F("VALOR DO pacote enviado"));
    Serial.println(mydata[0], HEX);
    Serial.print(contEnvio);
    Serial.println(F(" Valor Contador"));
    Serial.println(F("Packet queued"));

    Serial.print(F("Sending packet on frequency: "));
    Serial.println(LMIC.freq);

  }
  // Next TX is scheduled after TX_COMPLETE event.
}



void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0); //Cria a tarefa "loop2()" com prioridade 1, atribuída ao core 0
  Serial.print(sizeof(node));
  Serial.println(F("Starting"));
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
  Serial.printf("\n**loop2() em core: %d /n", xPortGetCoreID());//Mostra no monitor em qual core o loop2() foi chamado
  while (1)
  {
    mydata[0]++;

    backup->insereFinal(mydata);

    Serial.print(backup->getQuantidade());
    Serial.println(F(" Tamanho backup"));
    Serial.print(backup->getQuantidadeConfima());
    Serial.println(F(" Confirmados"));

    if (buff->getQuantidade() < 5 && !flagThread )
    {
      buff->insereFinal(mydata);
      Serial.println(F(" Tamanho buff"));
     Serial.print(buff->getQuantidadeConfima());
      contEnvio++;
    }
    delay(10000);
  }
}
