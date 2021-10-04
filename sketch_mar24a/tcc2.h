#ifndef TCC2_H_INCLUDED
#define TCC2_H_INCLUDED

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>


  
  int flagThread = 0;
  int flagEnvioRapido = 0;
  int flagFalhaBuff   = 0; // falha do envio do buff
  int flagReenvio     = 0; // falha no 11 priemiro
  int flagConfV       = 0; // apos falha do envio do buff e consegue enviar
  int contEnvio       = 0;
  int auxAtraso       = 0;


void carregaBUFFext();


void do_sendRenv(osjob_t* j) ;

void do_send(osjob_t* j);


void tcc2(int * pflagThread, osjob_t *sendjob);






#endif // TCC2_H_INCLUDED
