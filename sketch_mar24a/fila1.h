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
    int insereDado(double *);
    int excluiDado(void *);
    int atualizaPTR(void *);
    int atualizaPTRAnt(void *);
    double * getDado();
    void* getPtrProx();
    void* getPtrAnt();
    void SetPos(int pos);
    int getPos();

    private:
      double *ptrDado;
      void *ptrProxNode;
      void *ptrAntNode;
      int pos;

};


class fila
{
  public:
    fila();
    ~fila();
    void* criaFila();
    int insereFinal(double *ptrDado);
    int removeFila();
    int getQuantidade();
    int getPos();
    int getQuantidadeConfima();
    void setPTRconfirmado(int );
    void setPTRconfirmadoMod(int);

    double * getDadoPosConf(int pos);
    double * getDado();
    double * getDadoFinal();
    double * getDadoConf();
    double * getDadoConfBigEnd();
    double * getDadoPosConfBigEnd(int pos);
    int getStartPosConfBigEnd();
    void resetBigEnd();
    node* criaNode();

      private:
      int quantidadeNode;
      int posConfirmada;
      int posConfirmadaBigEnd;
      int posStartBigEndConf;
      node *ptrInicio;
      node *ptrFinal;
      node *ptrConfirmado;
      node *ptrConfirmadoBigEnd;
};




#endif // FILA_H_INCLUDED
