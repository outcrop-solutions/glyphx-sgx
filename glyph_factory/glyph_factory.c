/**
 **   glyph_factory -  use a bunch of files to convert data
 ** 
 **  CREATED: 2014.04.12  ABS  copied from xyz2csv
 ** MODIFIED: 2014.04.13  ABS  debugged
 ** MODIFIED: 2014.04.14  ABS  debugged column counting
 ** MODIFIED: 2014.04.15  ABS  debugged
 ** MODIFIED: 2014.04.19  ABS  added infinite tags
 ** MODIFIED: 2014.04.23  ABS  debugged for demo
 **
 **/

#include        "glyph_factory.h"

main(argc, argv)
int argc;
char** argv;
{
    /* declare functions */

    void read_glyph();
    int write_glyph();

    /* declare local variables */

    int id, row, current_data_row, current_data_col, current_glyph_row;
    int current_glyph_diff_row, old_id, v, s, tags_row, data_col;
    int gr, gc, intDbl;

    int col_in_q, row_in_q; /* "column in question" & "row in question" */
    int col_in, type_in, id_counter;
    double col_min, col_max;
    double normalized, data_val;
    int glyph_row_col2mapper_line[MAX_GLYPH_ROWS][NUM_GLYPH_COLS];
    char col_tag[MAX_LINE];
    char junk[MAX_LINE];

    int num_lines, n, current_mapper_row, gl;

    int num_data_stats_lines;
    int num_glyph_diff_lines;
    int num_mapper_lines;
    int num_types_lines;

    double dblArray[MAX_COLS]; /* one row of input data */

    FILE *tg_fp; /* tags file */
    FILE *ds_fp; /* data stats file */
    FILE *gd_fp; /* glyph diffs file */
    FILE *m_fp;  /* mapper file */
    FILE *ty_fp; /* types file */

    /* initialize global variables (including parameters) */

    defaultParms();
    parseArgs(argc, argv);

    for(data_col = 1; data_col <= NumDataColumns; data_col++) {
        strcpy(&ColNames[data_col][0], "empty_col_name");
        }

    if (Debug) {
        fprintf(stderr, "%s: DEBUG: Debug = %d\n", argv[0], Debug);
        fprintf(stderr, "%s: DEBUG: NumDataLines = %d\n", argv[0], NumDataLines);
        fprintf(stderr, "%s: DEBUG: NumDataColumns = %d\n", argv[0], NumDataColumns);
        fprintf(stderr, "%s: DEBUG: GlyphFile = %s\n", argv[0], GlyphFile);
        fprintf(stderr, "%s: DEBUG: NumGlyphLines = %d\n", argv[0], NumGlyphLines);
        fprintf(stderr, "%s: DEBUG: Decimate = %d\n", argv[0], Decimate);
        fprintf(stderr, "%s: DEBUG: Scale = %f\n", argv[0], Scale);
        fprintf(stderr, "%s: DEBUG: StartID = %d\n", argv[0], StartID);
        fprintf(stderr, "%s: DEBUG: TagsFlag = %d\n", argv[0], TagsFlag);
        fprintf(stderr, "%s: DEBUG: Without = %d\n", argv[0], Without);
        fprintf(stderr, "%s: DEBUG: ZScale = %f\n", argv[0], ZScale);

        for(data_col = 1; data_col <= NumDataColumns; data_col++) {
            fprintf(stderr, "%s: DEBUG: ColNames[%d] = %s\n", argv[0],
              data_col, &ColNames[data_col][0]);
            }
        }

    if (NumGlyphLines > MAX_GLYPH_ROWS) {
        fprintf(stderr, "%s: DEBUG: NumGlyphLines must be <= %d\n", argv[0],
          MAX_GLYPH_ROWS);
        }
    if (NumDataColumns > MAX_COLS) {
        fprintf(stderr, "%s: DEBUG: NumDataColumns must be <= %d\n", argv[0],
          MAX_COLS);
        }

    /* initialize local variables */

    for (gr = 0; gr < MAX_GLYPH_ROWS; gr++) {
        for (gc = 0; gc < NUM_GLYPH_COLS; gc++) {
            glyph_row_col2mapper_line[gr][gc] = FALSE;
            }
        }

    if (TagsFlag) {

        /* open tag file and write header */

        tg_fp  = fopen(TagsFile, "w");
        fprintf(tg_fp, "id,record_id,table_id,title,description\n");
        tags_row = 0;

        }

    /* read */

    /* The confusing thing about what we're doing here is that the input
     * data file and the template glyph file are both in Comm Separated Value
     * (CSV) form, and one line of input data will typically be associated
     * with many lines of glyph data.  There are inconsistencies in different
     * CSV tools in terms of how rows and columns are numbered.  In the
     * 'glyph_factory' suite as exposed to users through the command line
     * and input/output text files, row 0 is the text header, and data begins
     * with row 1.  Columns are numbered starting with 1.  In code internals
     * this may be different.
     */

    /* read template glyph file */

    read_glyph(GlyphFile, NumGlyphLines);

    /* ok? */

    if ( Glyph_array[PARENT_ID_4][FirstGoodLine] != 0.0) {
        fprintf(stderr,
          "%s: ERROR: Glyph_array[PARENT_ID_4][%d] = %f, should be 0.0\n",
          argv[0], FirstGoodLine, Glyph_array[PARENT_ID_4][FirstGoodLine]);
        exit(0);
        }

    /* one-time surgery */

    Glyph_array[SCALE_X_25][FirstGoodLine] = 
      Glyph_array[SCALE_X_25][FirstGoodLine]*Scale;
    Glyph_array[SCALE_Y_26][FirstGoodLine] = 
      Glyph_array[SCALE_Y_26][FirstGoodLine]*Scale;
    Glyph_array[SCALE_Z_27][FirstGoodLine] = 
      Glyph_array[SCALE_Z_27][FirstGoodLine]*Scale;

    /* read data stats file */

    num_lines = lines_in_file(DataStatsFile);
    NumStatLines = num_lines;

    ds_fp = fopen(DataStatsFile, "r");
    for (n = 0; n < num_lines; n++) {
        fscanf(ds_fp, "%d,%lf,%lf,%s\n", &col_in_q, &col_min, &col_max, &col_tag);
 
        /* n goes from 0 to number of lines in the stats file - 1 */

        DataStatsCol[n] = col_in_q;
        DataStatsMin[n] = col_min;
        DataStatsMax[n] = col_max;
        strcpy(&ColNames[col_in_q][0], col_tag);

        DataStatsRange[n] = DataStatsMax[n] - DataStatsMin[n];

        if (Debug) {
            fprintf(stderr,
        "%s: DEBUG: n:%d, col:%d, name:%s, min:%f, max:%f, tag:%s, range:%f\n",
              argv[0], n, DataStatsCol[n], col_tag, DataStatsMin[n],
              DataStatsMax[n], ColNames[DataStatsCol[n]], DataStatsRange[n]);
            }
        }
    fclose(ds_fp);

    /* read glyph diff file */

    num_lines = lines_in_file(GlyphDiffFile);
    NumGlyphDiffLines = num_lines;
    gd_fp = fopen(GlyphDiffFile, "r");

    for (n = 0; n < num_lines; n++) {
        fscanf(gd_fp, "%d,%d:%lf,%lf\n", &row_in_q, &col_in_q,
          &col_min, &col_max);
 
        /* n goes from 0 to number of lines in the glyph diff file - 1 */

        /* subtract the size of the glyph header to get useful row # */

        GlyphDiffRow[n] = row_in_q - FirstGoodLine;
        GlyphDiffCol[n] = col_in_q;
        GlyphDiffMin[n] = col_min;
        GlyphDiffMax[n] = col_max;
        GlyphDiffRange[n] = GlyphDiffMax[n] - GlyphDiffMin[n];

        if (Debug) {
            fprintf(stderr,
              "%s: DEBUG: n: %d, row: %d, col: %d, min: %f, max: %f\n",
              argv[0], n, row_in_q, col_in_q, col_min, col_max);
            }
        }
    fclose(gd_fp);

    /* read mapper file */

    NumMapperLines = lines_in_file(MapperFile);
    m_fp  = fopen(MapperFile, "r");

    for (n = 0; n < NumMapperLines; n++) {
        fscanf(m_fp, "%d,%d:%d\n", &row_in_q, &col_in_q,
          &col_in);
 
        /* n goes from 0 to number of lines in the mapper file - 1 */

        /* subtract the size of the glyph header to get useful row # */

        MapperGlyphRow[n] = row_in_q - FirstGoodLine;
        MapperGlyphCol[n] = col_in_q;
        MapperDataCIn[n] = col_in;    /* input column for this output row & col */

        /* for reverse lookup --  so far unused */

        glyph_row_col2mapper_line[MapperGlyphRow[n]][MapperGlyphCol[n]] = n;

        if (Debug) {
            fprintf(stderr,
              "%s: DEBUG: n: %d, row: %d, col: %d, col_in: %d\n",
              argv[0], n,  MapperGlyphRow[n], MapperGlyphCol[n], MapperDataCIn[n]);
            }
        }
    fclose(m_fp);

#ifdef TYPE
    /* read type file */

    /* this will be changed very rarely -- I think */

    /* note col 0 is first column here */

    num_lines = lines_in_file(TypeFile);
    ty_fp  = fopen(TypeFile, "r");

    for (n = 0; n < NUM_GLYPH_COLS - 1; n++) {
        fscanf(ty_fp, "%d,", &type_in);
 
        if (Debug) {
            fprintf(stderr,
              "%s: DEBUG: n: %d, type: %d\n",
              argv[0], n, type_in);
            }

        Types[n] = type_in;
        }
    fscanf(m_fp, "%f", &type_in); /* last one, no comma */
    Types[n] = type_in;
    fclose(ty_fp);
#endif

    /* read & process data file */

    id = StartID;

    /* throw away header */

    getl(stdin, junk, MAX_LINE);
    if (Debug) {
        fprintf(stderr, "%s: DEBUG: junk:%s\n",
          argv[0], junk);
        }
 
    /* loop while reading input data rows */

    /* it is important for this loop to be able to run virtually endlesly
       -- no storing row data between loops */

    for (current_data_row = 1; current_data_row < NumDataLines; current_data_row++) {

        /* start from scratch with strings */

        for (tags_row = 0; tags_row <= NumGlyphLines; tags_row++) {
            /* INSURANCE */
            strcpy(&Tags[tags_row][0], ".");
            TagsUsed[tags_row] = FALSE;
            }
        for (current_data_col = 1; current_data_col < NumDataColumns; current_data_col++) {
            fscanf(stdin, "%lf,", &dblArray[current_data_col]);
            if (Debug) {
                fprintf(stderr, "%s: DEBUG: dblArray[%d]:%f\n",
                  argv[0], current_data_col, dblArray[current_data_col]);
                }
            }

        /* last column, no comma */

        if (RowNames) {
            fscanf(stdin, "%s", &RowName[0]);
            }
        else {
            fscanf(stdin, "%f", &dblArray[current_data_col]);
            }

        /* process & write */

        /* special handling */

        for (gl = 0; gl < NumGlyphLines; gl++) {
            Glyph_array[SELECTED_3][FirstGoodLine + gl] = 0.0;
            }

        /* magic */

        /* this is very inefficient since the searchers are all done on
           each row -- better to have a data converter prepped with
           paramters already worked out for each column, and run a concise
           version of the algebra inside the loop */

        if (Debug) {
            fprintf(stderr,"===================");
            fprintf(stderr,"===================");
            fprintf(stderr,"===================");
            fprintf(stderr,"===================\n");
            }

        for (current_mapper_row = 0; current_mapper_row < NumMapperLines; current_mapper_row++) {
            for (current_data_col = 1; current_data_col <= NumDataColumns; current_data_col++) {
                if (MapperDataCIn[current_mapper_row] == current_data_col) {
                     /* find stats row */
                    for (s = 0; s < NumStatLines; s++) {
                        if (DataStatsCol[s] == current_data_col) {
                            /* s stats is the line that mentions column current_data_col */
                            for (current_glyph_diff_row = 0; current_glyph_diff_row < NumGlyphDiffLines; current_glyph_diff_row++) {
                                if (GlyphDiffRow[current_glyph_diff_row] == MapperGlyphRow[current_mapper_row]
                                      && GlyphDiffCol[current_glyph_diff_row] == MapperGlyphCol[current_mapper_row]) {
                                    /* g glyph_diff is the line that mentions
                                       column c */

                                    if (Debug) {
                                        fprintf(stderr,"-------------------");
                                        fprintf(stderr,"-------------------");
                                        fprintf(stderr,"-------------------");
                                        fprintf(stderr,"-------------------\n");
                                        fprintf(stderr,
                                        "%s: DEBUG: s:%d g:%d dblArray[current_data_col]:%f\n",
                                          argv[0], s, current_glyph_diff_row, dblArray[current_data_col]);
                                        }

                                    /* simplify */

                                    current_glyph_row
                                      = MapperGlyphRow[current_mapper_row];
                                    /* this shouldn't work without - 1 ??? */
        
                                    /* compute data */
                                    normalized
                                      = (dblArray[current_data_col]
                                      - DataStatsMin[s])/DataStatsRange[s];
                                    data_val
                                      = normalized*GlyphDiffRange[current_glyph_diff_row]
                                        + GlyphDiffMin[current_glyph_diff_row];
       
                                    if (Debug) {
                                        fprintf(stderr,
      "%s: DEBUG: current_mapper_row:%d current_data_col:%d norm:%f data:%f\n",
                                          argv[0], current_mapper_row,
                                          current_data_col,
                                          normalized, data_val);
                                        }
        
                                    /* perform surgery */

                                    /* in Glyph_array rows & cols start at 0 */

                                    /* I don't understand why this works */
                                    Glyph_array[MapperGlyphCol[current_mapper_row]
                                      - 1][FirstGoodLine
                                      + MapperGlyphRow[current_mapper_row]] = data_val;

                                    if (TagsFlag) {  /* KLUDGE BELOW */
                                        if
                                          (((MapperGlyphCol[current_mapper_row])                                          == Particular)
                                          && !TagsUsed[current_glyph_row]) {
                                            if (Debug) {
                                                fprintf(stderr,
                                              "%s: DEBUG: T: TagsUsed[%d]: %d\n",
                                                  argv[0], current_glyph_row,
                                                  TagsUsed[current_glyph_row]);
                                                }
                                            intDbl = (int)dblArray[current_data_col];
                                            if (dblArray[current_data_col]
                                              == (double)intDbl) {
                                                if (RowNames) {
                                                    sprintf(&Tags[current_glyph_row][0],
                                                      "%s - %s:%d",
                                                      RowName,
                                                      &ColNames[current_data_col][0],
                                                      intDbl);
                                                    TagsUsed[current_glyph_row] = TRUE;
                                                    }
                                                else {
                                                    sprintf(&Tags[current_glyph_row][0],
                                                      "data row %d column %d - %s:%d",
                                                      current_data_row, current_data_col,
                                                      &ColNames[current_data_col][0],
                                                      intDbl);
                                                    TagsUsed[current_glyph_row] = TRUE;
                                                    }
                                                }
                                            else {
                                                if (RowNames) {
                                                    sprintf(&Tags[current_glyph_row][0],
                                                      "%s - %s:%f",
                                                      RowName,
                                                      &ColNames[current_data_col][0],
                                                      dblArray[current_data_col]);
                                                    TagsUsed[current_glyph_row] = TRUE;
                                                    }
                                                else {
                                                    sprintf(&Tags[current_glyph_row][0],
                                                      "data row %d column %d - %s:%f",
                                                      current_data_row, current_data_col,
                                                      &ColNames[current_data_col][0],
                                                      dblArray[current_data_col]);
                                                    TagsUsed[current_glyph_row] = TRUE;
                                                    }
                                                }
                                            }
                                        else if (!TagsUsed[current_glyph_row]) {
                                            if (Debug) {
                                                fprintf(stderr,
                                              "%s: DEBUG: F: TagsUsed[%d]: %d\n",
                                                  argv[0], current_glyph_row,
                                                  TagsUsed[current_glyph_row]);
                                                }
                                            sprintf(&Tags[current_glyph_row][0],
                                              "data row %d",
                                              current_data_row);
                                            }
                                        }

                                    if (Debug) {
                                        fprintf(stderr,
                    "%s: DEBUG: MapperGlyphCol[current_mapper_row] - 1: %d\n",
                                          argv[0],
                                          MapperGlyphCol[current_mapper_row]
                                          - 1);
                                        fprintf(stderr,
                        "%s: DEBUG: MapperGlyphRow[current_mapper_row]: %d\n",
                                          argv[0],
                                          MapperGlyphRow[current_mapper_row]);
                                        fprintf(stderr,
                                          "%s: DEBUG: Tags[%d] = %s\n",
                                          argv[0], current_glyph_row,
                                          &Tags[current_glyph_row][0]);
                                        }
        
                                    } /* end if*/
                                } /* end for g */
                            } /* end if */
                        } /* end for s */
                    } /* end if */
                } /* end for c */
            } /* end for m */

        old_id = id;
        id = write_glyph(NumGlyphLines, id);

        if (Debug) {
            fprintf(stderr, "%s: DEBUG: id = %d\n", argv[0], id);
            }

        if (TagsFlag) {

            /* write tags */

            if (Debug) {
                fprintf(stderr, "%s: DEBUG: about to write some tags\n",
                  argv[0]);
                for (v = 0; v < NumGlyphLines; v++) {
                    fprintf(stderr, "%s: DEBUG: Tags[%d] = %s\n",
                      argv[0], v,  Tags[v]);
                    }
                }

            v = 0;
            for (id_counter = old_id; id_counter < id; id_counter++) {
                if (v < Without) {
                    fprintf(tg_fp, "%d,%d,0,\"%s\",\"unused\"\n",
                      tags_row++, id_counter, RowName);
                    }
                else {
                    fprintf(tg_fp, "%d,%d,0,\"%s\",\"unused\"\n",
                      tags_row++, id_counter, Tags[v]);
                    }
                v++;
                } 
            } 

        } /* end for current_data_row */

    } /* end main */

