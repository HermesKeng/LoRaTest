//
//  main.cpp
//  Lora_Setup_Nov
//
//  Created by Keng on 08/11/2016.
//  Copyright © 2016 Keng. All rights reserved.
//

#include "arduPiLoRa.h"
#include "Lora_Setup.h"
#include <iostream>
#include <string>

using namespace std;

int e;
int num = 0;
int NodeAddress;
char SendStr[100];

void loop(int e)
{
    num++;
    // Send packet broadcast and print the result
    sprintf(SendStr,"%i",NodeAddress);
    e = sx1272.sendPacketTimeout(0, SendStr);
    printf("Packet sent, state %d\n",e);
}

int main (int argc, char **argv){
    
    
    
    cout<<"NodeAddress (1=A,2=B,3=C)"<<endl;
    cin>>NodeAddress;
    e=setup(NodeAddress);
    while(1){
        loop(e);
    }
    return (0);
}
