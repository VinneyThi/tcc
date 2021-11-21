#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "fila1.h"

double id = 0;
# define AtiveInverse  0

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

fila *buff = new PROGMEM fila;
fila *backup = new PROGMEM fila;


void setPTRconfirmado(fila *ptrBackup)
{
  if(AtiveInverse && flagEnvioRapido)
   {
     ptrBackup->setPTRconfirmadoMod(2);
     ptrBackup->setPTRconfirmado(3);
   }
   else
      ptrBackup->setPTRconfirmado(5);


}

void carregaBUFF(fila *ptrbackup)
{
  if (AtiveInverse && flagEnvioRapido)
    LoadBuffBigEnd(ptrbackup);
  else
    LoadBuffLowEnd(ptrbackup);
}
void LoadBuffLowEnd(fila *ptrbackup)
{
  Serial.println(F("**LoadBuffLowEnd** "));

  for (int i = 0; i < 5; i++)
  {
    double *ptrAuxDado;
    double *ptrAuxInsert = new double[4];

    ptrAuxDado = ptrbackup->getDadoPosConf( (i + 1) );
    
    for (int i =0 ; i < 4 ; i++)
   {
    ptrAuxInsert[i] = ptrAuxDado[i];
   }

    
    Serial.print("id: ");
    Serial.println(ptrAuxDado[3]);
  }
  Serial.println(F("**LoadBuffLowEnd2** "));
}


void LoadBuffBigEnd(fila *ptrbackup)
{
  Serial.println(F("**LoadBuffBigEnd1** "));


  for (int i = 0; i < 2; i++)
  {
    double *ptrAuxDado;
    double *ptrAuxInsert = new double[4];
    
    ptrAuxDado = ptrbackup->getDadoPosConfBigEnd( (i) );

    for (int j =0 ; j < 3 ; j++)
      ptrAuxInsert[j] = ptrAuxDado[j];
      
    Serial.print("id: ");
    Serial.println(ptrAuxDado[3]);;
      
  }

  for (int i = 0; i < 3; i++)
  {
    double *ptrAuxDado;
    double *ptrAuxInsert = new double[4];
    
    Serial.println("Entou la meu ");
    ptrAuxDado = ptrbackup->getDadoPosConf( (i + 1) );


    Serial.println("Saiu mais la ");  
    if( !ptrAuxDado)
      Serial.println("NULLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");  
      
    for (int j =0 ; j < 4 ; j++)
     ptrAuxInsert[j] = ptrAuxDado[j];
    
    Serial.print("id: ");
    Serial.println(ptrAuxDado[3]);
    
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


void testeFila(fila *ptrbackup)
{
    for (int i = 0; i < 25; i++)
  {
     double *ptrAuxInsert = new double[4];
      double * ptrAux;
     id++;
    ptrAuxInsert[0] = 5.5;
    ptrAuxInsert[1] = 5.5;
    ptrAuxInsert[2] = 5.5;
    ptrAuxInsert[3] = id;
    backup->insereFinal(ptrAuxInsert);    
ptrAux = backup->getDadoFinal();
    Serial.print("Valor inserido ");
    Serial.println(ptrAux[3]);
  }

}




void insere10(fila *ptrbackup)
{
     

    for (int i = 0; i < 2; i++)
  {
     double *ptrAuxInsert = new double[4];
     double * ptrAux;
    Serial.print("teses insere ");
    id++;
    ptrAuxInsert[0] = 5.5;
    ptrAuxInsert[1] = 5.5;
    ptrAuxInsert[2] = 5.5;
    ptrAuxInsert[3] = id;
   
    
    backup->insereFinal(ptrAuxInsert);    
    ptrAux = backup->getDadoFinal();
    Serial.print("Valor inserido ");
    Serial.println(ptrAux[3]);
  }

}


void simularConf(fila *ptrbackup, int count)
{
  for (int i = 0; i < count; i++)
    {
      double *ptrAux;
      setPTRconfirmado(ptrbackup);
      flagEnvioRapido = 1;
       ptrAux = backup->getDadoConf();
      Serial.print("Valor confirmado Leagacy ");
      Serial.println(ptrAux[3]);

      Serial.print("Valor confirmado  Centopeia ");
      ptrAux =  backup->getDadoConfBigEnd();
      Serial.println(ptrAux[3]);
      Serial.print("QTN ");
      Serial.println(backup->getQuantidade());
      carregaBUFF(ptrbackup);
    }
  
}


void setup()
{
  Serial.begin(115200);
  // xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0); //Cria a tarefa "loop2()" com prioridade 1, atribuída ao core 0
  Serial.print(sizeof(node));
  Serial.println(F("Starting"));
  testeFila(backup);

}

void loop()
{
  
  simularConf(backup, 1);
  delay(10000);
  insere10(backup);
}
