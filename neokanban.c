#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "neokanban.h"

const size_t TODO = 0;
const size_t DOING = 1;
const size_t DONE = 2;

const char data_file[] = ".data";

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
			table[i][j][0] = '\0';
			strcpy(table[i][j], "");
		}
	}
	for (int i = 0; i < N_COLS; i++)
	{
		cols[i].id = i;
		cols[i].populated = 0;
		for (int j = 0; j < HEIGHT; j++)
		{
			cols[i].tasks[j].id = 0;
		}
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
		task.id = -1;

		task.content[0] = '\0';
		// go to all columns, check all ids and go for the smaller available
		for (int i = 0; i < N_COLS; i++)
		{
			if (cols[i].populated == 0)
			{
				// empty column
				continue;
			}
			for (int j = 0; j < cols[i].populated; j++)
			{
				if (cols[i].tasks[j].id > task.id)
					task.id = cols[i].tasks[j].id;
			}
		}
		task.id++; 

		// set content to user input
		snprintf(task.content, MAX_BUF, content);

		// add task to column (in this case, todo)
		cols[TODO].tasks[cols[TODO].populated] = task;

		// update table
		snprintf(table[cols[TODO].populated][TODO], MAX_BUF, "[%d] %s", task.id, task.content);

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
	print_row(titles);
	print_separator();
}

void print_table(char* table[HEIGHT][N_COLS])
{
	print_header();
	for (int i = 0; i < HEIGHT; i++)
	{
		print_row(table[i]);
	}
	print_separator();
}

void print_row(char* row[])
{
	size_t biggest = 0;
	formatted_str *s[N_COLS];

	for (int i = 0; i < N_COLS; i++)
	{
		s[i] = malloc(sizeof(formatted_str));
		format(0, i, row[i], COL_WIDTH, s[i]);
		if (s[i]->n_lines > biggest)
			biggest = s[i]->n_lines;
	}
	
	for (int i = 0; i < N_COLS; i++)
	{
		size_t xtra = biggest - s[i]->n_lines;
		if (s[i]->n_lines < biggest)
			format(xtra, i, row[i], COL_WIDTH, s[i]);
	}

	for (int i = 0; i < biggest; i++)
	{
		for (int j = 0; j < N_COLS; j++)
		{
			printf("%s", s[j]->text_a[i]);
		}
		printf("\n");
	}

	// cleanup
	for (int i = 0; i < N_COLS; i++)
	{
		free(s[i]);
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

