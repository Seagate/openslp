/***************************************************************************/
/*                                                                         */
/* Project:     OpenSLP - OpenSource implementation of Service Location    */
/*              Protocol Version 2                                         */
/*                                                                         */
/* File:        slpd_property.c                                            */
/*                                                                         */
/* Abstract:    Defines the data structures for global SLP properties      */
/*                                                                         */
/* WARNING:     NOT thread safe!                                           */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*     Please submit patches to http://www.openslp.org                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* Copyright (C) 2000 Caldera Systems, Inc                                 */
/* All rights reserved.                                                    */
/*                                                                         */
/* Redistribution and use in source and binary forms, with or without      */
/* modification, are permitted provided that the following conditions are  */
/* met:                                                                    */ 
/*                                                                         */
/*      Redistributions of source code must retain the above copyright     */
/*      notice, this list of conditions and the following disclaimer.      */
/*                                                                         */
/*      Redistributions in binary form must reproduce the above copyright  */
/*      notice, this list of conditions and the following disclaimer in    */
/*      the documentation and/or other materials provided with the         */
/*      distribution.                                                      */
/*                                                                         */
/*      Neither the name of Caldera Systems nor the names of its           */
/*      contributors may be used to endorse or promote products derived    */
/*      from this software without specific prior written permission.      */
/*                                                                         */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     */
/* `AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   */
/* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CALDERA      */
/* SYSTEMS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON       */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT */
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   */
/* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    */
/*                                                                         */
/***************************************************************************/

/*=========================================================================*/
/* slpd includes                                                           */
/*=========================================================================*/
#include "slpd_property.h"


/*=========================================================================*/
/* Common code code includes                                               */
/*=========================================================================*/
#include "slp_message.h"
#include "slp_property.h"  
#include "slp_iface.h"
#include "slp_net.h"
#include "slp_xmalloc.h"


/*=========================================================================*/
SLPDProperty G_SlpdProperty;
/*=========================================================================*/

