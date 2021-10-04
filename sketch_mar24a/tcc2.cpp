  #include "tcc2.h"
  #define sizeBuff 5

void do_sendRenv(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    
  } else {
        

    uint8_t *ptrAuxDate = new uint8_t;
    *ptrAuxDate = (*pBufferGetdData)();
    
    
    
    uint8_t myaux[1];
    myaux[0] = *ptrAuxDate;

    if ((*pBuffetGetSize)() == 1 && !flagConfV && !flagEnvioRapido) // !flagConfV !flagEnvioRapido
    {

      flagFalhaBuff = 1;
      flagReenvio = 0;
      LMIC_setTxData2(1, myaux, sizeof(myaux), 1);

    }
    else
    {
      LMIC_setTxData2(1, myaux, sizeof(myaux), 0);
    }

    pRemoveBuff();
    
    
  }

  
  
  

}
// Next TX is scheduled after TX_COMPLETE event.


void do_send(osjob_t* j, uint8_t *mydata) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    
  } else {
    


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



    
    
    
    
    

    
    

  }
  // Next TX is scheduled after TX_COMPLETE event.
}
//////////////////////////////////////////////////////////////////////////////////////
/****************************************************************************************************************/
/* struct */


void carregaBUFFext(){
  
  for (int i = 0; i < 5 ; i++)
  { 
    
    uint8_t* ptrAuxDado = new uint8_t;

 // *ptrAuxDado = (*pBackupGetConf)(i); 
   //(* pBufferSetdData)(ptrAuxDado);
   
  }
  
}


/* /struct */

// strTxRx_ACK  =  TXRX_ACK         LMIC
//cTxRxFlagLmic = LMIC.txrxFlags    LMIC
// flagThread MUTEX REF&

void tcc2(int * pTxRxFlagLmic , char *strTxRx_ACK, int * pflagThread,osjob_t *sendjob)
{
    //   
    //   
    //   


      auxAtraso = (*pBackupGetSize)() - (*pBufferGetSizeConf)();


    //   
    //   
    //   
    //   
    //   
    //   
    //   
    //   
    //   
    //   
    //   
    //   
    //   

      if (*pTxRxFlagLmic & TXRX_ACK)
      { 
        //   

        int auxTamBuff = pBuffetGetSize();

        // 
        // 
        

        
        
        if (flagReenvio && !flagConfV  )// add 25/11 tam - pos < 5
        {
        //   

          if(pBuffetGetSize()> 0) // add 06/07
            for (int i = 0 ; i < auxTamBuff  ; i++) //p
              pRemoveBuff();
        //   

          flagReenvio = 0;
          flagFalhaBuff = 0;

          if(!flagEnvioRapido)
           {
            
             pBackupSetConf(sizeBuff);
             flagThread = 0;
            // 
            os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_send); //TRABALHAR COM REF PARA A FUNÇÃO OU PUXAR A .H

           }
           else
          {
            carregaBUFFext();
            
            
            
            
            pBackupSetConf(sizeBuff);
            
            os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
          }
          //
          /*if (auxAtraso >= 5)//
            {
            carregaBUFFext(backup,buff);
            flagEnvioRapido = 1;
            
            
            
            pBackupSetConf(5);
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
            }*/
          //os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);


        }
        else if (!flagReenvio && flagFalhaBuff && !flagConfV)
        {  
        // 
          flagFalhaBuff = 0;

          if (auxAtraso >= sizeBuff) // TROCAR PARA UM TYPEDEF 
          {
            // 
            carregaBUFFext();
            flagEnvioRapido = 1;
            // 
            // 
            // 
            // 
            pBackupSetConf(sizeBuff);
            // 
            os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_sendRenv);

          }

          else
            os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_send);

        }
        else if (flagConfV) // incluir um else if*
        {  
            // 
            // 
            // 
            // 
            flagFalhaBuff = 0; // coloquei*
            flagConfV = 0;

          if (auxAtraso > sizeBuff )
          { 
            // 
            // 
            // 
            // 
            carregaBUFFext();
            // 
            flagEnvioRapido = 1;
            pBackupSetConf(sizeBuff);
            // 
            os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
          }
        }

        else if (auxAtraso >= sizeBuff)// mover para cima.                       
        {
        //   
          carregaBUFFext();
          flagEnvioRapido = 1;
        //   
        //   
        //   

          pBackupSetConf(sizeBuff);
          
        //   
          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        } 
        else
        {
        //   
          flagConfV = 0;
          flagThread = 0;
          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_send);

        }

      }
      
      //flagFalhaBuff =0;
      //flagReenvio =0;

//****************************************************************************                                      
      else if (!(LMIC.txrxFlags & TXRX_ACK))
      {
        // 
        // 
        // 
        // 
        
        if (flagReenvio) //!flagFalhaBuff
        {
            
        //   
          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_sendRenv);/// desvio ?
          //break;
        }
        else if (flagFalhaBuff && pBuffetGetSize() < 5)
        { 
        //   
        //   
        //   
          flagConfV = 1 ; // yflagEnvioRapido
          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_send);
        }
        else if (flagConfV )
        {
            // 

          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_send);
        }

        else if (auxAtraso >= 5 && flagEnvioRapido && pBuffetGetSize() == 0) //p
        {
            // 
          carregaBUFFext();
          (*pBackupSetConf)(5); // PROBLEMA AQUI 25/11
             // 
          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }
        else if ( flagEnvioRapido && pBuffetGetSize() > 0) // REMOVIR auxAtraso > 0 &&
        {
            // 
          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }
        else
        { 
            // 
          int auxB =  pBuffetGetSize();

          if ( auxB > 0 && flagThread)
            for (int i = 0 ; i < auxB  ; i++) //p
              pRemoveBuff();

        //    
            // 
            
          flagEnvioRapido = 0;
          flagThread = 0; /*******/


          os_setTimedCallback(sendjob, os_getTime() + sec2osticks(1), do_send);
        }
      }

      //      // Schedule next transmission
      //            if( flagReenvio && !flagConfV)
      //            {
      //              flagReenvio =0;
      //              os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      //            }
      //           else if (flagReenvio && flagFalhaBuff)
      //                  os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);


      }
