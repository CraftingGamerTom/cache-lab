#include "cachelab.h"

#include "cachelab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

typedef struct {
    // valid bit
    int valid;
    // tag
    int tag;
    // used counter for LRU (Least recently used)
    int count;
} Line;

typedef struct {
    Line* line;

} Set;

typedef struct {
    Set* sets;
    int size_set;
    int size_line;
} Cache;

// A struct that groups cache parameters together
typedef struct {
    int s; /* 2**s cache sets */
    int b; /* cacheline block size 2**b bytes */
    int E; /* number of cachelines per set */
    int S; /* number of sets, derived from S = 2**s */
    int B; /* cacheline block size (bytes), derived from B = 2**b */

    int hits;
    int misses;
    int evictions;
} cache_params_t;

void printHelp(char* argv[]) {
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

void getSet() {

}

void getTag() {

}

void loadData(Cache *cache, int addr, int size, int setBits, int tageBits, int isVerbose) {

}

void storeData(Cache *cache, int addr, int size, int setBits, int tageBits, int isVerbose) {

}

void modifyData(Cache *cache, int addr, int size, int setBits, int tageBits, int isVerbose) {

}

int isMiss(Cache *cache, int setBits, int tagBits) {

}

void updateLRUCounter(Cache *cache, int setBits, int hitNum) {

}

int updateCache(Cache* cache, int setBits, int tagBits) {

}

int findMinLru(Cache* cache, int setBits) {

}

int get_Opt(int argx, char **arrgv, int *s, int *E, int *b, char *traccefileName, int *isVerbose);

//misses
int misses;
//hits
int hits;
//evictions
int evictions;

void initCache(cache_params_t cache_params, Cache* cache) {

    cache->size_set = 2 << cache_params.s;
    cache->size_line = cache_params.E;
    cache.sets = (Set *) malloc(cache->size_set * sizeof(Set));
    if (!cache->sets) {
        printf("Set memory error!\n");
        exit(0);
    }
    //allocate memory for the lines
    for (int j = 0; j < cache_params.s; j++) {
        cache_params.s.Line = (Line *) malloc(sizeof (Line) * cache_params.E);

        for (int i = 0; i < cache_params.E; i++) {
            Line.count = 0;
            Line.tag = 0;
            Line.valid = 0;
        }
    }



}


int main()
{
    int s, E, b isVerbos=0;
    char tracefilename[100], opt[10];
    int addr, size;

    // Zero out all counts
    miss = hits = evictions = 0;

    // Initialize objects
    Cache cache;
    cache_params_t cache_params;

    // Get the variables
    get_Opt(argc, argv, &s, &E, &b, tracefilename, &isVerbose);

    // Set the parameters
    cache_params.s = s; /* 2**s cache sets */
    cache_params.b = b; /* cacheline block size 2**b bytes */
    cache_params.E = E; /* number of cachelines per set */
    cache_params.S = pow(2.0, cache_params.s); /* number of sets, derived from S = 2**s */
    cache_params.B = pow(2.0, cache_params.b); /* cacheline block size (bytes), derived from B = 2**b */

    cache_params.hits = 0;
    cache_params.misses = 0;
    cache_params.evictions = 0;

    // Initialize the cache
    initCache(cache_params, &cache);

    // Read in the data
    FILE *file = fopen(tracefilename, "r");

    while(fscanf(file, "%s, %x, %d", opt, &addr, $size) != EOF) {
        if(strcmp(opt, "I") == 0) continue;
        int setBits = getSet(addr, s, b);
        int getBits = getTag(addr, s, b);
        if(isVerbose == 1) printf("%s %x, %d", opt, addr, size);

        if(strcmp(opt, "S") == 0) {
            storeData(&cache, addr, size, setBits, tagBits, isVerbose);
        }
        if(strcmp(opt, "M") == 0) {
            modifyData(&cache, addr, size, setBits, tagBits, isVerbose);
        }
        if(strcmp(opt, "L") == 0) {
            loadData(&cache, addr, size, setBits, tagBits, isVerbose);
        }
        if(isVerbose == 1) {
            printf("\n");
        }
    }



    printSummary(cache_params.hits, cache_params.misses, cache_params.evictions);
    return 0;
}


