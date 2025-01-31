/**
 * @file test_fopen.c Test file opening
 * @author David Huggins-Daines <dhdaines@gmail.com>
 */

#include <sphinxbase/pio.h>
#include "test_macros.h"

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    FILE *fh;
    char line[256];
    int ispipe;

    (void)argc;
    (void)argv;
    fh = fopen_comp(LMDIR "/100.lm.gz", "r", &ispipe);
    TEST_ASSERT(fh != NULL);
    (void) fgets(line, sizeof(line), fh);
    TEST_EQUAL('#', line[0]);
    fclose_comp(fh, ispipe);

    fh = fopen_compchk(LMDIR "/100.lm.gz", &ispipe);
    TEST_ASSERT(fh != NULL);
    (void) fgets(line, sizeof(line), fh);
    TEST_EQUAL('#', line[0]);
    fclose_comp(fh, ispipe);

    fh = fopen_compchk(LMDIR "/100.lm.bz2", &ispipe);
    TEST_ASSERT(fh != NULL);
    (void) fgets(line, sizeof(line), fh);
    TEST_EQUAL('#', line[0]);
    fclose_comp(fh, ispipe);

    fh = fopen_compchk(LMDIR "/100.lm", &ispipe);
    TEST_ASSERT(fh != NULL);
    (void) fgets(line, sizeof(line), fh);
    TEST_EQUAL('#', line[0]);
    fclose_comp(fh, ispipe);

    return 0;
}
