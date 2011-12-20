#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include <math.h>    // for the square root function sqrt()     
#include <stdio.h>
#include <stdlib.h>
#include "wavereader.h"
#include "stats.h"

/*short a = 0xff00
(char[])a[0] == 0xff
((char [])a)[0] == 0xff
((char *)a)[0] == 0xff <-- most */


double **
xcorr(int maxdelay, buffer_t *sig1, buffer_t *sig2);


#endif
