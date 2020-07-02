#include <stdio.h>
#include <stdbool.h>

#define GRANULARITY 100

int mod(int val1, int val2) {
    if (val1 > val2)
        return val1-val2;
    else
        return val2-val1;
}

int main(int argc, char *argv[]) {
    FILE *fp;
    unsigned char c;
    float avgdiff = 0;
    bool complete = false; 
    unsigned char lastchar = 0;
    int numofloops = 0;
    int numofchars = 0;
    
    if (argc != 2) {
        printf("Pass file name\n");
        return -1;
    }
    
    fp = fopen(argv[1], "r");
    
    if (fp == NULL) { 
        printf("Error in opening file");
        return -1;
    }
    
    while(complete != true) {
        numofloops++;
        int i;
        int count = 0;
        for(i = 0; i < GRANULARITY; i++) {
            c = fgetc(fp);
            numofchars++;
            if (c == 255) {
                complete = true;
                break;
            }
            count += mod(c, lastchar);
            lastchar = c;
            printf("Got : %d : %d\n", c, count);    
        }
        printf("Completed %d characters\n", numofchars);
        avgdiff += count/100.0;
    }
    avgdiff /= numofloops;
    printf("Avgdiff for %d is %f\n", numofchars, avgdiff);
}
