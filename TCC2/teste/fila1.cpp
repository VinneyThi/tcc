#include "fila1.h"
#include <stdlib.h>

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

node::node()
{
  this->ptrDado = NULL;
  this->ptrProxNode = NULL;
  this->ptrAntNode = NULL;
  this->pos = 0;
}

node::~node()
{
  free(this->ptrDado);

}

void node:: SetPos(int pos)
{
  this->pos = pos;
}

int node:: getPos()
{
  return this->pos;
}

uint8_t node::getDado()
{
  uint8_t  * ptrAux = new  uint8_t;
  *ptrAux = *this->ptrDado;
  return *ptrAux;
}
void* node::getPtrProx()
{
  return this->ptrProxNode;
}
void* node::getPtrAnt()
{
  return this->ptrAntNode;
}
int node::insereDado(uint8_t *ptrDado)
{
  if (!ptrDado)
    return -1;
  uint8_t  * ptrAux = new  uint8_t;
  *ptrAux = *ptrDado;
  this->ptrDado = ptrAux;
  return 1;
}
int node::atualizaPTR(void *ptrProx)
{
  if (!ptrProx)
    return -1;
  this->ptrProxNode = ptrProx;

  return 1;
}

int node::atualizaPTRAnt(void *ptrAnt)
{

  if (!ptrAnt)
    return -1;

  this->ptrAntNode = ptrAnt;
  return 1;
}

fila::fila()
{
  this->quantidadeNode = 0;
  this->posConfirmada = 0;
  this->posStartBigEndConf =0;
  this->posConfirmadaBigEnd =0;
  this->ptrFinal = NULL;
  this->ptrInicio = NULL;
  this->ptrConfirmado = NULL;
  this->ptrConfirmadoBigEnd = NULL;

}
fila::~fila()
{
  free(ptrInicio);
  free(ptrFinal);
  free(ptrConfirmado);
  free(ptrConfirmadoBigEnd);
}
node* fila::criaNode()
{
  node *ptrAux = (node *)   malloc(sizeof(node));
  if (!ptrAux)
    return NULL;
  return ptrAux;
}

uint8_t fila::getDado()
{ if (!this->quantidadeNode)
    return NULL;
  return this->ptrInicio->getDado();
}

uint8_t fila::getDadoFinal()
{ if (!this->quantidadeNode)
    return NULL;
  return this->ptrFinal->getDado();
}

int fila::getQuantidade()
{
  int aux;
  aux = this->quantidadeNode;
  return aux;
}
int fila::insereFinal(uint8_t *ptrDado)
{
  node *ptrAuxNode = criaNode();

  if (!ptrDado || !ptrAuxNode)
    return -1;

  ptrAuxNode->insereDado(ptrDado);
  ptrAuxNode->SetPos(this->quantidadeNode);

  if (this->quantidadeNode == 0)
  {
    
    this->ptrInicio = ptrAuxNode;
    this->ptrFinal = ptrAuxNode;
    this->quantidadeNode++;
  }
  else
  {
    this->ptrFinal->atualizaPTR(ptrAuxNode);
    ptrAuxNode->atualizaPTRAnt(this->ptrFinal);
    this->ptrFinal = ptrAuxNode;
    this->quantidadeNode++;

  }

   
    this->ptrConfirmadoBigEnd = this->ptrFinal;
    
  return 1;
}
int fila::removeFila()
{
  if (!this->quantidadeNode)
    return -1;
  if (this->quantidadeNode == 1)
  {
    delete this->ptrFinal;
    this->ptrFinal = NULL;
    this->ptrInicio = NULL;
    this->ptrConfirmado = NULL;
    this->quantidadeNode = 0;
    return 1;
  }
  node *ptrAux;
  ptrAux = (node *)this->ptrInicio->getPtrProx();
  delete this->ptrInicio;
  ptrAux->atualizaPTR(NULL);
  this->ptrInicio = ptrAux;
  ptrAux = NULL;
  this->quantidadeNode --;

  return 1;
}

void fila::setPTRconfirmado(int qtnConfirmado)
{

 
  if (!this->ptrConfirmado)
  { node *ptrAux = this->ptrInicio;
    int contAux = 0;
    while (contAux < qtnConfirmado -1 )
    { if (!ptrAux)
        return ;
      ptrAux = (node *)ptrAux->getPtrProx();
      contAux++;
    }
    this-> ptrConfirmado = ptrAux;
    this->posConfirmada = qtnConfirmado -1;
  }
  else
  {
    node *ptrAux = this->ptrConfirmado;
    int contAux = 0;
    int sizeAux = 0;
    
    if(ptrAux->getPos() == this->ptrConfirmadoBigEnd->getPos())
    {
      this->ptrConfirmado = this->ptrConfirmadoBigEnd;
      this->posConfirmada = this->posConfirmada + this->posConfirmadaBigEnd;
      return ;

    }

    sizeAux = qtnConfirmado + this->posConfirmada <= this->getQuantidade() ? qtnConfirmado :  qtnConfirmado -1;
    while (contAux < sizeAux )
    {
      if (!ptrAux)
        return ;
      ptrAux = (node *) ptrAux->getPtrProx();
      contAux++;
    }
    this-> ptrConfirmado = ptrAux;
    this->posConfirmada =  this->posConfirmada + qtnConfirmado;
  }
}

void fila::setPTRconfirmadoMod(int qtnConfirmado)
{
  if(! this->posConfirmadaBigEnd)
    posStartBigEndConf = this->ptrConfirmadoBigEnd->getPos();

  this->posConfirmadaBigEnd = this->posConfirmadaBigEnd + qtnConfirmado;  
}

uint8_t fila:: getDadoConf()
{
  if (!this->quantidadeNode)
    return NULL;
  return this->ptrConfirmado->getDado();
}
int fila::getQuantidadeConfima()
{
  int aux;
  aux = this->posConfirmada + 1;
  return aux;
}

uint8_t fila:: getDadoConfBigEnd()
{
  if (!this->quantidadeNode)
    return NULL;
  return this->ptrConfirmadoBigEnd->getDado();
}

uint8_t fila::getDadoPosConf(int pos)
{
  node *ptrAux = this->ptrConfirmado;
  int contAux = 0;
  while (contAux < pos)
  {
    if(!ptrAux)
      return 0x00;
      
    ptrAux = (node *)ptrAux->getPtrProx();
    contAux++;
  }
  return ptrAux->getDado();

}

uint8_t fila::getDadoPosConfBigEnd(int pos)
{
  node *ptrAux = this->ptrConfirmadoBigEnd;
  int contAux = 0;
  while (contAux < pos)
  {
    if(!ptrAux)
      return 0x00;
      
    ptrAux = (node *)ptrAux->getPtrAnt();
    contAux++;
  }
  return ptrAux->getDado();


}