/*=========================================================================*/
int SLPDPropertyInit(const char* conffile)
/*=========================================================================*/
{
    char                    myname[MAX_HOST_NAME];
    char*                   myinterfaces = 0;
    char*                   myurl = 0;
    struct sockaddr_storage myaddr;
    int                     myaddr_check;
    int                     myname_len;
    int                     i;
    
    SLPPropertyReadFile(conffile);

    memset(&G_SlpdProperty,0,sizeof(G_SlpdProperty));

    /*-------------------------------------------------------------*/
    /* Set the properties without hard defaults                    */
    /*-------------------------------------------------------------*/
    G_SlpdProperty.isDA = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.isDA"));
    G_SlpdProperty.activeDADetection = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.activeDADetection"));               
    
    if(G_SlpdProperty.activeDADetection)
    {
        G_SlpdProperty.DAActiveDiscoveryInterval = atoi(SLPPropertyGet("net.slp.DAActiveDiscoveryInterval"));
        if(G_SlpdProperty.DAActiveDiscoveryInterval > 1 &&
           G_SlpdProperty.DAActiveDiscoveryInterval < SLPD_CONFIG_DA_FIND )
        {
            G_SlpdProperty.DAActiveDiscoveryInterval = SLPD_CONFIG_DA_FIND;
        }
    }
    else
    {
        G_SlpdProperty.DAActiveDiscoveryInterval = 0;
    }
    
    G_SlpdProperty.passiveDADetection = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.passiveDADetection"));                   
    G_SlpdProperty.isBroadcastOnly = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.isBroadcastOnly"));
    G_SlpdProperty.multicastTTL = atoi(SLPPropertyGet("net.slp.multicastTTL"));
    G_SlpdProperty.multicastMaximumWait = atoi(SLPPropertyGet("net.slp.multicastMaximumWait"));
    G_SlpdProperty.unicastMaximumWait = atoi(SLPPropertyGet("net.slp.unicastMaximumWait"));
    G_SlpdProperty.randomWaitBound = atoi(SLPPropertyGet("net.slp.randomWaitBound"));
    G_SlpdProperty.maxResults = atoi(SLPPropertyGet("net.slp.maxResults"));
    G_SlpdProperty.traceMsg = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.traceMsg"));
    G_SlpdProperty.traceReg = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.traceReg"));
    G_SlpdProperty.traceDrop = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.traceDrop"));
    G_SlpdProperty.traceDATraffic = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.traceDATraffic"));
    G_SlpdProperty.DAAddresses = SLPPropertyGet("net.slp.DAAddresses");
    G_SlpdProperty.DAAddressesLen = strlen(G_SlpdProperty.DAAddresses);
    /* TODO make sure that we are using scopes correctly.  What about DHCP, etc*/
    G_SlpdProperty.useScopes = SLPPropertyGet("net.slp.useScopes");
    G_SlpdProperty.useScopesLen = strlen(G_SlpdProperty.useScopes);
    G_SlpdProperty.locale = SLPPropertyGet("net.slp.locale");
    G_SlpdProperty.localeLen = strlen(G_SlpdProperty.locale);
    G_SlpdProperty.securityEnabled = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.securityEnabled"));
    G_SlpdProperty.checkSourceAddr = SLPPropertyAsBoolean(SLPPropertyGet("net.slp.checkSourceAddr"));
    G_SlpdProperty.DAHeartBeat = SLPPropertyAsInteger(SLPPropertyGet("net.slp.DAHeartBeat"));


    /*-------------------------------------*/
    /* Set the net.slp.interfaces property */
    /*-------------------------------------*/
    //// this will need to be done twice, once for ipv4 and once for ipv6
    if(SLPIfaceGetInfo(SLPPropertyGet("net.slp.interfaces"),&G_SlpdProperty.ifaceInfo,AF_INET) == 0)
    {
        if(SLPPropertyGet("net.slp.interfaces"))
        {
            if(SLPIfaceSockaddrsToString(G_SlpdProperty.ifaceInfo.iface_addr,
                                         G_SlpdProperty.ifaceInfo.iface_count,
                                         &myinterfaces) == 0)
            {
                if(myinterfaces)
                {
                    SLPPropertySet("net.slp.interfaces", myinterfaces);
                    xfree(myinterfaces);
                }
            }
        }
        G_SlpdProperty.interfaces = SLPPropertyGet("net.slp.interfaces");
    }
    G_SlpdProperty.interfacesLen = strlen(G_SlpdProperty.interfaces);
    
    
    /*---------------------------------------------------------*/
    /* Set the value used internally as the url for this agent */
    /*---------------------------------------------------------*/
    /* 27 is the size of "service:directory-agent://(NULL)" */
    //// ipv6 stuff needs to be done here too
    if(SLPNetGetThisHostname(myname,sizeof(myname),1,AF_INET) == 0)
    {
        /* if myname is an IPv6 address, wrap it with '[' and ']' */
        myaddr_check = inet_pton(AF_INET6, myname, &myaddr);
        if (myaddr_check == 1)
        {
            myname_len = strlen(myname);
            if (myname_len < MAX_HOST_NAME - 1)   /* make room for the null terminator */
            {
                for (i = myname_len; i > 0; i--)
                    myname[i] = myname[i - 1];
                myname[0] = '[';
                myname[myname_len + 1] = ']';
                myname[myname_len + 2] = '\0';
            }
        }

        myurl = (char*)xmalloc(27 + strlen(myname));
        if(G_SlpdProperty.isDA)
        {
            strcpy(myurl,SLP_DA_SERVICE_TYPE);
        }
        else
        {
            strcpy(myurl,SLP_SA_SERVICE_TYPE);
        }
        strcat(myurl,"://");
        strcat(myurl,myname);
        
        SLPPropertySet("net.slp.agentUrl",myurl);

        G_SlpdProperty.myUrl = SLPPropertyGet("net.slp.agentUrl");
        G_SlpdProperty.myUrlLen = strlen(G_SlpdProperty.myUrl);

        xfree(myurl);
    }

    /*----------------------------------*/
    /* Set other values used internally */
    /*----------------------------------*/
    G_SlpdProperty.DATimestamp = 1;  /* DATimestamp must start at 1 */
    G_SlpdProperty.activeDiscoveryXmits = 3; /* ensures xmit on first 3 calls to SLPDKnownDAActiveDiscovery() */
    G_SlpdProperty.nextActiveDiscovery = 0;  /* ensures xmit on first call to SLPDKnownDAActiveDiscovery() */
    G_SlpdProperty.nextPassiveDAAdvert = 0;  /* ensures xmit on first call to SLPDKnownDAPassiveDiscovery()*/

    return 0;
}


#ifdef DEBUG
/*=========================================================================*/
void SLPDPropertyDeinit()
/*=========================================================================*/
{
    SLPPropertyFreeAll();     
}
#endif
