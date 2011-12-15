#include <math.h>    // for the square root function sqrt()     
#include <stdio.h>
#include <stdlib.h>
#include "wavereader.h"
#include "stat.h"

/*short a = 0xff00
(char[])a[0] == 0xff
((char [])a)[0] == 0xff
((char *)a)[0] == 0xff <-- most */

/*x[i + 0] << 24 || x[i + 1] << 16 || x[i + 2] << 8 || x[i + 3] / (double)(2 << bitdepth)
    for(...) {
        accum ||= *++x;
        accum <<= 8
    }
accum / (double)(2 << bitdepth)*/

int
xrr(int maxdelay, buffer_t *sig1, buffer_t *sig2)
{
int i,j 
   double mx,my,sx,sy,sxy,denom,r;
    x = sig1->pcm[0];
    y = sig2->pcm[0];

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
    
    for (delay = -maxdelay; delay < maxdelay; delay++;){
        sxy=0;
        for(i = 0; i < n; i ++){
            j = i + delay;
            if(j < 0 || j >= n)
                continue;
            /* OR? sxy += (x[i] - mx) * (- my); */
            else
                sxy += (x[i] - mx) * (y[j] - my);
        } 
        r[delay] = sxy / denom;

}

    /* Calc the corr series - cirular boundary conditions 
    for (delay = -maxdelay; delay < maxdelay; delay++;)
        sxy=0;
        for(i = 0; i < n; i ++){
            j = i + delay;
            while(j < 0)
                j += n;
            j %= n;
            sxy += (x[i] - mx) * (y[j] - my);
        }
        r = sxy / denom;*/
