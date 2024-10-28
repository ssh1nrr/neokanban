#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "neokanban.h"

const size_t TODO = 0;
const size_t DOING = 1;
const size_t DONE = 2;

// const char *DATA_FILE = ".data";

int main(int argc, char* argv[])
{
	char *table[HEIGHT][N_COLS];
	Column cols[N_COLS];

	empty_table(table);
	empty_cols(&cols[0]);

	read_from_file(&cols[0], table);

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
		task.col_id = TODO;

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

		write_to_file(DATA_FILE, &cols[0]);
	}
	else if (strcmp(flag, "--remove") == 0)
	{
		int task_id = atoi(argv[2]);
		Task *t = malloc(sizeof(Task));
		t = remove_task(task_id, &cols[0], table);
		free(t);
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

Task *remove_task(int task_id, Column *cols, char* table[HEIGHT][N_COLS])
{
	Task *t = malloc(sizeof(Task));
	Task *copy = malloc(sizeof(Task));
	int col_found = -1;

	for (int i = 0; i < N_COLS; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			if(cols[i].tasks[j].id == task_id)
			{
				t = &cols[i].tasks[j];
				copy->id = t->id;
				copy->col_id = t->col_id;
				copy->content[0] = '\0';
				snprintf(copy->content, MAX_BUF, t->content);
				t->content[0] = '\0';
				t->id = -1;
				break;
			}	
		}
		if(col_found > -1)
			break;
	}

	Task *cur = malloc(sizeof(Task));
	Task *next = malloc(sizeof(Task));
	size_t cur_len, next_len;
	for (int i = 0; i < cols[copy->col_id].populated - 1; i++)
	{
		cur = &cols[copy->col_id].tasks[i];
		next = &cols[copy->col_id].tasks[i + 1];
		if (strlen(cur->content) == 0 && strlen(next->content) > 0)
		{
			snprintf(cur->content, MAX_BUF, next->content);
			next->content[0] = '\0';
			cur->id = next->id;
		}
	}
	cols[copy->col_id].populated--;
	write_to_file(DATA_FILE, cols);

	return copy;
}

void write_to_file(const char *file_name, Column *cols)
{
	Column cols_a[N_COLS];
	for (int i = 0; i < N_COLS; i++)
	{
		cols_a[i] = cols[i];
	}

	FILE *f = fopen(file_name, "wb");
	fwrite(&cols_a, sizeof(Column) * N_COLS, 1, f);
	fclose(f);
}

void empty_table(char* table[HEIGHT][N_COLS])
{
	// build empty table and empty cols
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < N_COLS; j++)
		{
			table[i][j] = malloc(sizeof(char) * MAX_BUF);
			table[i][j][0] = '\0';
		}
	}
}

void empty_cols(Column *cols)
{
	for (int i = 0; i < N_COLS; i++)
	{
		cols[i].id = i;
		cols[i].populated = 0;
		for (int j = 0; j < HEIGHT; j++)
		{
			cols[i].tasks[j].id = -1;
		}
	}
}

void read_from_file(Column *cols, char *table[HEIGHT][N_COLS])
{
	FILE *data = fopen(DATA_FILE, "rb");
	if (!data)
	{
		data = fopen(DATA_FILE, "wb+");
		if (!data)
		{
			printf("couldn't open file");
			return;
		}
	}

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
	fclose(data);
	free(buf);
}
