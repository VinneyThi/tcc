/* struct */


int flagConfV = 0; // apos falha do envio do buff e consegue enviar
int flagReenvio = 0; // falha no 11 priemiro
int contEnvio = 0;
int flagEnvioRapido = 0;
int auxAtraso = 0;
int flagFalhaBuff = 0; // falha do envio do buff


/* /struct */

// strTxRx_ACK  =  TXRX_ACK         LMIC
//cTxRxFlagLmic = LMIC.txrxFlags    LMIC
// flagThread MUTEX REF&



tcc2(cTxRxFlagLmic , strTxRx_ACK, flagThread)
{
    //   Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));   
    //   Serial.print(F("ContEnvio "));
    //   Serial.print(contEnvio);


      auxAtraso = backup->getQuantidade() - backup->getQuantidadeConfima();


    //   Serial.println(F("Flags"));
    //   Serial.print(F("FlagReenvio "));
    //   Serial.println(flagReenvio);
    //   Serial.print(F("FlagFalhaBuff "));
    //   Serial.println(flagFalhaBuff);
    //   Serial.print(F("FlagConfV "));
    //   Serial.println(flagConfV);
    //   Serial.print(F("FlagThread "));
    //   Serial.println(flagThread);
    //   Serial.print(F("flagEnvioRapido "));
    //   Serial.println(flagEnvioRapido);
    //   Serial.print(F("TAMANHO - POSCONF "));
    //   Serial.println(auxAtraso);

      if (cTxRxFlagLmic & strTxRx_ACK)
      { 
        //   Serial.println(F("Received ack"));

        int auxTamBuff = buff->getQuantidade();

        // Serial.print(buff->getQuantidade());
        // Serial.println(F(" Tamanho buff")); // 10 minutos crash aqui mostars as flags depois.
        

        
        
        if (flagReenvio && !flagConfV  )// add 25/11 tam - pos < 5
        {
        //   Serial.println(F("**1** "));

          if(buff->getQuantidade()> 0) // add 06/07
            for (int i = 0 ; i < auxTamBuff  ; i++) //p
              buff->removeFila();
        //   Serial.println(F("**2** "));

          flagReenvio = 0;
          flagFalhaBuff = 0;

          if(!flagEnvioRapido)
           {
            
             backup->setPTRconfirmado(5);
             flagThread = 0;
            // Serial.println(F("**SETCONF** "));
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send); //TRABALHAR COM REF PARA A FUNÇÃO OU PUXAR A .H

           }
           else
          {
            carregaBUFF(backup, buff);
            Serial.println(F("*$"));
            Serial.print(F(" Tamanho buff apos recarregar "));
            Serial.println(buff->getQuantidade());
            Serial.println(F("**565** "));
            backup->setPTRconfirmado(5);
            Serial.println(F("**SETCONF** "));
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
          }
          //Serial.println(F("**SETCONF** "));
          /*if (auxAtraso >= 5)//
            {
            carregaBUFF(backup,buff);
            flagEnvioRapido = 1;
            Serial.println(F("&&"));
            Serial.print(buff->getQuantidade());
            Serial.println(F(" Tamanho buff apos recarregar"));
            backup->setPTRconfirmado(5);
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
            }*/
          //os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);


        }
        else if (!flagReenvio && flagFalhaBuff && !flagConfV)
        {  
        // Serial.println(F("**3** "));
          flagFalhaBuff = 0;

          if (auxAtraso >= 5) // TROCAR PARA UM TYPEDEF 
          {
            // Serial.println(F("**4** "));
            carregaBUFF(backup, buff);
            flagEnvioRapido = 1;
            // Serial.println(F("*"));
            // Serial.print(F(" Tamanho buff apos recarregar "));
            // Serial.println(buff->getQuantidade());
            // Serial.println(F("**5** "));
            backup->setPTRconfirmado(5);
            // Serial.println(F("**SETCONF** "));
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);

          }

          else
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);

        }
        else if (flagConfV) // incluir um else if*
        {  
            // Serial.println(F("**6** "));
            // Serial.println(F("*************"));
            // Serial.println(F("FLAGCONFV"));
            // Serial.println(F("*************"));
            flagFalhaBuff = 0; // coloquei*
            flagConfV = 0;

          if (auxAtraso > 5 )
          { 
            // Serial.println(F("**7** "));
            // Serial.println(F("*************"));
            // Serial.println(F("Show"));
            // Serial.println(F("*************"));
            carregaBUFF(backup, buff);
            // Serial.println(F("$$$$$$$$$$$$$$$"));
            flagEnvioRapido = 1;
            backup->setPTRconfirmado(5);
            // Serial.println(F("**SETCONF** "));
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
          }
        }

        else if (auxAtraso >= 5)// mover para cima.                       
        {
        //   Serial.println(F("**8** "));
          carregaBUFF(backup, buff);
          flagEnvioRapido = 1;
        //   Serial.println(F("*"));
        //   Serial.print(F(" Tamanho buff apos recarregar "));
        //   Serial.println(buff->getQuantidade());

          backup->setPTRconfirmado(5);
          
        //   Serial.println(F("**9** "));
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        } 
        else
        {
        //   Serial.println("*****10***");
          flagConfV = 0;
          flagThread = 0;
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);

        }

      }
      
      //flagFalhaBuff =0;
      //flagReenvio =0;

//****************************************************************************                                      
      else if (!(LMIC.txrxFlags & TXRX_ACK))
      {
        // Serial.println(F("*************"));
        // Serial.println(F("Dont Received ack"));
        // Serial.print(F("Tamanho buff "));
        // Serial.println(buff->getQuantidade());
        
        if (flagReenvio) //!flagFalhaBuff
        {
            
        //   Serial.println(F("**11** "));
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);/// desvio ?
          //break;
        }
        else if (flagFalhaBuff && buff->getQuantidade() < 5)
        { 
        //   Serial.println(F("**12** "));
        //   Serial.print(F("Tamanho buff apos carga via backup "));
        //   Serial.println(buff->getQuantidade());
          flagConfV = 1 ; // yflagEnvioRapido
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
        }
        else if (flagConfV )
        {
            // Serial.println(F("**13** "));

          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
        }

        else if (auxAtraso >= 5 && flagEnvioRapido && buff->getQuantidade() == 0) //p
        {
            // Serial.println(F("**14** "));
          carregaBUFF(backup, buff);
          backup->setPTRconfirmado(5); // PROBLEMA AQUI 25/11
             // Serial.println(F("**SETCONF** "));
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }
        else if ( flagEnvioRapido && buff->getQuantidade() > 0) // REMOVIR auxAtraso > 0 &&
        {
            // Serial.println(F("**15** "));
          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_sendRenv);
        }
        else
        { 
            // Serial.println(F("**16** "));
          int auxB =  buff->getQuantidade();

          if ( auxB > 0 && flagThread)
            for (int i = 0 ; i < auxB  ; i++) //p
              buff->removeFila();

        //    Serial.println(F("**17** "));   
            // Serial.println(buff->getQuantidade());
            
          flagEnvioRapido = 0;
          flagThread = 0; /*******/


          os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(1), do_send);
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