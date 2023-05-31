// Adrian Pilkington 2023
// this started as trying to get a sensor called DHT 22 working
// DHT 22 is a Temperature humitity sensor thats widely available 
// and should be possible to implement the interface manually using the 
// info from the datasheet at: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf

// I've had a lot of trouble getting this working, the checksum is ok though??
// I may try the pido PIO rather than gpio_get, but that'll be in different source file

// of course I could have used the built in pico pi library but where's the fun in that!

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint DHT_PIN = 15;

void debug_read_from_dht();

int main() {
    stdio_init_all();
    gpio_init(DHT_PIN);
    sleep_ms(2000);

    while (1) {
        debug_read_from_dht();
        sleep_ms(4000);
    }
}

void debug_read_from_dht() 
{
    static uint DHTAttemptGetCount = 0;
    const uint SLEEP_US_TIME = 2;
    uint writeCounter = 0;
    // 80 * 2 lead in, 50+7 0 worst case all ones * 8 bits * 5 bytes + 500 for luck!
    const int MAX_DATA = ((80*2)+((50+70)*8*5) + 500) / 2; 
    char data[MAX_DATA];

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(20);
    gpio_set_dir(DHT_PIN, GPIO_IN);
    
    //as we're sleeping for 10us on each gpio get we shoudl get the following
    // after start signal from pico, 80us low, high 80us, which is 8 reads each at 10us
    // what I've seen repeately is 0, 1 then 8 zeros then 8 ones hence ignore first 2 reads
    // after that should be a 40bits of actual data
    // bit data is zero if low 50us on 20us, a one is low 50, high 70

    uint continuousOnes = 0;
    for (uint i = 0; i < MAX_DATA; i++)
    {
        uint t =  gpio_get(DHT_PIN);
        data[writeCounter++] = t;
        if (t == 1) continuousOnes++;
        if (t == 0) continuousOnes=0;
        
        sleep_us(SLEEP_US_TIME);
        if (continuousOnes > 80) break;
    }
        

    printf("gotData %d\n", DHTAttemptGetCount++);    
    printf("DHT raw data (should equate to 40bits once decoded)");
    printf("====================================================\n\n");
    
    uint outCount = 0;
    for (uint i = 0; i < writeCounter/8;i++)
    {
        for (uint p = 0; p < 8; p++)
        {
            printf("%d",data[outCount++]);
        }
        printf("  ");  
        if (outCount % (8 * 10) == 0) printf("\n");
    }
    printf("\n");
      
    printf("read attempt count %d\n", DHTAttemptGetCount++);   

    uint runLength = 0;
    uint last = 0;
    uint current = 0;
    bool inInitialSection = true;
    enum sectionType {
            e_noSection,
            e_protoLeadIn, 
            e_protoLeadOut,
            e_One,
            e_Zero,
            e_preDigit
            } theSection;
    theSection = e_noSection;
    uint bitCount = 8;
    uint byteCount = 0;
    char bitsDecoded[5] = {0,0,0,0,0};
    for (uint i = 0; i < writeCounter;i++)
    {
        current = data[i];
        if (last == current) runLength++;
        
        if (inInitialSection == true && current == 0 && runLength >= 40) 
            theSection = e_protoLeadIn;
        else  if (inInitialSection == true && current == 1 && runLength >= 40) 
            theSection = e_protoLeadOut;
        else if (inInitialSection == false)
        {
            if (current == 0 && last == 0 && runLength >= 24) theSection = e_preDigit;
            else if (last == 1 && current == 0 && runLength >= 34) theSection = e_One;
            else if (last == 1 && current == 0 && runLength >= 11) theSection = e_Zero;        
        }

        last = current;
        switch (theSection)
        {
            case e_noSection: break;  // do nothing
            case e_protoLeadIn: 
                printf("got 80usec low\n"); 
                runLength = 0; 
                break;
            case e_protoLeadOut: 
                printf("got 80usec high\n"); 
                inInitialSection = false; 
                runLength = 0; 
                break;
            case e_preDigit: 
                //printf("P"); 
                runLength = 0; 
                break;
            case e_One: 
                printf("1"); 
                char mask1 = 1 << bitCount;
                bitsDecoded[byteCount] |= mask1;
                bitCount--; 
                if (bitCount == 0) 
                {
                    bitCount = 8;
                    byteCount++;
                    printf("\n");
                } 
                runLength = 0; 
                break;
            case e_Zero: 
                printf("0");
                char mask0 = 0 << bitCount;
                bitsDecoded[byteCount] |= mask0;
                bitCount--; 
                if (bitCount == 0) 
                {
                    bitCount = 8;
                    byteCount++;
                    printf("\n");
                }
                runLength = 0; 
                break;
            default : break;
        };
        theSection = e_noSection;
    }   
    printf("data 0 = %d, ",bitsDecoded[0]);
    printf("data 1 = %d, ",bitsDecoded[1]);
    printf("data 2 = %d, ",bitsDecoded[2]);
    printf("data 3 = %d, ",bitsDecoded[3]);
    printf("data 4 = %d\n",bitsDecoded[4]);
    if (bitsDecoded[4] != bitsDecoded[0]+bitsDecoded[1]+bitsDecoded[2]+bitsDecoded[3])
    {
        printf("checksum failed\n");
    }
    else
    {
        float temp = (float) (((bitsDecoded[3] & 0x7F) << 8) + bitsDecoded[2]) / 10.0;
        if (temp > 125) {
            temp = bitsDecoded[3];
        }
        if (bitsDecoded[3] & 0x80) {
            temp = -temp;
        }
        printf("Temperature in C = %fC", temp);
    }
}
 