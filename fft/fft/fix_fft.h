//
//  fix_fft.h
//  fixedTest
//
//  Created by Adam on 2018-01-28.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef fix_fft_h
#define fix_fft_h

#include <stdio.h>

int16_t fix_fftr(int8_t f[], uint8_t m, uint8_t inverse);
uint16_t fix_fft(int8_t fr[], int8_t fi[], uint8_t m, uint8_t inverse);
void split(int8_t* X, int8_t* G, uint16_t N);

#endif /* fix_fft_h */
