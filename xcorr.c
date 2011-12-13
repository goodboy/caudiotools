#include <math.h>    // for the square root function sqrt()     
#include <stdio.h>
#include <stdlib.h>
#include "wavereader.h"

/* function prototypes */
//int xcorr(int maxdelay, int x[], int y[]);
 
void *main(int argc, char *argv[])
{
    int count;
    
    if (argc > 3){  //limit input to 2 vectors

        printf("You've passed too many inputs to \%s ",argv[0]);
    }
    else if(argc>1){
        for(count=1; count<argc; count++){
        //do stuff to open .wav files      
        //x = waveparse(argv[1]);
        //y = waveparse(argv[2]);
        }

    return 1;
}

int xcorr(int maxdelay,int x[],int y[])
{

//xcorr function: takes args -> 
void xcorr(short imaxdelay,buffer *x, buffer *y)
{
    int i,j 
    double mx,my,sx,sy,sxy,denom,r;

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
    for (i=0;i<n;i++) {
        sx += (x[i] - mx) * (x[i] - mx);
        sy += (y[i] - my) * (y[[i] - my);
    }
    denom = sqrt(sx*sy); 

    /* Calc the corr series */
    for (delay=-maxdelay;delay<maxdelay;delay++;)
        sxy=0;
        for
