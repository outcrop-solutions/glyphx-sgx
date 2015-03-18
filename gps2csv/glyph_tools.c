/**
** glyph_tools: contains:
**
**   read_glyph()
**   write_glyph()
**/

#include        "gps2csv.h"

/*
* read_glyph - read from a CSV file into Glyph_array[][]
*/
void read_glyph(filename, num_lines)
char * filename;
int num_lines;
{

    /* declare local variables */

    int len, col, i_col, d_col, line_num;

    char temp_char;
    char input_line[MAX_GLYPH_LINE];
    char data_array[MAX_GLYPH_LINE];

    FILE* g_fp;

    /* init local variables */

    if (Debug != FALSE) {
        fprintf(stderr, "%s: read_glyph(): DEBUG: num_lines = %d\n",
            ProgName, num_lines);
    }

    FirstGoodLine = 0;

    /* read */

    g_fp = fopen(GlyphFile, "r");
    if (g_fp == NULL) {
        fprintf(stderr, "%s: read_glyph(): ERROR: fopen failed, error: %s\n",
            ProgName, strerror(errno));
        exit(0);
    }

    len = getl(g_fp, &input_line, MAX_GLYPH_LINE); /* text header */

    if (Debug != FALSE) {
        fprintf(stderr,
            "%s: read_glyph(): DEBUG: header length = %d\n", ProgName, len);
#ifdef SUPER_DEBUG
        fprintf(stderr,
            "%s: read_glyph(): DEBUG: header input_line = >%s<\n",
            ProgName, input_line);
#endif
    }

    for (line_num = 1; line_num < num_lines; line_num++) {
        len = getl(g_fp, &input_line, MAX_GLYPH_LINE);
#ifdef SUPER_DEBUG
        if (Debug != FALSE) {
            fprintf(stderr, "%s: read_glyph(): DEBUG: input_line number %d: >%s<\n",
                ProgName, line_num, input_line);
        }
#endif
        if (len == 0) {
            fprintf(stderr, "%s: read_glyph(): ERROR: zero length line number %d\n",
                ProgName, line_num);
            break;
        }
        i_col = 0;
        for (col = 0; col < NUM_COLUMNS - 1; col++) {
            d_col = 0;
            temp_char = input_line[i_col];
            while (temp_char != ',') {
                temp_char = input_line[i_col++];
                data_array[d_col++] = temp_char;
            }
            data_array[d_col - 1] = EOS;

            sscanf(data_array, "%f", &Glyph_array[col][line_num]);
#ifdef SUPER_DEBUG
            if (Debug != FALSE) {
                fprintf(stderr,
                    "%s: read_glyph(): DEBUG: data_array = >%s<\n",
                    ProgName, data_array);
                fprintf(stderr,
                    "%s: read_glyph(): DEBUG: Glyph_array[%d][%d] = %f\n",
                    ProgName, col, line_num,
                    Glyph_array[col][line_num]);
            }
#endif
        }
        d_col = 0;
        temp_char = input_line[i_col];
        while (temp_char != ',') {
            temp_char = input_line[i_col++];
            data_array[d_col++] = temp_char;
        }
        data_array[d_col - 1] = EOS;

        sscanf(data_array, "%f", &Glyph_array[NUM_COLUMNS - 1][line_num]);

        if ((!FirstGoodLine) && (Glyph_array[1][line_num] == 5.0)) {
            FirstGoodLine = line_num;

#ifdef SUPER_DEBUG
            if (Debug != FALSE) {
                fprintf(stderr, "%s: read_glyph(): DEBUG: FirstGoodLine = %d\n",
                    ProgName, FirstGoodLine);
            }
#endif
        }

        if (Glyph_array[NUM_COLUMNS - 1][line_num] != 420.0) {
            fprintf(stderr, "%s: read_glyph(): ERROR: record %d was: %f, should be 420.0\n",
                NUM_COLUMNS - 1, ProgName,
                Glyph_array[NUM_COLUMNS - 1][line_num]);
            exit(0);
        }

    }


    fclose(g_fp);

}

/*
* write_glyph - write from Glyph_array[][] into a CSV file
*/
int write_glyph(num_lines, start_id)
int num_lines;
int start_id;
{

    int col, row, temp_int;

#ifdef SUPER_DEBUG
    if (Debug != FALSE) {
        fprintf(stderr, "%s: write_glyph(): DEBUG: num_lines = %d\n",
            "write_glyph()", num_lines);
        fprintf(stderr, "%s: write_glyph(): DEBUG: start_id = %d\n",
            "write_glyph()", start_id);
    }
#endif

    for (row = FirstGoodLine; row < num_lines; row++) {

        if (FirstGoodLine == row) {
            First_id = Glyph_array[ID_0][row];
            if (Debug != FALSE) {
                fprintf(stderr, "%s: write_glyph(): DEBUG: First_id = %d\n",
                    "write_glyph()", First_id);
            }
        }

        /* do all surgery here */

        Glyph_array[ID_0][row] =
            Glyph_array[ID_0][row] - First_id + start_id;

        Glyph_array[DATA_2][row] = Glyph_array[ID_0][row];

        /* anything with parent zero stays that way */

        if (Glyph_array[PARENT_ID_4][row] != 0.0) {
            Glyph_array[PARENT_ID_4][row] =
                Glyph_array[PARENT_ID_4][row] - First_id + start_id;
        }

        Glyph_array[RECORD_ID_92][row] = Glyph_array[ID_0][row];

        /* end of surgery */

        for (col = 0; col < NUM_COLUMNS; col++) {

            temp_int = (int)Glyph_array[col][row];
            if ((float)temp_int == Glyph_array[col][row]) {
                fprintf(stdout, "%d", (int)Glyph_array[col][row]);
            }
            else {
                fprintf(stdout, "%f", Glyph_array[col][row]);
            }
            if (col < NUM_COLUMNS - 1) {
                fprintf(stdout, ",");
            }
        }
        fprintf(stdout, "\n");
    }

    return start_id + num_lines - FirstGoodLine;
}