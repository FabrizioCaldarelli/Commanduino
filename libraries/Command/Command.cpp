//
//  Commander.cpp
//  
//
//  Created by Fabrizio on 10/12/14.
//
//

#include "Command.h"

Command::Command()
{

}

Command::~Command()
{
    
}

void Command::setCommander(Commander *cmdIn)
{
    this->_commander = cmdIn;
}

bool Command::checkBufferMacaddressWithLocal(char *buffer, char *tempBuff, char *localMacaddressBuff)
{
    byte *btMac = NULL;
    
    bool retVal = false;
    
    if(this->_commander!=NULL)
    {
        btMac = this->_commander->getMacAddress();
        if(btMac != NULL)
        {
            
            this->getBufferAtIndex(buffer, tempBuff, IDX_BUFF_MACADDRESS);
            
            if(strlen(tempBuff) == 12)
            {
                //char strMac[12];
                for(byte k=0;k<6;k++)
                {
                    sprintf(localMacaddressBuff+(k*2), "%02X", btMac[k]);
                }
                if(strcmp(localMacaddressBuff, tempBuff) == 0)
                {
                    retVal = true;
                }
            }
            
        }
    }
    
    return retVal;
}

bool Command::isBufferMacaddressGeneric(char *buffer, char *tempBuff)
{
    bool retVal = false;
    
    if(this->_commander!=NULL)
    {
        this->getBufferAtIndex(buffer, tempBuff, IDX_BUFF_MACADDRESS);
        
        if(strlen(tempBuff) == 12)
        {
            if(strcmp("000000000000", tempBuff) == 0)
            {
                retVal = true;
            }
        }
    }
    
    return retVal;
}


void Command::getBufferAtIndex(char *buffer, char *outBuffer, int index)
{
    int k = 0;
    char *p = buffer;
    char *pLast = buffer;
    bool trovato = false;
    
    outBuffer[0] = '\0';
    
    while((p!=NULL)&&(trovato == false)&&(strlen(p)>0))
    {
        pLast = p;
        
        if ((buffer != pLast)||(k>0)) pLast++;
        
        p = strstr(pLast, ";");
        
        if(p!=NULL)
        {
            if(index == k)
            {
                trovato = true;
                strncpy(outBuffer, pLast, p-pLast);
                outBuffer[p-pLast] = '\0';
                
            }
        }
        else
        {
            if (index == k)
            {
                trovato = true;
                strcpy(outBuffer, pLast);
            }
        }

        k++;
    }
}

int Command::numberOfToken(char *buffer)
{
    int k = 0;
    int posStart = 0;
    char *p = buffer;
    
    while((p!=NULL)&&(strlen(p)>0))
    {
        if ((buffer != p)||(k>0)) p++;
        p = strstr(p, ";");
        k++;
    }
    
    return k;
}

void Command::createResponseMessage(char *inStr, char *outStr)
{
    byte *btMac = NULL;
    
    outStr[0] = '\0';
    
    if(this->_commander!=NULL)
    {
        btMac = this->_commander->getMacAddress();
        if(btMac != NULL)
        {
            for(byte k=0;k<6;k++)
            {
                sprintf(outStr+(k*2), "%02X", btMac[k]);
            }
            strcpy(outStr+12, ";resp");
            strcat(outStr+17, strstr(inStr+13, ";"));
        }
    }
}

int Command::dispatch(char *inStr, char *outStr, char *localMacaddressBuff)
{
    int retVal = 0;
    
    Serial.print("CMD = ");
    Serial.println(inStr);
    
    Serial.print("IN numOfTok = ");
    
    int numOfTok = this->numberOfToken(inStr);
    
    Serial.println(numOfTok);
    
    if(numOfTok != this->numberOfTokenRequested(inStr))
    {
        Serial.println("FN numOfTok");
        retVal = -1;
    }
    else
    {
        Serial.println("IN DspCmd");
        
        if(this->checkCommandRequested(inStr, outStr))
        {
            Serial.println("DspCmd");
            
            if(
                (this->checkBufferMacaddressWithLocal(inStr, outStr, localMacaddressBuff))
               ||
               (this->isBufferMacaddressGeneric(inStr, outStr))
            )
            {
                retVal = this->executeActionRequested(inStr, outStr);
            }
            else
            {
                retVal = -3;
            }

        }
        else
        {
            retVal = -2;
        }
        Serial.println("FN DspCmd");

        
    }
    
    return retVal;
}

