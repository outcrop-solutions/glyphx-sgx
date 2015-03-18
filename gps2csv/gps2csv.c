/**
**   gps2csv - conver lat/lon to Antz fromat
**
**  CREATED: 2014.03.05  ABS  copied from test_glyph_tools
** MODIFIED: 2014.03.10  ABS  added Scale & Z, corners
** MODIFIED: 2014.03.12  ABS  added tags
** MODIFIED: 2014.03.15  ABS  tweaked
** MODIFIED: 2014.05.01  ABS  mods for use by Ray's GUI
** MODIFIED: 2014.05.03  ABS  corners out, header processing
** MODIFIED: 2014.05.04  ABS  fencepost error; UNIX -> Windows style line termination
** MODIFIED: 2014.05.05  ABS  added -k for Kludge (ignore last input line)
** MODIFIED: 2014.05.06  ABS  looking for corners bug
** MODIFIED: 2014.05.13  ABS  yet another corners bug
**
**/

#include        "gps2csv.h"

char *replace(const char *str, const char *old, const char *new)
{
    char *ret, *r;
    const char *p, *q;
    size_t oldlen = strlen(old);
    size_t count, retlen, newlen = strlen(new);

    if (oldlen != newlen) {
        for (count = 0, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen)
            count++;
        /* this is undefined if p - str > PTRDIFF_MAX */
        retlen = p - str + strlen(p) + count * (newlen - oldlen);
    }
    else
        retlen = strlen(str);

    if ((ret = malloc(retlen + 1)) == NULL)
        return NULL;

    for (r = ret, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen) {
        /* this is undefined if q - p > PTRDIFF_MAX */
        ptrdiff_t l = q - p;
        memcpy(r, p, l);
        r += l;
        memcpy(r, new, newlen);
        r += newlen;
    }
    strcpy(r, p);

    return ret;
}

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

/*
*  defaultParms - initialize global variables
*/
defaultParms()
{
    Debug = FALSE;
    IgnoreHeader = FALSE;
    InputCorners = FALSE;

    NumDataLines = 0;
    Scale = 1.0;
    Z = 0.0;
    Margin = 0.1;

    /**** Afghanistan ****
    SW_lat = 33.955454;
    SW_lon = 69.837091;
    NE_lat = 34.764398;
    NE_lon = 71.503645;
    ********************/

    SW_lat_in = 0.0;
    SW_lon_in = 0.0;
    NE_lat_in = 0.0;
    NE_lon_in = 0.0;

    strcpy(DataFile, "data_in.csv");
    strcpy(GlyphFile, "glyph_in.csv");
    strcpy(TagsFile, "ANTzTag0001.csv");
    strcpy(TextFile, "corners.txt");
    strcpy(CornersTemplateFile, "corners_template.csv");
    strcpy(CornersFile, "corners.csv");

} /* end defaultParms() */

/*
*  parseArgs - parse arguments into global variables
*/
parseArgs(argc, argv)
int argc;
char** argv;
{
    int i;

    for (i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'D': Debug = TRUE;
                break;
            case 'c': InputCorners = TRUE;
                SW_lat_in = atof(&argv[++i][0]);
                SW_lon_in = atof(&argv[++i][0]);
                NE_lat_in = atof(&argv[++i][0]);
                NE_lon_in = atof(&argv[++i][0]);
                break;
            case 'C': strcpy(CornersTemplateFile, &argv[++i][0]);
                strcpy(CornersFile, &argv[++i][0]);
                break;
            case 'd': strcpy(DataFile, &argv[++i][0]);
                break;
            case 'g': NumGlyphLines = atoi(&argv[++i][0]);
                break;
            case 'G': strcpy(GlyphFile, &argv[++i][0]);
                break;
            case 'h': help(argv);
                exit(0);
                break;
            case 'i': IgnoreHeader = TRUE;
                break;
            case 'k': Kludge = TRUE;
                break;
            case 'm': Margin = atof(&argv[++i][0]);
                break;
            case 'p': PrintCorners = TRUE;
                break;
            case 'n': NumDataLines = atoi(&argv[++i][0]);
                break;
            case 's': Scale = atof(&argv[++i][0]);
                break;
            case 'T': strcpy(TagsFile, &argv[++i][0]);
                break;
            case 'X': strcpy(TextFile, &argv[++i][0]);
                break;
            case 'z': Z = atof(&argv[++i][0]);
                break;
            case 'u': usage(argv);
                exit(0);
            default:   fprintf(stderr, "%s: ERROR: illegal flag: %s\n",
                ProgName, argv[i]);
                exit(0);
                break;
            } /* end switch */
        } /* end if */
    } /* end for(i) */
} /* end parseArgs() */

