/*
 * glyph_factory.h -- include file for glyph_factory.c
 */

/* includes */

#include        "synglyphx.h"

/* constants */

#define MAX_CODE          (5)
#define MAX_NODE_NUM      (500)
#define STRING_EQUAL      (0)
#define MAX_COLS          (512)
#define ERROR_CODE        (-1)
#define NO_MATCH          (-1)
#define UNIVERSAL_PARENT  (0)
#define START_ID          (33)
#define NUM_GLYPH_COLS    (95)

#define CIRCLE_DEG      (360.0)

/* global variables */

char Tags[MAX_GLYPH_ROWS][MAX_LINE]; /* tags */

int DataStatsCol[MAX_COLS];
double DataStatsMin[MAX_COLS];
double DataStatsMax[MAX_COLS];
double DataStatsRange[MAX_COLS];
char ColNames[MAX_COLS][MAX_LINE]; /* for tags */
char RowName[MAX_LINE];            /* for tags */

int GlyphDiffRow[MAX_GLYPH_ROWS];
int GlyphDiffCol[MAX_GLYPH_ROWS];
double GlyphDiffMin[MAX_GLYPH_ROWS];
double GlyphDiffMax[MAX_GLYPH_ROWS];
double GlyphDiffRange[MAX_GLYPH_ROWS];

int NumMapperLines;
int NumStatLines;
int NumGlyphDiffLines;
int MapperGlyphRow[MAX_COLS];
int MapperGlyphCol[MAX_COLS];
int MapperDataCIn[MAX_COLS];

int TagsUsed[MAX_GLYPH_ROWS];
int Types[NUM_GLYPH_COLS];

/* flag-settable parameters (global variables) */

int Debug;                     /* Boolean: TRUE = debug writes */
int Decimate;                  /* */
float Scale;                   /* */
float ZScale;                  /* */
int NumDataLines;              /* */
int NumDataColumns;            /* */
int NumGlyphLines;             /* */
int NumMapperLines;            /* */
int Particular;                /* */
int RowNames;                  /* Boolean */
int StartID;                   /* */
int TagsFlag;                  /* Boolean */
char TagsFile[MAX_LINE];       /* tags file name */
char DataStatsFile[MAX_LINE];  /* */
char MapperFile[MAX_LINE];     /* */
char GlyphDiffFile[MAX_LINE];  /* */
char TypeFile[MAX_LINE];       /* */
int Without;                   /* */

/* macros */

