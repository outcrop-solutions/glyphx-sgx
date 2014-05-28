/*
 * gps2csv.h -- include file for gps2csv.c
 */

/* includes */

#include        <stdio.h>
#include        <ctype.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <errno.h>

#include        "synglyphx.h"

/* constants */

#define TRUE              (1)
#define FALSE             (0)
#define MAX_LINE          (255)
#define MAX_GLYPH_LINE    (4096)
#define NUM_COLUMNS       (94)
#define MAX_CODE          (5)
#define MAX_NODE_NUM      (500)
#define EOS               (0)
#define STRING_EQUAL      (0)
#define MAX_ROWS          (1600000)
#define MAX_GLYPH_ROWS    (400)
#define ERROR_CODE        (-1)
#define NO_MATCH          (-1)
#define UNIVERSAL_PARENT  (0)
#define START_ID          (33)

#define CIRCLE_DEG      (360.0)

/* global variables */

int ActualNumDataLines;                    /* */

double Lat[MAX_ROWS];                      /* latitude (NS) */
double Lon[MAX_ROWS];                      /* longitude (EW) */
double Ele[MAX_ROWS];                      /* elevation */
double NewLat[MAX_ROWS];                   /* adjusted latitude (NS) */
double NewLon[MAX_ROWS];                   /* adjusted longitude (EW) */
double NE_lat;                             /* NE map corner lat */
double NE_lon;                             /* NE map corner lon */
double SW_lat;                             /* SW map corner lat */
double SW_lon;                             /* SW map corner lon */

int InputCorners;                          /* Boolean */

char Tags[MAX_ROWS][MAX_LINE];             /* tags */
char ProgName[MAX_LINE];                   /* */

/* used by glyph_tools */

int FirstGoodLine;
int First_id;
float Glyph_array[NUM_COLUMNS][MAX_GLYPH_ROWS];

/* flag-settable parameters (global variables) */

int Debug;                     /* Boolean: TRUE = debug writes */
int IgnoreHeader;              /* Boolean: TRUE = don't add header */
int Kludge;                    /* Boolean: TRUE = ignore last input line */
int PrintCorners;              /* Boolean: TRUE = don't add header */
float Scale;                   /* */
float Margin;                  /* */
float Z;                       /* */
int NumDataLines;              /* */
int NumGlyphLines;             /* */
char GlyphFile[MAX_LINE];      /* glyph file name */
char DataFile[MAX_LINE];       /* data file name */
char TagsFile[MAX_LINE];       /* tags file name */
char TextFile[MAX_LINE];       /* text file name */
char CornersFile[MAX_LINE];    /* corners file name */
char CornersTemplateFile[MAX_LINE];    /* corners template file name */

double NE_lat_in;                          /* NE map corner lat */
double NE_lon_in;                          /* NE map corner lon */
double SW_lat_in;                          /* SW map corner lat */
double SW_lon_in;                          /* SW map corner lon */

/* global variables */

/* macros */

