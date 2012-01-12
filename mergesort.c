/* Copyright (c) 2012 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Merge_sort_(C)?action=history&offset=20110810210421

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

Retrieved from: http://en.literateprograms.org/Merge_sort_(C)?oldid=17235
*/

#include <string.h>
#include <time.h>    /* time() */
#include <stdlib.h>  /* rand() */
#include <stdio.h>   /* puts() */

#define MIN_MERGESORT_LIST_SIZE    32
void
mergesort_array(double a[], int size, double temp[]); 

void mergesort(double a[], int size) {
    double *temp = (double *)malloc(sizeof(double)*2047);
    mergesort_array(a, size, temp);
    free(temp);
}

void mergesort_array(double a[], int size, double temp[]) {
    int i1, i2, tempi;
/*    if (size < MIN_MERGESORT_LIST_SIZE) {

       //Use insertion sort 
        int i;
        for (i=0; i < size; i++) {
            int j, v = a[i];
            for (j = i - 1; j >= 0; j--) {
               if (a[j] <= v) break;
                a[j + 1] = a[j];
            }
            a[j + 1] = v;
        }
        return;
    } */
    if(size <= 1)
        return;
    
    mergesort_array(a, size/2, temp);
    mergesort_array(a + size/2, size - size/2, temp);
    i1 = 0;
    i2 = size/2;
    tempi = 0;
    while (i1 < size/2 && i2 < size) {
        if (a[i1] <= a[i2]) {
            temp[tempi] = a[i1];
            i1++;
        } else {
            temp[tempi] = a[i2];
            i2++;
        }
        tempi++;
    }

    while (i1 < size/2) {
        temp[tempi] = a[i1];
        i1++;
        tempi++;
    }
    while (i2 < size) {
        temp[tempi] = a[i2];
        i2++;
        tempi++;
    }

    memcpy(a, temp, size*sizeof(double));
}

/*
int main(int argc, char* argv[]) {
    int size = atoi(argv[1]);
    int* a    = malloc(sizeof(int)*size);
    double* temp = malloc(sizeof(double)*size);
    int i;
    srand(time(NULL));
    for(i=0; i<size; i++) {
        a[i] = rand() % size;
    }
    mergesort_array(a, size, temp);
    for(i=1; i<size; i++) {
        if (!(a[i-1] <= a[i])) {
            puts("ERROR");
            return -1;
        }
    }
    puts("SUCCESS");
    return 0;
}*/
