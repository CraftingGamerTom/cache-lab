#include "cachelab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define MAX_COUNTER_VAL 10000

typedef unsigned long long int mem_addr_t;

typedef struct {
	int valid;		// valid bit
	mem_addr_t tag; // tag
	int count;		// used for eviction
} Line;

typedef struct {
	Line* lines;	// array of lines
} Set;

typedef struct {
	Set* sets;		// array of sets
	int size_set;	// number of sets
	int size_line;	// number of lines per set
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

// forward declarations
void initCache(cache_params_t cache_params, Cache* cache);
void initParams(cache_params_t * cache_params, int s, int b, int E);
void get_Opt(int argc, char **argv, int *s, int *E, int *b, char *tracefileName,
		int *isVerbose);
int findMinLruNumber(Cache *cache, int setBits);
int updateCache(Cache* cache, int setBits, int tagBits);
void updateLRUCounter(Cache *cache, int setBits, int hitNum);
int isMiss(Cache *cache, int setBits, int tagBits);
void modifyData(Cache *cache, int addr, int size, int setBits, int tagBits,
		int isVerbose, cache_params_t * params);
void storeData(Cache *cache, int addr, int size, int setBits, int tagBits,
		int isVerbose, cache_params_t * params);
void loadData(Cache *cache, int addr, int size, int setBits, int tagBits,
		int isVerbose, cache_params_t * params);
int getTag(int addr, int s, int b);
int getSet(int addr, int s, int b);
void validateArgument(char *curOptarg, char ** argv);


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

int getSet(int addr, int s, int b) {
	addr = addr >> b;
	int mask = (1 << s) - 1;
	return addr & mask;
}

int getTag(int addr, int s, int b) {
	int mask = s + b;
	return addr >> mask;
}

void loadData(Cache *cache, int addr, int size, int setBits, int tagBits,
		int isVerbose, cache_params_t * cache_params) {
	if (isMiss(cache, setBits, tagBits) == 1) {
		cache_params->misses++;
		if (isVerbose == 1) {
			printf(" miss ");
		}
		if (updateCache(cache, setBits, tagBits) == 1) {
			cache_params->evictions++;
			if (isVerbose == 1)
				printf(" eviction ");
		}
	} else {
		cache_params->hits++;
		if (isVerbose == 1) {
			printf(" hit ");
		}
	}
}

void storeData(Cache *cache, int addr, int size, int setBits, int tagBits,
		int isVerbose, cache_params_t * params) {
	loadData(cache, addr, size, setBits, tagBits, isVerbose, params);
}

void modifyData(Cache *cache, int addr, int size, int setBits, int tagBits,
		int isVerbose, cache_params_t * params) {
	loadData(cache, addr, size, setBits, tagBits, isVerbose, params);
	storeData(cache, addr, size, setBits, tagBits, isVerbose, params);
}

int isMiss(Cache *cache, int setBits, int tagBits) {
	int isMiss = 1;
	for (int i = 0; i < cache->size_line; i++) {
		if (cache->sets[setBits].lines[i].valid == 1
				&& cache->sets[setBits].lines[i].tag == tagBits) {
			isMiss = 0;
			updateLRUCounter(cache, setBits, i);
		}
	}
	return isMiss;
}

void updateLRUCounter(Cache *cache, int setBits, int hitNum) {
	cache->sets[setBits].lines[hitNum].count = MAX_COUNTER_VAL;
	for (int j = 0; j < cache->size_line; j++) {
		if (j != hitNum) {
			cache->sets[setBits].lines[j].count--;
		}
	}
}

int updateCache(Cache* cache, int setBits, int tagBits) {
	int i;
	int isfull = 1;
	for (i = 0; i < cache->size_line; i++) {
		if (cache->sets[setBits].lines[i].valid == 0) {
			isfull = 0;
			break;
		}
	}
	if (isfull == 0) {
		cache->sets[setBits].lines[i].valid = 1;
		cache->sets[setBits].lines[i].tag = tagBits;
		updateLRUCounter(cache, setBits, i);
	} else {
		int evictionIndex = findMinLruNumber(cache, setBits);
		cache->sets[setBits].lines[evictionIndex].valid = 1;
		cache->sets[setBits].lines[evictionIndex].tag = tagBits;
		updateLRUCounter(cache, setBits, evictionIndex);
	}
	return isfull;
}

int findMinLruNumber(Cache *cache, int setBits) {
	int minIndex = 0;
	int minLru = MAX_COUNTER_VAL;
	for (int i = 0; i < cache->size_line; i++) {
		if (cache->sets[setBits].lines[i].count < minLru) {
			minIndex = i;
			minLru = cache->sets[setBits].lines[i].count;
		}
	}
	return minIndex;
}

void get_Opt(int argc, char **argv, int *s, int *E, int *b, char *tracefileName,
		int *isVerbose) {
	int c;
	while ((c = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
		switch (c) {
		case 'v':
			*isVerbose = 1;
			break;
		case 's':
			validateArgument(optarg, argv);
			*s = atoi(optarg);
			break;
		case 'E':
			validateArgument(optarg, argv);
			*E = atoi(optarg);
			break;
		case 'b':
			validateArgument(optarg, argv);
			*b = atoi(optarg);
			break;
		case 't':
			validateArgument(optarg, argv);
			strcpy(tracefileName, optarg);
			break;
		case 'h':
		default:
			printHelp(argv);
		}
	}
}

void initParams(cache_params_t * cache_params, int s, int b, int E) {
	// Set the parameters
	cache_params->s = s; /* 2**s cache sets */
	cache_params->b = b; /* cacheline block size 2**b bytes */
	cache_params->E = E; /* number of cachelines per set */
	cache_params->S = 2 ^ cache_params->s; /* number of sets, derived from S = 2**s */
	cache_params->B = 2 ^ cache_params->b; /* cacheline block size (bytes), derived from B = 2**b */
	cache_params->hits = 0;
	cache_params->misses = 0;
	cache_params->evictions = 0;
}

void initCache(cache_params_t cache_params, Cache* cache) {

	cache->size_set = 2 << cache_params.s;
	cache->size_line = cache_params.E;
	cache->sets = (Set *) malloc(cache->size_set * sizeof(Set));
	if (!cache->sets) {
		printf("Set memory error!\n");
		exit(0);
	}

	//allocate memory for the lines
	for (int j = 0; j < cache->size_set; j++) {
		cache->sets[j].lines = (Line *) malloc(sizeof(Line) * cache_params.E);

		for (int i = 0; i < cache_params.E; i++) {
			cache->sets[j].lines[i].count = 0;
			cache->sets[j].lines[i].tag = 0;
			cache->sets[j].lines[i].valid = 0;
		}
	}
}

int main(int argc, char **argv) {
	int s, E, b, isVerbose = 0;
	char tracefilename[257], opt[10];
	int addr, size;

	// Initialize objects
	Cache cache;
	cache_params_t cache_params;

	// Get the variables
	get_Opt(argc, argv, &s, &E, &b, tracefilename, &isVerbose);

	// init working parameters
	initParams(&cache_params, s, b, E);

	// Initialize the cache
	initCache(cache_params, &cache);

	// Read in the data
	FILE *file = fopen(tracefilename, "r");

	while (fscanf(file, "%s, %x, %d", opt, &addr, &size) != EOF) {
		if (strcmp(opt, "I") == 0)
			continue;
		int setBits = getSet(addr, s, b);
		int tagBits = getTag(addr, s, b);
		if (isVerbose == 1)
			printf("%s %x, %d", opt, addr, size);

		if (strcmp(opt, "S") == 0) {
			storeData(&cache, addr, size, setBits, tagBits, isVerbose,
					&cache_params);
		}
		if (strcmp(opt, "M") == 0) {
			modifyData(&cache, addr, size, setBits, tagBits, isVerbose,
					&cache_params);
		}
		if (strcmp(opt, "L") == 0) {
			loadData(&cache, addr, size, setBits, tagBits, isVerbose,
					&cache_params);
		}
		if (isVerbose == 1) {
			printf("\n");
		}
	}

	// print summary
	printSummary(cache_params.hits, cache_params.misses,
			cache_params.evictions);
	return 0;
}

void validateArgument(char *arg, char ** argv) {
	if (arg[0] == '-') {
		printf("./csim : Invalid command line argument\n");
		printHelp(argv);
	}
}
