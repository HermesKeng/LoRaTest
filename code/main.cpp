//
//  main.cpp
//  Lora
//
//  Created by Keng on 19/10/2016.
//  Copyright © 2016 Keng. All rights reserved.
//

#include "arduPiLoRa.h"
#include <string>
#include "FileTran.h"
#include "LoRa_RSSIToDistance.h"
using namespace std;

int e;
float rssi_value;
string LoRaRecvNum;
int NodeAddress;
char my_packet[100];
Rssi_info rssi_info;

void setup()
{
    // Print a start message
    printf("SX1272 module configuration in Raspberry Pi\n");
    
    // Power ON the module
    e = sx1272.ON();
    printf("Setting power ON: state %d\n", e);

    // Set transmission mode
    e |= sx1272.setMode(4);
    printf("Setting Mode: state %d\n", e);
    
    // Set header
    e |= sx1272.setHeaderON();
    printf("Setting Header ON: state %d\n", e);
    
    // Select frequency channel
    e |= sx1272.setChannel(CH_10_868);
    printf("Setting Channel: state %d\n", e);
    
    // Set CRC
    e |= sx1272.setCRC_ON();
    printf("Setting CRC ON: state %d\n", e);
    
    // Select output power (Max, High or Low)
    e |= sx1272.setPower('H');
    printf("Setting Power: state %d\n", e);
    // Set the node address
    NodeAddress=1;
    e |= sx1272.setNodeAddress(NodeAddress);
    printf("Setting Node address: state %d\n", e);
    
    // Print a success message
    if (e == 0)
        printf("SX1272 successfully configured\n");
    else
        printf("SX1272 initialization failed\n");
    
    delay(1000);
}

void Recv(Rssi_info &rssi_info){
    
    bool is_RSSI;
    char rssiValue[20];
    char ReceiveMessage[100];
    printf("Begin receiving message ! \n");
    // Receive message
    e = sx1272.receivePacketTimeout(10000);
    if ( e == 0 )
    {
        printf("Receive packet, state %d\n",e);
        
        for (unsigned int i = 0; i < sx1272.packet_received.length; i++)
        {
            my_packet[i] = (char)sx1272.packet_received.data[i];
        }
               //get rssi_value;
        is_RSSI=sx1272.getRSSIpacket();
        if(is_RSSI){
            rssi_value=sx1272._RSSIpacket;
            rssi_info.RSSI = rssi_value;
            
        }
        else{
            printf("rssi error !\n");
            fileInput("rssi error!\n");
            return;
        }
        
        
        //display RSSI record it
        sprintf(ReceiveMessage,"Packet send from address = %s\n",my_packet);
        sprintf(rssiValue,"rssi = %f ",rssi_value);
        printf("%s ", ReceiveMessage);
        printf("%f\n",rssi_value);
        fileInput(ReceiveMessage);
        fileInput(rssiValue);
    }
    else {
        printf("Receive packet, state %d\n",e);
    }
    return;
}

char* findDeviceID(int address){
    if(address == 1){
        return "A";
    }
    else if(address == 2){
        return "B";
    }
    else if(address == 3){
        return "C";
    }
}
int main (int argc, char **argv){
    LoRaRecvNum = 'none';
    fileOpen(argv[1]);
    
    //Rssi_info* rssi_arr;
    //Rssi_BufferManager bufferManger;
    Rssi_info new_rssi;
    Locate_info loca_info;
    
    int count = 0;
    setup();
    while(count < 3){
        rssi_info.RSSI = 0;
        rssi_info.deviceID = NULL;
        rssi_info.number = 0;
        Recv(new_rssi);
        count++;
    }
    
    
    return (0);
}

