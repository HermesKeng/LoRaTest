//
//  LoraSetup.h
//  
//
//  Created by Keng on 08/11/2016.
//
//

#ifndef LoraSetup_h
#define LoraSetup_h

int setup(int Address)
{
    int e;
    // Print a start message
    printf("SX1272 module configuration in Raspberry Pi\n");
    
    // Power ON the module
    e = sx1272.ON();
    printf("Setting power ON: state %d\n", e);
    
    // Set transmission mode
    e |= sx1272.setMode(4);
    // Set header
    e |= sx1272.setHeaderON();
    // Select frequency channel
    e |= sx1272.setChannel(CH_10_868);
    // Set CRC
    e |= sx1272.setCRC_ON();
    // Select output power (Max, High or Low)
    e |= sx1272.setPower('H');
    // Set the node address
    e |= sx1272.setNodeAddress(Address);
    printf("Setting Node address: state %d\n", Address);
    
    // Print a success message
    if (e == 0)
        printf("SX1272 successfully configured\n");
    else
        printf("SX1272 initialization failed\n");
    
    delay(1000);
    return e;
}
#endif /* LoraSetup_h */
