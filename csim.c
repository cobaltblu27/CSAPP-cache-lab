#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "cachelab.h"

const char *msg = "Options:\n"
        "  -h         Print this help message.\n"
        "  -v         Optional verbose flag.\n"
        "  -s <num>   Number of set index bits.\n"
        "  -E <num>   Number of lines per set.\n"
        "  -b <num>   Number of block offset bits.\n"
        "  -t <file>  Trace file.\n"
        "\n"
        "Examples:\n"
        "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
        "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace";

int main(int argc, char *argv[]) {
    FILE *tracefile = NULL;
    long **cache;
    unsigned addr;
    int size;
    int s = 0, E = 0, b = 0, S, B;
    int hit = 0, miss = 0, evict = 0;
    int verbose = 0;
    int opt;
    char id;

    while (-1 != (opt = getopt(argc, argv, "hvs:E:b:t:"))) {
        switch (opt) {
            case 'h':
                puts(msg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = (int) strtol(optarg, NULL, 10);
                break;
            case 'E':
                E = (int) strtol(optarg, NULL, 10);
                break;
            case 'b':
                b = (int) strtol(optarg, NULL, 10);
                break;
            case 't':
                tracefile = fopen(optarg, "r");
                break;
            default:
                break;
        }
    }

    if (tracefile == NULL || E == 0) {
        if (verbose)
            printf("wrong argument!\n");
        printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
        exit(0);
    }
    S = 1 << s;
    B = 1 << b;
    cache = malloc(sizeof(long *) * S);
    for (int i = 0; i < S; i++) {
        cache[i] = malloc(sizeof(long) * E);
        for (int j = 0; j < E; j++)
            cache[i][j] = -1;
    }
    while (fscanf(tracefile, " %c %x,%d", &id, &addr, &size) > 0) {
        if (id == 'I') continue;
        if (verbose)
            printf("%c %x,%d ", id, addr, size);
        addr -= addr % B;
        long *tset = cache[(addr % (S * B)) >> b];
        //where memory should be in cache
        for (int c = 0; c < 2; c++) {//do this twice when id == 'M', once otherwise
            int hit_index = -1;
            for (int i = 0; i < E; i++) {
                if (tset[i] == addr) {//hit
                    if (verbose)
                        printf("hit ");
                    hit++;
                    hit_index = i;
                    break;
                }
            }
            if (hit_index == -1) {//miss
                miss++;
                if (verbose)
                    printf("miss ");
                if (tset[E - 1] != -1) {
                    if (verbose)
                        printf("eviction ");
                    evict++;
                }
                tset[E - 1] = addr;
                hit_index = E - 1;
            }
            long temp = tset[hit_index];
            for (int i = hit_index; i > 0; i--) {
                tset[i] = tset[i - 1];
            }
            tset[0] = temp;
            if (id != 'M')
                break;
        }
        printf("\n");
    }


    printSummary(hit, miss, evict);

    for (int i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);

    fclose(tracefile);
    return 0;
}





