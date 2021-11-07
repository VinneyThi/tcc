#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "fila1.h"


# define AtiveInverse  1

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

uint8_t mydata[1];
uint8_t lastDataSend[1];

fila *buff = new PROGMEM fila;
fila *backup = new PROGMEM fila;


void setPTRconfirmado(fila *ptrBackup)
{
  if(AtiveInverse)
   {
     ptrBackup->setPTRconfirmadoMod(2);
     ptrBackup->setPTRconfirmado(3);
   }
   else
      ptrBackup->setPTRconfirmado(5);


}

void carregaBUFF(fila *ptrbackup)
{
  if(AtiveInverse)
    LoadBuffBigEnd(ptrbackup);
  else
    LoadBuffLowEnd(ptrbackup);
}

void LoadBuffLowEnd(fila *ptrbackup)
{
  Serial.println(F("**LoadBuffLowEnd** "));

  for (int i = 0; i < 5; i++)
  {
    uint8_t *ptrAuxDado = new uint8_t;
    *ptrAuxDado = ptrbackup->getDadoPosConf( (i+1) );
    Serial.println(i);
    Serial.println(*ptrAuxDado, HEX);
    
  }
  Serial.println(F("**LoadBuffLowEnd2** "));
}


void LoadBuffBigEnd(fila *ptrbackup)
{
  Serial.println(F("**LoadBuffBigEnd1** "));


  for (int i = 0; i < 2; i++)
  {
    uint8_t *ptrAuxDado = new uint8_t;

    *ptrAuxDado = ptrbackup->getDadoPosConfBigEnd( (i+1) );
    Serial.println(i);
    Serial.println(*ptrAuxDado, HEX);
    
  }

  for (int i = 0; i < 3; i++)
  {
    uint8_t *ptrAuxDado = new uint8_t;

    *ptrAuxDado = ptrbackup->getDadoPosConf( (i+1) );
    Serial.println(i);
    Serial.println(*ptrAuxDado, HEX);
    
  }
  Serial.println(F("**LoadBuffBigEnd2** "));
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
      setPTRconfirmado(5);
      
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
