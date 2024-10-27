#ifndef NEOKANBAN_H
#define NEOKANBAN_H

#define MAX_BUF 256
#define COL_WIDTH 24
#define N_COLS 3
#define HEIGHT 5
#define MAX_CELL_HEIGHT 10

typedef struct
{
	char *text_a[MAX_CELL_HEIGHT];
	size_t n_lines;
} formatted_str;

typedef struct
{
	int id;
	char content[MAX_BUF];
} Task;

typedef struct
{
	int id;
	Task tasks[HEIGHT];
	size_t populated;
} Column;


extern void append_char(char* str, char c);
extern int next_wlen(char* str);
void format(size_t extra_lines,
						size_t col_id,
						char* str, 
						int limit, 
						formatted_str *container);

void print_table(char* table[HEIGHT][N_COLS]);
void print_separator(void);
void print_header(void);
void print_help(void);
void print_row(char* row[]);


#endif