/*
 *  defaultParms - initialize global variables
 */
defaultParms()
{
    NumDataLines = 1;
    NumGlyphLines = 1;
    NumDataColumns = 1;
    Particular = SCALE_X_25 + 1;
    RowNames = FALSE;
    StartID = 300;
    Decimate = 0;
    Scale = 1.0;
    TagsFlag = FALSE;
    Without = 1;
    ZScale = 1.0;

    strcpy(GlyphFile, "glyph_in.csv");
    strcpy(DataStatsFile, "data_stats.txt");
    strcpy(MapperFile, "mapper.txt");
    strcpy(GlyphDiffFile, "glyph_diff.txt");
    strcpy(TagsFile, "ANTzTag0001.csv");
    strcpy(TypeFile, "types.txt");

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
                case 'D' : Debug = TRUE;
                           break;
                case 'c' : NumDataColumns = atoi(&argv[++i][0]);
                           break;
                case 'd' : Decimate = atoi(&argv[++i][0]);
                           break;
                case 'g' : NumGlyphLines = atoi(&argv[++i][0]);
                           break;
                case 'i' : StartID = atoi(&argv[++i][0]);
                           break;
                case 'G' : strcpy(GlyphFile, &argv[++i][0]);
                           break;
                case 'h' : help(argv);
                           exit(0);
                           break;
                case 'n' : NumDataLines = atoi(&argv[++i][0]);
                           break;
                case 'p' : Particular = atoi(&argv[++i][0]);
                           break;
                case 'r' : RowNames = TRUE;
                           break;
                case 's' : Scale = atof(&argv[++i][0]);
                           break;
                case 't' : TagsFlag = TRUE;
                           break;
                case 'T' : strcpy(TagsFile, &argv[++i][0]);
                           break;
                case 'u' : usage(argv);
                           exit(0);
                           break;
                case 'w' : Without = atoi(&argv[++i][0]);
                           break;
                case 'z' : ZScale = atof(&argv[++i][0]);
                           break;
                default:   fprintf(stderr, "%s: ERROR: illegal flag: %s\n",
                               argv[0], argv[i]);
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

    while ( (c=fgetc(fp)) != EOF ) {
        if ( c == '\n' ) {
            newline_count++;
            }
        }

    fclose(fp);
    return newline_count;

    } /* end lines_in_file() */
