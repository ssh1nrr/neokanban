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

const size_t TODO = 0;
const size_t DOING = 1;
const size_t DONE = 2;

const char data_file[] = ".data";

typedef struct
{
	size_t id;
	char content[MAX_BUF];
} Task;

typedef struct
{
	size_t id;
	Task tasks[HEIGHT];
	size_t populated;
} Column;

int main(int argc, char* argv[])
{
	char *table[HEIGHT][N_COLS];

	FILE *data = fopen(data_file, "wb+");
	bool empty = true;

	Column cols[N_COLS];

	Column *buf = malloc(sizeof(Column));
	while(fread(buf, sizeof(Column), 1, data))
	{
		empty = false;
		for (int i = 0; i < buf->populated; i++)
		{
			table[i][buf->id] = strdup(buf->tasks[i].content);
		}
	}

	free(buf);
	fclose(data);

	if (empty)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < N_COLS; j++)
			{
				table[i][j] = malloc(sizeof(char) * MAX_BUF);
				snprintf(table[i][j], MAX_BUF, "");
				table[i][j][strlen(table[i][j])] = '\0';
			}
		}
	}
	print_table(table);

	//
	// for (int i = 0; i < N_COLS; i++)
	// {
	// 	cols[i].id = i;
	// 	cols[i].populated = 0;
	// }
	//
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
