/*
 *  usage - give program usage
 */

#include        <stdio.h>

usage(argv)
char** argv;
{
    fprintf(stderr, "%s: USAGE: %s [flags] > outfile\n",
        argv[0], argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "    flag           meaning                 default   \n");
    fprintf(stderr, "    -------------  ----------------------  ----------\n");
    fprintf(stderr, "    -D             debug                   FALSE     \n");
    fprintf(stderr, "    -c # # # #     map lat/lon corners     0 0 0 0   \n");
    fprintf(stderr, "                   as floats - NW & SE               \n");
    fprintf(stderr, "                   (0.0s = ignore)                   \n");
    fprintf(stderr, "    -C             use colors              FALSE     \n");
    fprintf(stderr, "    -d <float>     decimate amount (1/d)   0.0       \n");
    fprintf(stderr, "                   0.0 = ignore                      \n");
    fprintf(stderr, "    -G <filename>  glyph file name         glyph_in.csv\n");
    fprintf(stderr, "    -g <int>       number of glyph lines   1         \n");
    fprintf(stderr, "    -h             help on this program              \n");
    fprintf(stderr, "    -n <int>       number of input lines   1         \n");
    fprintf(stderr, "    -T <filename>  tags file name          ANTzTag0001.csv\n");
    fprintf(stderr, "    -u             usage (this message)              \n");
    fprintf(stderr, "    -z <float>     z scale                 1.0       \n");
    fprintf(stderr, "\n");

    } /* end of usage */