/*
* getl - modified from Kernighan & Ritchie to take stream
*/
int
getl(stream, s, lim)            /* get line into s, return length */
FILE *stream;
char s[];
int lim;
{
    int c, i;

    i = 0;
    while (--lim > 0 && (c = getc(stream)) != EOF && c != '\n')
        s[i++] = c;
    if (c == '\n')
        s[i++] = c;
    s[i] = EOS;
    return(i);
} /* end getl() */

/*
* lines_in_file - from Stacktrace
*/
int
lines_in_file(filename)
char filename[];
{
    FILE                *fp;            /* */
    int                 c;              /* Nb. int (not char) for the EOF */
    unsigned long       newline_count = 0;

    /* count the newline characters */

    fp = fopen(filename, "r");

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            newline_count++;
        }
    }

    fclose(fp);
    return newline_count;

} /* end lines_in_file() */

main(argc, argv)
int argc;
char** argv;
{
    /* declare functions */

    void read_glyph();
    int write_glyph();
    char *replace();

    /* declare local variables */

    int id, row, len, stripped_len;
    int i, g, old_id, v, num_header_lines, template_lines, tl;

    int adl; /* short version of ActualNumDataLines */

    int atEOF;          /* Boolean */
    int gotAGoodHeader; /* Boolean */
    int gotAGoodData;   /* Boolean */

    double x_width, new_x_width;
    double y_width, new_y_width;
    double northeast_lon, southwest_lon, northeast_lat, southwest_lat;

    double xc, yc;

    double lat_min, lat_max, lon_min, lon_max;
    double lat_denom, lon_denom;

    char junk[MAX_LINE];
    char input_data_line[MAX_LINE];
    char *temp_data_line;
    char kml_line[MAX_LINE];
    char *new_kml_line0 = NULL;
    char *new_kml_line1 = NULL;
    char *new_kml_line2 = NULL;
    char *new_kml_line3 = NULL;

    char header_line[MAX_GLYPH_LINE];

    char sw_lat_str[MAX_LINE];
    char sw_lon_str[MAX_LINE];
    char ne_lat_str[MAX_LINE];
    char ne_lon_str[MAX_LINE];

    FILE *t_fp; /* tags */
    FILE *d_fp; /* data */
    FILE *g_fp; /* glyphs */
    FILE *c_fp; /* corners */
    FILE *k_fp; /* kml */
    FILE *x_fp; /* text */

    /* initialize global variables (including parameters) */

    strcpy(ProgName, argv[0]);

    defaultParms();
    parseArgs(argc, argv);

    SW_lat = 0.0;
    SW_lon = 0.0;
    NE_lat = 0.0;
    NE_lon = 0.0;

    if (NumDataLines == 0) {
        NumDataLines = lines_in_file(DataFile);
        if (Kludge) {
            NumDataLines = NumDataLines - 1;
        }
    }

    if (NumGlyphLines == 0) {
        NumGlyphLines = lines_in_file(GlyphFile);
    }

    if (Debug != FALSE) {
        fprintf(stderr, "%s: DEBUG: ARGS\n", ProgName);
        fprintf(stderr, "%s: DEBUG: Debug = %d\n", ProgName, Debug);
        fprintf(stderr, "%s: DEBUG: NumDataLines = %d\n", ProgName, NumDataLines);
        fprintf(stderr, "%s: DEBUG: GlyphFile = %s\n", ProgName, GlyphFile);
        fprintf(stderr, "%s: DEBUG: IgnoreHeader = %d\n", ProgName,
            IgnoreHeader);
        fprintf(stderr, "%s: DEBUG: Kludge = %d\n", ProgName, Kludge);
        fprintf(stderr, "%s: DEBUG: PrintCorners = %d\n", ProgName,
            PrintCorners);
        fprintf(stderr, "%s: DEBUG: InputCorners = %d\n", ProgName,
            InputCorners);
        fprintf(stderr, "%s: DEBUG: Margin = %f\n", ProgName, Margin);
        fprintf(stderr, "%s: DEBUG: DataFile = %s\n", ProgName, DataFile);
        fprintf(stderr, "%s: DEBUG: NumGlyphLines = %d\n", ProgName,
            NumGlyphLines);
        fprintf(stderr, "%s: DEBUG: Scale = %f\n", ProgName, Scale);
        fprintf(stderr, "%s: DEBUG: Z = %f\n", ProgName, Z);
    }

    /* initialize local variables */

    t_fp = fopen(TagsFile, "w");
    if (t_fp == NULL)  {
        fprintf(stderr, "%s: ERROR: fopen of %s failed, error: %s\n",
            ProgName, TagsFile, strerror(errno));
        exit(0);
    }

    /* read glyph file */

    read_glyph(GlyphFile, NumGlyphLines);

    if (Debug != FALSE) {
        fprintf(stderr, "%s: DEBUG: FirstGoodLine = %d\n",
            ProgName, FirstGoodLine);
    }

    /* ok? */

    if (Glyph_array[PARENT_ID_4][FirstGoodLine] != 0.0) {
        fprintf(stderr,
            "%s: ERROR: Glyph_array[PARENT_ID_4][%d] = %f, should be 0.0\n",
            ProgName, FirstGoodLine, Glyph_array[PARENT_ID_4][FirstGoodLine]);
        exit(0);
    }

    /* read data in gps file */

    /* sample row:                     */
    /* 70.77356,34.61004,0,,Bar Nurgal */

    d_fp = fopen(DataFile, "r");
    if (d_fp == NULL)  {
        fprintf(stderr, "%s: ERROR: fopen of %s failed, error: %s\n",
            ProgName, DataFile, strerror(errno));
        exit(0);
    }

    /* throw away header */

    gotAGoodHeader = FALSE;
    atEOF = FALSE;

    while (!atEOF && !gotAGoodHeader) {
        len = getl(d_fp, junk, MAX_LINE);

        if (Debug) {
            fprintf(stderr, "%s: DEBUG: raw header junk:>%s<\n",
                ProgName, junk);
        }

        /* strip carriage returns (\r) (^M) */

        temp_data_line = replace(junk, "\r", "");
        strcpy(junk, temp_data_line);
        stripped_len = strlen(junk);

        if (Debug) {
            fprintf(stderr,
                "%s: DEBUG: stripped_len: %d, processed header junk:>%s<\n",
                ProgName, stripped_len, junk);
        }

        if (stripped_len <= 1) { /* blank line or eof */
            if (feof(d_fp)) {
                fprintf(stderr,
                    "%s: ERROR: no header in data file\n", ProgName);
                exit(0);
            }
            else { /* do it again */
                if (Debug) {
                    fprintf(stderr,
                        "%s: DEBUG: got a blank line in data header\n",
                        ProgName);
                }
            }
        }
        else {
            gotAGoodHeader = TRUE;
        }
    }

    /* read data rows */ /* TODO: make NumDataLines long int */

    ActualNumDataLines = 0;

    for (row = 1; row < NumDataLines; row++) { /* we skipped the header line */

        if (Debug != FALSE) {
            fprintf(stderr,
                "%s: DEBUG: ================ top of loop: row: %d\n",
                ProgName, row);
        }

        /* pull data into string and parse there */

        gotAGoodData = FALSE;

        while (!atEOF && !gotAGoodData) {
            len = getl(d_fp, input_data_line, MAX_LINE);
            if (Debug) {
                fprintf(stderr, "%s: DEBUG: raw input_data_line:>%s<\n",
                    ProgName, input_data_line);
            }

            /* strip carriage returns (\r) (^M) */

            temp_data_line = replace(input_data_line, "\r", "");
            strcpy(input_data_line, temp_data_line);

            if (Debug) {
                fprintf(stderr, "%s: DEBUG: processed input_data_line:>%s<\n",
                    ProgName, input_data_line);
            }

            stripped_len = strlen(input_data_line);
            if (Debug) {
                fprintf(stderr,
                    "%s: DEBUG: stripped_len = %d\n",
                    ProgName, stripped_len);
            }

            gotAGoodData = TRUE; /* true faith */

            if (stripped_len <= 1) { /* blank line or eof */
                gotAGoodData = FALSE; /* either way we don't parse */
                if (feof(d_fp)) {
                    atEOF = TRUE;
                }
                else { /* do it again */
                    if (Debug) {
                        fprintf(stderr,
                            "%s: DEBUG: got a blank line in data body\n",
                            ProgName);
                    }
                }
            }
            else {
                ActualNumDataLines++;
            }
        }

        if (gotAGoodData && !atEOF) {

            adl = ActualNumDataLines;
            sscanf(input_data_line,
                "%lf,%lf,%lf,%[^\n]s\n", &Lon[adl], &Lat[adl], &Ele[adl],
                &Tags[adl][0]);
            if (Debug != FALSE) {
                fprintf(stderr,
                    "%s: DEBUG: ---- DATA lINE ----\n", ProgName);
                fprintf(stderr,
                    "%s: DEBUG: final input_data_line: >%s<\n",
                    ProgName, input_data_line);
                fprintf(stderr,
                    "%s: DEBUG: Lat[%d] = %lf\n", ProgName, adl, Lat[adl]);
                fprintf(stderr,
                    "%s: DEBUG: Lon[%d] = %lf\n", ProgName, adl, Lon[adl]);
                fprintf(stderr,
                    "%s: DEBUG: Ele[%d] = %lf\n", ProgName, adl, Ele[adl]);
                fprintf(stderr,
                    "%s: DEBUG: Tags[%d] = %s\n", ProgName, adl, &Tags[adl][0]);
            }
            if (adl == 1) {
                lat_max = Lat[adl];
                lat_min = Lat[adl];
                lon_max = Lon[adl];
                lon_min = Lon[adl];
                if (Debug != FALSE) {
                    fprintf(stderr, "%s: DEBUG: init OK\n", ProgName, lat_min);
                }
            }
            if (row > 1 && Lat[adl] > lat_max) {
                lat_max = Lat[adl];
                if (Debug != FALSE) {
                    fprintf(stderr, "%s: DEBUG: lat_max now: %lf\n", ProgName,
                        lat_max);
                }
            }
            if (adl > 1 && Lat[adl] < lat_min) {
                lat_min = Lat[adl];
                if (Debug != FALSE) {
                    fprintf(stderr, "%s: DEBUG: lat_min now: %lf\n", ProgName,
                        lat_min);
                }
            }
            if (adl > 1 && Lon[adl] > lon_max) {
                lon_max = Lon[adl];
                if (Debug != FALSE) {
                    fprintf(stderr, "%s: DEBUG: lon_max now: %lf\n", ProgName,
                        lon_max);
                }
            }
            if (adl > 1 && Lon[adl] < lon_min) {
                lon_min = Lon[adl];
                if (Debug != FALSE) {
                    fprintf(stderr, "%s: DEBUG: lon_min now: %lf\n", ProgName,
                        lon_min);
                }
            }
        }

    } /* end for row */

    if (Debug != FALSE) {
        fprintf(stderr, "%s: DEBUG: final ActualNumDataLines: %d\n", ProgName,
            ActualNumDataLines);
    }

    /* compute corners */

    /* Remember kids: x is lon and y is lat! */

    /* map from min/max to NSEW */

    southwest_lat = lat_min;
    southwest_lon = lon_min;
    northeast_lat = lat_max;
    northeast_lon = lon_max;

    x_width = northeast_lon - southwest_lon;
    y_width = northeast_lat - southwest_lat;

    if (x_width / y_width == 2.0) {
        new_x_width = x_width*(1.0 + (double)Margin);
        new_y_width = y_width*(1.0 + (double)Margin);
    }
    if (x_width / y_width >  2.0) {
        /* make y bigger */
        new_x_width = x_width*(1.0 + (double)Margin);
        new_y_width = new_x_width / 2.0;
    }
    if (x_width / y_width <  2.0) {
        /* make x bigger */
        new_y_width = y_width*(1.0 + (double)Margin);
        new_x_width = 2.0*new_y_width;
    }

    /* Finally compute center of original extents */

    xc = (northeast_lon + southwest_lon) / 2.0;
    yc = (northeast_lat + southwest_lat) / 2.0;

    /* compute final extents: */

    NE_lon = xc + new_x_width / 2.0;
    SW_lon = xc - new_x_width / 2.0;
    NE_lat = yc + new_y_width / 2.0;
    SW_lat = yc - new_y_width / 2.0;

    if (PrintCorners) {

        /* txt */

        x_fp = fopen(TextFile, "w");
        if (x_fp == NULL)  {
            fprintf(stderr, "%s: ERROR: fopen of %s failed, error: %s\n",
                ProgName, "corners.txt", strerror(errno));
            exit(0);
        }

        fprintf(x_fp, "old SW corner lat (min) = %lf\r\n", lat_min);
        fprintf(x_fp, "old SW corner lon (min) = %lf\r\n", lon_min);
        fprintf(x_fp, "old NE corner lat (max) = %lf\r\n", lat_max);
        fprintf(x_fp, "old NE corner lon (max) = %lf\r\n", lon_max);
        fprintf(x_fp, "new SW corner lat = %lf\r\n", SW_lat);
        fprintf(x_fp, "new SW corner lon = %lf\r\n", SW_lon);
        fprintf(x_fp, "new NE corner lat = %lf\r\n", NE_lat);
        fprintf(x_fp, "new NE corner lon = %lf\r\n", NE_lon);
        fclose(x_fp);

        /* kml */

        sprintf(sw_lat_str, "%.8lf", SW_lat_in);
        sprintf(sw_lon_str, "%.8lf", SW_lon_in);
        sprintf(ne_lat_str, "%.8lf", NE_lat_in);
        sprintf(ne_lon_str, "%.8lf", NE_lon_in);

        template_lines = lines_in_file("corners_template.kml");

        c_fp = fopen("corners_template.kml", "r");
        if (c_fp == NULL)  {
            fprintf(stderr, "%s: ERROR: fopen of %s failed, error: %s\n",
                ProgName, "corners_template.txt", strerror(errno));
            exit(0);
        }

        k_fp = fopen("corners.xml", "w");
        if (k_fp == NULL)  {
            fprintf(stderr, "%s: ERROR: fopen of %s failed, error: %s\n",
                ProgName, "corners.kml", strerror(errno));
            exit(0);
        }

        for (tl = 0; tl < template_lines; tl++) {
            getl(c_fp, kml_line, MAX_LINE);
            new_kml_line0 = replace(kml_line, "_SW_lon_", sw_lon_str);
            new_kml_line1 = replace(new_kml_line0, "_SW_lat_", sw_lat_str);
            new_kml_line2 = replace(new_kml_line1, "_NE_lon_", ne_lon_str);
            new_kml_line3 = replace(new_kml_line2, "_NE_lat_", ne_lat_str);
            fprintf(k_fp, "%s", new_kml_line3);
        }

        fclose(c_fp);
        fclose(k_fp);
    }

    /* over-write what we just computed and printed out */

    if (InputCorners) {
        NE_lat = NE_lat_in;
        NE_lon = NE_lon_in;
        SW_lat = SW_lat_in;
        SW_lon = SW_lon_in;
    }

    /* compute Newlat & NewLon */

    for (row = 1; row <= ActualNumDataLines; row++) {

        /* we skipped the header line */

        if (Debug != FALSE) {
            fprintf(stderr, "%s: DEBUG: 2nd loop, row: %d\n", ProgName, row);
        }

        if (SW_lat != NE_lat && SW_lon != NE_lon) {

            /* convert Lat/Lon to map coordinates */

            lat_denom = NE_lat - SW_lat;
            lon_denom = NE_lon - SW_lon;

            NewLat[row] = 180.0*(Lat[row] - SW_lat) / lat_denom - 90.0;
            NewLon[row] = 360.0*(Lon[row] - SW_lon) / lon_denom - 180.0;
        }
        else {
            NewLat[row] = Lat[row];
            NewLon[row] = Lon[row];
        }

        if (Debug != FALSE) {
            fprintf(stderr, "%s: DEBUG: NewLat[row] = %lf\n",
                ProgName, NewLat[row]);
            fprintf(stderr, "%s: DEBUG: NewLon[row] = %lf\n",
                ProgName, NewLon[row]);
            fprintf(stderr, "%s: DEBUG: lat_denom = %lf\n",
                ProgName, lat_denom);
            fprintf(stderr, "%s: DEBUG: lon_denom = %lf\n",
                ProgName, lon_denom);
        }

    } /* end for row */

    if (Debug) {
        fprintf(stderr, "%s: DEBUG: lat_min = %lf\n", ProgName, lat_min);
        fprintf(stderr, "%s: DEBUG: lat_max = %lf\n", ProgName, lat_max);
        fprintf(stderr, "%s: DEBUG: lon_min = %lf\n", ProgName, lon_min);
        fprintf(stderr, "%s: DEBUG: lon_max = %lf\n", ProgName, lon_max);
    }

    /* process & write */

    /* write headers */

    fprintf(t_fp, "id,record_id,table_id,title,description\n");

    if (!IgnoreHeader) {
        num_header_lines = FirstGoodLine;
        g_fp = fopen(GlyphFile, "r");
        for (row = 0; row < num_header_lines; row++) {
            getl(g_fp, header_line, MAX_GLYPH_LINE);
            fprintf(stdout, "%s", header_line);
        }
        fclose(g_fp);
    }

    i = 0;
    id = 300;
    for (row = 0; row < ActualNumDataLines; row++) {

        if (Debug != FALSE) {
            fprintf(stderr, "%s: DEBUG: about to write row %d\n", ProgName, row);
        }

        /* Glyph_array indexed from 0, NewLon indexed from 1 */

        Glyph_array[SELECTED_3][FirstGoodLine] = 0.0;
        Glyph_array[SELECTED_3][FirstGoodLine + 1] = 0.0; /* KLUDGE */
        Glyph_array[SCALE_X_25][FirstGoodLine] = Scale;
        Glyph_array[SCALE_Y_26][FirstGoodLine] = Scale;
        Glyph_array[SCALE_Z_27][FirstGoodLine] = Scale;
        Glyph_array[TRANSLATE_X_28][FirstGoodLine] = (float)NewLon[row + 1];
        Glyph_array[TRANSLATE_Y_29][FirstGoodLine] = (float)NewLat[row + 1];
        Glyph_array[TRANSLATE_Z_30][FirstGoodLine] = Z;

        old_id = id;
        id = write_glyph(NumGlyphLines, id);

        if (Debug != FALSE) {
            fprintf(stderr, "%s: DEBUG: at row %d got back id = %d\n", ProgName,
                row, id);
        }

        /* write tags */

        v = 0;
        for (g = old_id; g < id; g++) {
            if (g != old_id) {
                fprintf(t_fp, "%d,%d,0,\"id: %d\",\" \"\n",
                    i++, g, g);
            }
            else if (Tags[row + 1][0] == '\"') {
                fprintf(t_fp, "%d,%d,0,%s,\" \"\n",
                    i++, g, Tags[row + 1]);
            }
            else {
                fprintf(t_fp, "%d,%d,0,\"%s\",\" \"\n",
                    i++, g, Tags[row + 1]);
            }
            /* tags start at zero, data starts at one */
        }
    } /* end for */

    if (Debug != FALSE) {
        fprintf(stderr, "%s: DEBUG: id = %d\n", ProgName, id);
    }

} /* end main */