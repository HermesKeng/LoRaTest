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
    //printf("SX1272 module configuration in Raspberry Pi\n");
    
    // Power ON the module
    e = sx1272.ON();
    //printf("Setting power ON: state %d\n", e);

    // Set transmission mode
    e |= sx1272.setMode(4);
    printf("Setting Mode: state %d\n", e);
    
    // Set header
    e |= sx1272.setHeaderON();
    //printf("Setting Header ON: state %d\n", e);
    
    // Select frequency channel
    e |= sx1272.setChannel(CH_10_868);
    printf("Setting Channel: state %d\n", e);
    
    // Set CRC
    e |= sx1272.setCRC_ON();
    //printf("Setting CRC ON: state %d\n", e);
    
    // Select output power (Max, High or Low)
    e |= sx1272.setPower('H');
    //printf("Setting Power: state %d\n", e);
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

char findDeviceID(char address){
    if(address == '1'){
        return "A";
    }
    else if(address == '2'){
        return "B";
    }
    else if(address == '3'){
        return "C";
    }
}

Rssi_info Recv(Rssi_info rssi_info){
    
    bool is_RSSI;
    char rssiValue[20];
    char ReceiveMessage[100];
    char deviceID;
    
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
        if(!is_RSSI){
            rssi_value=sx1272._RSSIpacket;
            rssi_info.RSSI = rssi_value;
        }
        else{
            printf("rssi error !\n");
            fileInput("rssi error!\n");
            return rssi_info;
        }
        
        deviceID = findDeviceID(my_packet[0]);
        
        rssi_info.RSSI = rssi_value;
        rssi_info.deviceID = deviceID;
        rssi_info.number = 1;
        //display RSSI record it
        sprintf(ReceiveMessage,"Packet send from address = %s\n",my_packet);
        sprintf(rssiValue,"rssi = %.2f ",rssi_value);
        printf("%s ", ReceiveMessage);
        printf("%.2f\n",rssi_value);
        fileInput(ReceiveMessage);
        fileInput(rssiValue);
    }
    else {
        printf("Receive packet, state %d\n",e);
    }
    return rssi_info;
}


int main (int argc, char **argv){
    LoRaRecvNum = 'none';
    fileOpen(argv[1]);
    float distance[3];
    char output[50];
    //Rssi_info* rssi_arr;
    //Rssi_BufferManager bufferManger;
    Rssi_info new_rssi;
    Rssi_info packet[3];
    Locate_info loca_info;
    Point finalPoint;
    Point BSpointA,BSpointB,BSpointC;
    BSpointA=set_Bspoint(4.0,4.0, 'A');
    BSpointB=set_Bspoint(9.0,7.0, 'B');
    BSpointC=set_Bspoint(9.0,1.0, 'C');
    
    
    int count = 0;
    setup();
    printf("Begin receiving message ! \n");
    while(count < 3){
        //default set up;
        packet[count]=Recv();
        distance[count] = Rssi_to_distance(packet[count], -34, 2);
        printf("Point %c , Distance : %.2f \n",packet[count].deviceID,distance[count])
        count++;
    }
    for(int i=0;i<3;i++){
        switch (packet[count].deviceID) {
            case 'A':
                loca_info.distance[0]=distance[0];
                break;
            case 'B':
                loca_info.distance[1]=distance[1];
                break;
            case 'C':
                loca_info.distance[2]=distance[2];
                break;
            default:
                break;
        }
    }
    finalPoint=trilateration(BSpointA, BSpointB, BSpointC, loca_info.distances[0],loca_info.distances[1],loca_info.distances[2]);
    printf("finalPoint>> X: %.2f ,Y: %.2f \n",finalPoint.x,finalPoint.y);
    sprintf(output,"finalPoint>> X: %.2f ,Y: %.2f \n",finalPoint.x,finalPoint.y);
    fileInput(output);
    return (0);
}

