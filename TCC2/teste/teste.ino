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
uint8_t mydata[1];
int flagFalhaBuff = 0; // falha do envio do buff

fila *buff = new PROGMEM fila;
fila *backup = new PROGMEM fila;

void carregaBUFF(fila *ptrbackup)
{
  Serial.println(F("**Carrega1** "));

  for (int i = 0; i < 5; i++)
  {
    uint8_t *ptrAuxDado = new uint8_t;

    *ptrAuxDado = ptrbackup->getDadoPosConf((i+1) );
    Serial.println(i);
    Serial.println(*ptrAuxDado, HEX);
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


void testeFila(fila *ptrbackup)
{
    for (int i = 0; i < 25; i++)
  {
    uint8_t *ptrAuxDado = new uint8_t;
    backup->insereFinal(mydata);    
    mydata[0]++;
    Serial.print("Valor inserido ");
    Serial.println(backup->getDadoFinal(), HEX);
  }

}


void simularConf(fila *ptrbackup, int count)
{
  for (int i = 0; i < count; i++)
    {
      backup->setPTRconfirmado(5);
      
      Serial.print("Valor confirmado ");
      Serial.println(backup->getDadoConf(), HEX);
      Serial.println("Valor confirmado ");
      
      carregaBUFF(ptrbackup);
    }
  
}


void setup()
{
  Serial.begin(115200);
  // xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0); //Cria a tarefa "loop2()" com prioridade 1, atribuída ao core 0
  Serial.print(sizeof(node));
  Serial.println(F("Starting"));


}

void loop()
{
  testeFila(backup);
  simularConf(backup, 3);

  delay(60000);
}
