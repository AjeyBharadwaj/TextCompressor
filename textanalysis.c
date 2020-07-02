#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define GRANULARITY 1000
#define MAXDIFF 255

findpower(int maxdiff) {
    int power = 0;
    while(pow(2, power) < maxdiff)
        power++;
    return power;
}

void analyse_bit_level(int bits, int *indivdiff) {
    int i;
    int total_compressed_chars = 0;
    int total_uncompressed_chars = 0;
    
    //bits-1 : First bit is indication to do +/-
    //-1 : If all bits are 1, then, next is 8 bits
    for(i = 0; i < pow(2, bits-1)-1; i++)
        total_compressed_chars += indivdiff[i];
    for(; i < MAXDIFF; i++)
        total_uncompressed_chars += indivdiff[i];
    
    printf("Total Compressed Chars for %d bits : %d vs %d\n", bits, total_compressed_chars, total_uncompressed_chars);
    printf("\tTotal bytes : %d\n", ((total_compressed_chars*bits)+(total_uncompressed_chars*8))/8);
}

void analyser(int maxdiff, int *indivdiff) {
    int i;
    for(i = 0; i < 10; i++)
        analyse_bit_level(i, indivdiff);
}

int main(int argc, char *argv[]) {
    FILE *fp;
    unsigned char c;
    float avgdiff = 0;
    bool complete = false; 
    unsigned char lastchar = 0;
    int numofloops = 0;
    int numofchars = 0;
    int indivdiff[255] = {0};
    int i, count;
    
    if (argc != 2) {
        printf("Pass file name\n");
        return -1;
    }
    
    fp = fopen(argv[1], "r");
    
    if (fp == NULL) { 
        printf("Error in opening file");
        return -1;
    }
    
    while (complete != true) {
        numofloops++;
        i;
        count = 0;
        for(i = 0; i < GRANULARITY; i++) {
            c = fgetc(fp);
            numofchars++;
            if (feof(fp)) {
                complete = true;
                break;
            }
            count += abs(c-lastchar);
            indivdiff[abs(c-lastchar)]++;
            lastchar = c;
            //printf("Got : %d : %d\n", c, count);    
        }
        //printf("Completed %d characters\n", numofchars);
        avgdiff += (count*1.0)/GRANULARITY;
    }
    avgdiff /= numofloops;
    
    //for(i = 0; i < MAXDIFF; i++)
        //printf("Diff %d is %d\n", i, indivdiff[i]);
    
    analyser(ceil(avgdiff), indivdiff);
    
    printf("Avgdiff for %d is %f : ceil : %f\n", numofchars-1, avgdiff, ceil(avgdiff));
    printf("Max power : %d\n", findpower(ceil(avgdiff)));
}
