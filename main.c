#include <stdio.h>
#include <time.h>

#define BYTE unsigned char
#define UINT unsigned long
#define BITS 25
#define HASHSIZE 32
#define SEED 0xFAFAECECFAFAECEC

UINT firstrand = 2458719153079158768;
UINT firstb    = 4631534797403582785;
UINT firstv    = 3523455478921636871;


UINT mapSizeBits = BITS;
UINT mapSize = 1 << BITS;
UINT passes = 5;
BYTE bytemap[1 << BITS];


void generate() {
    UINT offset = SEED ^ firstrand;
    UINT b = SEED ^ firstb;
    UINT v = firstv;

    UINT mask = mapSize - 1;

    long rand(UINT i) {
        offset = offset<<9 ^ offset>>1 ^ offset>>7 ^ b;
        v = (UINT)(bytemap[(offset^b)&mask]) ^ v<<8 ^ v>>1;
        b = v<<7 ^ v<<13 ^ v<<33 ^ v<<52 ^ b<<9 ^ b>>1;
        return (long)((UINT)(offset) & (UINT)(mask));
    }

    for (int i = 0; i < mapSize; i++) {
        bytemap[i] = (BYTE)i;
    }

    for (int loops = 0; loops < passes; loops++) {
        //printf("Pass %d\n", loops);
        for (int i = 0; i < mapSize; i++) {
            long j = rand(i);
            BYTE tmp = bytemap[i];
            bytemap[i] = bytemap[j];
            bytemap[j] = tmp;
        }
    }

}

BYTE * hash(BYTE src[], int len)  {
    UINT hs[HASHSIZE];
    for (int i = 0; i < HASHSIZE; i++) {
        hs[i] = 0;
    }

    UINT as = SEED;
    UINT s1 = 0, s2 = 0, s3 = 0;
    UINT mk = mapSize -1;

    UINT B(UINT v) {
        return (UINT)(bytemap[v&mk]);
    }
    BYTE b(UINT v) {
        return bytemap[v&mk];
    }

    void faststep(UINT v2, UINT idx) {
        UINT b = B(as ^ v2);
        as = as<<7 ^ as>>5 ^ v2<<20 ^ v2<<16 ^ v2 ^ b<<20 ^ b<<12 ^ b<<4;
        s1 = s1<<9 ^ s1>>3 ^ hs[idx];
        hs[idx] = s1 ^ as;
        UINT t1 = s1;
        UINT t2 = s2;
        UINT t3 = s3;
        s1 = t3;
        s2 = t1;
        s3 = t2;
    }

    void step(UINT v2, UINT idx) {
        s1 = s1<<9 ^ s1>>1 ^ as ^ B(as>>5^v2)<<3;      
        s1 = s1<<5 ^ s1>>3 ^ B(s1^v2)<<7;              
        s1 = s1<<7 ^ s1>>7 ^ B(as^s1>>7)<<5;           
        s1 = s1<<11 ^ s1>>5 ^ B(v2^as>>11^s1)<<27;     
        hs[idx] = s1 ^ as ^ hs[idx]<<7 ^ hs[idx]>>13 ; 
        as = as<<17 ^ as>>5 ^ s1 ^ B(as^s1>>27^v2)<<3 ;
        as = as<<13 ^ as>>3 ^ B(as^s1)<<7;
        as = as<<15 ^ as>>7 ^ B(as>>7^s1)<<11         ;
        as = as<<9 ^ as>>11 ^ B(v2^as^s1)<<3          ;
        s1 = s1<<7 ^ s1>>27 ^ as ^ B(as>>3)<<13       ;
        s1 = s1<<3 ^ s1>>13 ^ B(s1^v2)<<11            ;
        s1 = s1<<8 ^ s1>>11 ^ B(as^s1>>11)<<9         ;
        s1 = s1<<6 ^ s1>>9 ^ B(v2^as^s1)<<3           ;
        as = as<<23 ^ as>>3 ^ s1 ^ B(as^v2^s1>>3)<<7;
        as = as<<17 ^ as>>7 ^ B(as^s1>>3)<<5;
        as = as<<13 ^ as>>5 ^ B(as>>5^s1)<<1;
        as = as<<11 ^ as>>1 ^ B(v2^as^s1)<<7;

        s1 = s1<<5 ^ s1>>3 ^ as ^ B(as>>7^s1>>3)<<6;
        s1 = s1<<8 ^ s1>>6 ^ B(s1^v2)<<11;
        s1 = s1<<11 ^ s1>>11 ^ B(as^s1>>11)<<5;
        s1 = s1<<7 ^ s1>>5 ^ B(v2^as>>7^as^s1)<<17;

        s2 = s2<<3 ^ s2>>17 ^ s1 ^ B(as^s2>>5^v2)<<13;
        s2 = s2<<6 ^ s2>>13 ^ B(s2)<<11;
        s2 = s2<<11 ^ s2>>11 ^ B(as^s1^s2>>11)<<23;
        s2 = s2<<4 ^ s2>>23 ^ B(v2^as>>8^as^s2>>10)<<1;

        s1 = s2<<3 ^ s2>>1 ^ hs[idx] ^ v2;
        as = as<<9 ^ as>>7 ^ s1>>1 ^ B(s2>>1^hs[idx])<<5;

        UINT t1 = s1;
        UINT t2 = s2;
        UINT t3 = s3;
        s1 = t3;
        s2 = t1;
        s3 = t2;

    }

    UINT idx = 0;

    for (int i = 0; i < len; i++) {
        if (idx > HASHSIZE) {
            idx = 0;
        }
        faststep(src[i], idx);
        idx++;
    }


    idx = 0;

    for (int i = 0; i < len; i++) {
         if (idx > HASHSIZE) {
            idx = 0;
        }
        step(src[i], idx);
        idx++;
    }

    static BYTE bytes[HASHSIZE];

    for (int i = 0; i < HASHSIZE; i++) {
        bytes[i] = 0;
    }

    for (int i = HASHSIZE - 1; i >= 0; i--) {
        step(hs[i], i);
        bytes[i] = b(as) ^ b(hs[i]);
    }

    return bytes;
}


int main() {
    //mapSize = 1 << mapSizeBits;
    //printf("%lX\n", SEED);
    //printf("%lu\n", mapSize);
    clock_t t; 
    t = clock();

    generate();
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("generate() took %f seconds to execute \n", time_taken); 


    BYTE src[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    BYTE * hs = src;

    t = clock();
    BYTE * res;
    for (int i = 0; i < 100000; i++) {
        hs = hash(hs, 32);
    }
    
    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("hash() took %f seconds to execute \n", time_taken); 

    for (int i = 0; i < HASHSIZE; i++) {
        printf("%X", *(hs+i));
    }
    printf("\n");

    return 0;
}