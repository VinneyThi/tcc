#ifndef FILA_H_INCLUDED
#define FILA_H_INCLUDED

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <stdlib.h>
class node
{
  public:
    node();
    ~node();
    int insereDado(uint8_t *);
    int excluiDado(void *);
    int atualizaPTR(void *);
    int atualizaPTRAnt(void *);
    uint8_t getDado();
    void* getPtrProx();

    private:
      uint8_t *ptrDado;
      void *ptrProxNode;
       void *ptrAntNode;

};


class fila
{
  public:
    fila();
    ~fila();
    void* criaFila();
    int insereFinal(uint8_t *ptrDado);
    int removeFila();
    int getQuantidade();
    int getQuantidadeConfima();
    void setPTRconfirmado(int );
  uint8_t getDadoPosConf(int pos);
     uint8_t getDado();
     uint8_t getDadoFinal();
      uint8_t getDadoConf();
    node* criaNode();


      private:
      int quantidadeNode;
      int posConfirmada;
      node *ptrInicio;
      node *ptrFinal;
      node *ptrConfirmado;
};




#endif // FILA_H_INCLUDED
