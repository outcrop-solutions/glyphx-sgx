/*
 *  help - give program help
 */

#include        <stdio.h>

help(argv)
char** argv;
{
    fprintf(stderr, "%s: HELP:\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "    xyz2csv - convert lon/lat, elevation, optional\n");
    fprintf(stderr, "              colors and tags to ANTz csv files\n");
    fprintf(stderr, "\n");

    } /* end of help */
