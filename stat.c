#include <math.h>    // for the square root function sqrt()     
#include <stdio.h>
#include <stdlib.h>
#include "wavereader.h"
#include "stat.h"

/*short a = 0xff00
(char[])a[0] == 0xff
((char [])a)[0] == 0xff
((char *)a)[0] == 0xff <-- most */


double **
xcorr(int maxdelay, buffer_t *sig1, buffer_t *sig2)
{
    //THE SHITTY O(n^2) way! (i.e. not using fft)
    int i,j; 
    double mx,my,sx,sy,sxy,denom;
    double n;       // length of longest vector

    double *small;
    double *big;
    double **R;

    /* hack to match vector lengths by zero padding */
    if(sig1->length >= sig2->length) {
        n = sig1->length;
        small = sig2->vector[0];
        big = sig1->vector[0];
    }else{
        n = sig2->length;
        small = sig1->vector[0];
        big = sig2->vector[0];
    }

    double y[(size_t)n];
    //y = calloc(n, sizeof(double));
    for(i = 0; i < n; i++) 
        y[i] = small[i];
    
    double *x = big;
    
    /* allocate mem for output correlations and lags */
    R = malloc(2 * sizeof(double *));
    R[0] = calloc((size_t)(2*n - 1), sizeof(double));
    R[1] = calloc((size_t)(2*n - 1), sizeof(double));

    /*calculate the mean of the two series x[], y[] */
    mx = 0;
    my = 0;

    for (i = 0; i < n; i++) {
        mx += x[i];
        my += y[i];
    }
    mx /= n;
    my /= n;

    /*Calc the denominator*/

    sx = 0;
    sy = 0;
    for (i = 0; i < n; i++) {
        sx += (x[i] - mx) * (x[i] - mx);
        sy += (y[i] - my) * (y[i] - my);
    }
    denom = sqrt(sx * sy);

    /* Calculate the correlation series */

    if(maxdelay == 0)
        maxdelay = n - 1;    

    int delay = -maxdelay;

    for (int index = 0; delay < 2*n; index++) {
        sxy=0;
        for(i = 0; i < n; i ++) {
            j = i + delay;
            if(j < 0 || j >= n)
                continue;
               //OR? sxy += (x[i] - mx) * (- my); 
            else
                sxy += (x[i] - mx) * (y[j] - my);
        } 
        R[0][index] = -delay;
        R[1][index] = sxy / denom;
        delay ++;
    }
    return R;
}

    /* Calc the corr series - cirular boundary conditions 
    for (delay = -maxdelay; delay < maxdelay; delay++;)
        sxy=0;
        for(i = 0; i < n; i ++) {
            j = i + delay;
            while(j < 0)
                j += n;
            j %= n;
            sxy += (x[i] - mx) * (y[j] - my);
        }
        R = sxy / denom;*/
