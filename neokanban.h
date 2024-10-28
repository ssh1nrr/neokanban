#ifndef NEOKANBAN_H
#define NEOKANBAN_H

#define MAX_BUF 4096
#define COL_WIDTH 24
#define N_COLS 3
#define HEIGHT 10
#define MAX_CELL_HEIGHT 10

#define DATA_FILE ".data"

#define TODO 0
#define DOING 1
#define DONE 2

typedef struct
{
	char *text_a[MAX_CELL_HEIGHT];
	size_t n_lines;
} formatted_str;

typedef struct
{
	int id;
	int col_id;
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
void remove_task(int task_id, Column *cols);
void write_to_file(const char *file_name, Column *cols);
void empty_table(char* table[HEIGHT][N_COLS]);
void empty_cols(Column *cols);
void read_from_file(Column* cols, char *table[HEIGHT][N_COLS]);
void add_task(Column *cols, char* content, size_t col_id, int id);
void upgrade_task(size_t task_id, Column* cols);
void downgrade_task(size_t task_id, Column* cols);
Task *find_task(size_t task_id, Column *cols);
void free_space(char *table[HEIGHT][N_COLS]);

#endif
