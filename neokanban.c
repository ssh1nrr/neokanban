#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 256
#define COL_WIDTH 24
#define N_COLS 3
#define HEIGHT 5

void print_table(char* table[HEIGHT][N_COLS]);
void print_separator(void);
void print_header(void);
void print_cell(size_t col_id, char* content);
void print_help(void);

const size_t TODO = 0;
const size_t DOING = 1;
const size_t DONE = 2;

const char data_file[] = ".data";

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

int main(int argc, char* argv[])
{
	char *table[HEIGHT][N_COLS];

	FILE *data = fopen(data_file, "rb");
	if (!data)
	{
		data = fopen(data_file, "wb+");
		if (!data)
		{
			printf("couldn't open file");
			return 1;
		}
	}

	Column cols[N_COLS];

	// build empty table and empty cols
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < N_COLS; j++)
		{
			table[i][j] = malloc(sizeof(char) * MAX_BUF);
			snprintf(table[i][j], MAX_BUF, "");
			table[i][j][strlen(table[i][j])] = '\0';
		}
	}
	for (int i = 0; i < N_COLS; i++)
	{
		cols[i].id = i;
		cols[i].populated = 0;
	}

	// read from file if possible
	Column *buf = malloc(sizeof(Column));
	int cur_col = TODO;
	while(fread(buf, sizeof(Column), 1, data))
	{
		cols[cur_col] = *buf;

		for (int i = 0; i < buf->populated; i++)
		{
			table[i][buf->id] = malloc(sizeof(char) * MAX_BUF);
			snprintf(table[i][buf->id], MAX_BUF, "[%d] %s", buf->tasks[i].id, buf->tasks[i].content);
			table[i][buf->id][strlen(table[i][buf->id])] = '\0';
		}
		cur_col++;
	}

	free(buf);
	fclose(data);

	// flags
	if (argc == 1)
	{
		print_table(table);
		return 0;
	}
	if (argc > 3)
	{
		print_help();
	}

	char *flag = argv[1];

	if (strcmp(flag, "--add") == 0)
	{
		char *content = argv[2];
		// create task
		Task task;
		task.id = 0;
		task.content[0] = '\0';
		// go to all columns, check all ids and go for the smaller available
		int min_id = 0;
		for (int i = 0; i < N_COLS; i++)
		{
			if (cols[i].populated == 0)
			{
				// empty column
				continue;
			}
			for (int j = 0; j < cols[i].populated; j++)
			{
				if (cols[i].tasks[j].id > min_id)
					min_id = cols[i].tasks[j].id;
			}
		}
		task.id = min_id;
		printf("task id: %d\n", task.id);

		// set content to user input
		strncat(task.content, content, sizeof(task.content) - strlen(content) - 1);
		task.content[strlen(content)] = '\0';

		// add task to column (in this case, todo)
		cols[TODO].tasks[cols[TODO].populated] = task;
		printf("%d\n", cols[TODO].tasks[cols[TODO].populated]);

		// update table
		snprintf(table[cols[TODO].populated][TODO], MAX_BUF, "[%d] %s", task.id,task.content);

		cols[TODO].populated++;
		cols[TODO].id = TODO;

		// update file
		FILE *f = fopen(data_file, "wb");
		fwrite(&cols, sizeof(Column) * N_COLS, 1, f);
		fclose(f);
		print_table(table);
	}
	
	// CLEANUP
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < N_COLS; j++)
		{
			free(table[i][j]);
		}
	}
	return 0;
}

void print_separator(void)
{
	size_t full_width = COL_WIDTH * N_COLS;
	for (int i = 0; i < full_width; i++)
	{
		if (i == 0 || i == full_width - 1)
		{
			printf("+");
		}
		else
		{
			printf("-");
		}
	}
	printf("\n");
}

void print_header(void)
{
	print_separator();
	char* titles[] = {"to-do", "doing", "done"};
	for (int i = 0; i < N_COLS; i++)
	{
		print_cell(i, titles[i]);
	}
	print_separator();
}

void print_table(char* table[HEIGHT][N_COLS])
{
	print_header();
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < N_COLS; j++)
		{
			print_cell(j, table[i][j]);
		}
	}
	print_separator();
}

void print_cell(size_t col_index, char* content)
{
	size_t n_spaces;
	if (col_index == 0)
	{
		printf("|");
		n_spaces = COL_WIDTH - strlen(content) - 3;
	}
	else
	{
		n_spaces = COL_WIDTH - strlen(content) - 2;
	}

	// strings that are smaller than the column width	
	int len = strlen(content);
	if (len < COL_WIDTH - 2)
	{
		printf(" %s", content);
		for (int i = 0; i < n_spaces; i++)
		{
				printf(" ");
		}
		printf("|");
	}
	// TODO prevent bigger strings of overflowing
	// 
	//
	// last col adds a newline
	if (col_index == N_COLS - 1)
	{
		printf("\n");
	}
}

void print_help(void)
{
	printf("usage: ./neokanban <options> [id,content]\n");
	printf("options:\n");
	printf("\t--add <content>\n");
	printf("\t--upgrade <id>\n");
	printf("\t--downgrade <id>\n");
	printf("\t--delete <id>\n");
}

