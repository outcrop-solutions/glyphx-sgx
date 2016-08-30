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
    fprintf(stderr, "    flag           meaning                 default\n");
    fprintf(stderr, "    -------------  ----------------------  -------\n");
    fprintf(stderr, "    -D             debug                   FALSE  \n");
    fprintf(stderr, "    -c SWlat SWlon NElat NElon             0 0 0 0\n");
    fprintf(stderr, "                   corners provided as input\n");
    fprintf(stderr, "    -C filename filename                   corners_template.kml corners.csv\n");
    fprintf(stderr, "                   two file names\n");
    fprintf(stderr, "    -d filename    data file               data_in.csv\n");
    fprintf(stderr, "    -g int         num glyph lines         0\n");
    fprintf(stderr, "                   (0=read size from file)\n");
    fprintf(stderr, "    -G filename    glyph file              glyph_in.csv\n");
    fprintf(stderr, "    -h             help on this program           \n");
    fprintf(stderr, "    -i             ignore header           FALSE  \n");
    fprintf(stderr, "    -k             kludge (skip last line) FALSE  \n");
    fprintf(stderr, "    -m float       margin for corners      0.1\n");
    fprintf(stderr, "    -n int         num data lines          0\n");
    fprintf(stderr, "                   (0=read size from file)\n");
    fprintf(stderr, "    -u             usage (this message)           \n");
    fprintf(stderr, "    -p             print out corners       FALSE  \n");
    fprintf(stderr, "    -s float       scale for parent glyphs 1.0\n");
    fprintf(stderr, "    -z float       Z val for parent glyphs 0.0\n");
    fprintf(stderr, "    -T filename    tags file               ANTzTag0001.csv\n");
    fprintf(stderr, "    -x filename    text file for corners   corners.txt\n");
    fprintf(stderr, "\n");

    } /* end of usage */
