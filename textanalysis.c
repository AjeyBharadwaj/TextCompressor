#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#define GRANULARITY 1000
#define MAX_BITS 8
//NOTE : Can I make MAX_DIFF based on MAX_BITS
#define MAX_DIFF 255
#define HEADER_BITS 2 
#define ALGO_MISS_FULL_READ_BIT 1

unsigned long int analyse_bit_level(int bits, int *indivdiff) {
    int i = 0;
    unsigned long int total_compressed_chars = 0;
    unsigned long int total_uncompressed_chars = 0;
    
    //bits-2 : 
    //  First bit is to indicate read next 8 bits.
    //  Second bit is indication to do +/-
    //-1 : If all bits are 1, then, next is 8 bits
    if (bits > HEADER_BITS)
        for(;i < pow(2, bits-HEADER_BITS); i++)
            total_compressed_chars += indivdiff[i];
    for(; i < MAX_DIFF; i++)
        total_uncompressed_chars += indivdiff[i];
    
    printf("Total Compressed Chars for %d bits : %d vs %d\n", bits, total_compressed_chars, total_uncompressed_chars);
    int TotalBytes = ((total_compressed_chars*bits)+(total_uncompressed_chars*(MAX_BITS+ALGO_MISS_FULL_READ_BIT)))/8;
    printf("\tTotal bytes : %d\n", TotalBytes);
    return TotalBytes;
}

unsigned long int algo_get_min_bits(int maxdiff, int *indivdiff) {
    int i;
    int max = MAX_BITS;
    unsigned long int max_file_size = INT_MAX;
    unsigned long int curr_size;
    
    for(i = 1; i < MAX_BITS+HEADER_BITS+1; i++) {
        curr_size = analyse_bit_level(i, indivdiff);
        if (curr_size < max_file_size) {
            max_file_size = curr_size;
            max = i;
        }
        //NOTE : Once it starts to increase, can I stop the loop ?
    }
    return max;
}

int get_min_bits(int val) {
    int i;
    for(i = 0; i < MAX_BITS; i++) {
        if (val < pow(2, i))
            return i;
    }
    return MAX_BITS;
}

int algo1(char *filename) {
    FILE *fp;
    unsigned char c;
    float avgdiff = 0;
    bool complete = false; 
    unsigned char lastchar = 0;
    int numofloops = 0;
    int numofchars = 0;
    int indivdiff[255] = {0};
    int i, count;
    int hit = 0, miss = 0;
        
    fp = fopen(filename, "r");
    
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
            if (feof(fp)) {
                complete = true;
                break;
            }
            numofchars++;
            count += abs(c-lastchar);
            indivdiff[abs(c-lastchar)]++;
            lastchar = c;
            //printf("Got : %d : %d\n", c, count);    
        }
        //printf("Completed %d characters\n", numofchars);
        avgdiff += (count*1.0)/GRANULARITY;
    }
    avgdiff /= numofloops;
    
    //for(i = 0; i < MAX_DIFF; i++)
        //printf("Diff %d is %d\n", i, indivdiff[i]);
    
    int NumberOfBits = algo_get_min_bits(ceil(avgdiff), indivdiff);
    
    printf("Avgdiff for %d is %f : ceil : %f\n", numofchars-1, avgdiff, ceil(avgdiff));
    
    fseek(fp, 0, SEEK_SET);
    
    complete = false;
    lastchar = 0;
    while (complete != true) {
        count = 0;
        c = fgetc(fp);
        if (feof(fp)) {
            complete = true;
            break;
        }
        if (get_min_bits(abs(lastchar-c)) <= NumberOfBits) {
            hit++;
            
        } else {
            miss++;
        }
        lastchar = c;
    }
    
    float CurrFileSize = (numofchars*1.0)/pow(2, 20);
    //23 : 2^3 bits, 2^10 KB, 2^10 MB
    float NewFileSize = ((hit*NumberOfBits)+(miss*(MAX_BITS+2)))/pow(2, 23);
    
    printf("Max power : %d\n", NumberOfBits);
    printf("Current File size : %f Mb\n", CurrFileSize);
    printf("With Compression File size : %f Mb\n", NewFileSize);
    printf("You save : %f Mb\n", CurrFileSize - NewFileSize);
    
    fclose(fp);
    
    return NumberOfBits;
    
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Pass file name\n");
        return -1;
    }
    
    algo1(argv[1]);
    
    //algo2(argv[1], algo1(argv[1]));
    return 0;
}
