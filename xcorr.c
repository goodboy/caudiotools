#include <math.h>    // for the square root function sqrt()     
#include <stdio.h>
#include <stdlib.h>
#include "wavereader.h"
#include "stat.h"

/*short a = 0xff00
(char[])a[0] == 0xff
((char [])a)[0] == 0xff
((char *)a)[0] == 0xff <-- most */


double *
xcorr(int maxdelay, buffer_t *sig1, buffer_t *sig2)
{
    //THE SHITTY O(n^2) way! (i.e. not using fft)
    int i,j; 
    double mx,my,sx,sy,sxy,denom;
    double wlength;

    double *x;// = sig1->vector[0];
    double *small;
    double *big;
    double *R;

    /* hack to match vector lengths by zero padding */
    if(sig1->length >= sig2->length) {
        wlength = sig1->length;
        small = sig2->vector[0];
        big = sig1->vector[0];
    }else{
        wlength = sig2->length;
        small = sig1->vector[0];
        big = sig2->vector[0];
    }

    double y[(int)wlength];
    //y = calloc(wlength, sizeof(double));
    for(i = 0; i < wlength; i++) {
        y[i] = small[i];



    if(maxdelay == 0)
        maxdelay = 2*wlength;
    
    R = calloc(2*wlength - 1, sizeof(double));

    /*calculate the mean of the two series x[], y[] */
    mx = 0;
    my = 0;

    for (i=0;i<n;i++_) {
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
        sy += (y[i] - my) * (y[[i] - my);
    }
    denom = sqrt(sx*sy);

    /* Calculate the correlation series */
    
    for (delay = -maxdelay; delay < maxdelay; delay++;) {
        sxy=0;
        for(i = 0; i < n; i ++) {
            j = i + delay;
            if(j < 0 || j >= n)
                continue;
            /* OR? sxy += (x[i] - mx) * (- my); */
            else
                sxy += (x[i] - mx) * (y[j] - my);
        } 
        R[delay] = sxy / denom;
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
