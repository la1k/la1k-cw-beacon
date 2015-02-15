//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include "spi.h"

#include <xc.h>
#include "platform.h"

void spi(char data){
    char dummy;
    dummy = SSP1BUF;

    SSP1BUF = data;
    while(PIR1bits.SSPIF == 0);
    PIR1bits.SSPIF = 0;

    __delay_us(20);
}
