/*
 *  help - give program help
 */

#include        <stdio.h>

help(argv)
char** argv;
{
    fprintf(stderr, "%s: HELP:\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "    gps2csv - convert lon/lat and tags to ANTz files\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    This program is designed to take as input the output\n");
    fprintf(stderr, "    of a Python script to convert Google Earth kml and\n");
    fprintf(stderr, "    kmz files into plain text, with a one-line\n");
    fprintf(stderr, "    header followed by this format:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    longitude,latitude,0,,name\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    example:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    70.77356,34.61004,0,,Bar Nurgal\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    plus a template glyph file in ANTZ csv fotmat,\n");
    fprintf(stderr, "    and then produce an ANTz csv file with one copy\n");
    fprintf(stderr, "    of the glyph for each data row.\n");
    fprintf(stderr, "\n");

    } /* end of help */
