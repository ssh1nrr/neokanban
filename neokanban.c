#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neokanban.h"

static char *DATA_FILE;

int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 4)
	{
		print_help();
		return 0;
	}
	DATA_FILE = calloc(1, MAX_BUF);
	strncat(DATA_FILE, argv[1], 1);

	char *table[HEIGHT][N_COLS];
	Column cols[N_COLS];

	empty_table(table);
	empty_cols(&cols[0]);
	read_from_file(&cols[0], table);

	if (argc == 2)
	{
		print_table(table);
		free_space(table);
		return 0;
	}

	char *flag = argv[2];

	if (strcmp(flag, "--add") == 0 || strcmp(flag, "-a") == 0)
	{
		add_task(&cols[0], argv[3], TODO, -1);
	}
	else if (strcmp(flag, "--remove") == 0 || strcmp(flag, "-r") == 0)
	{
		int task_id = atoi(argv[3]);
		remove_task(task_id, &cols[0]);
	}
	else if (strcmp(flag, "--upgrade") == 0 || strcmp(flag, "-u") == 0)
	{
		int task_id = atoi(argv[3]);
		upgrade_task(task_id, &cols[0]);
	}
	else if (strcmp(flag, "--downgrade") == 0 || strcmp(flag, "-d") == 0)
	{
		int task_id = atoi(argv[3]);
		downgrade_task(task_id, &cols[0]);
	}
	else if (strcmp(flag, "--clear") == 0 || strcmp(flag, "-c") == 0)
	{
		empty_cols(&cols[0]);
		write_to_file(DATA_FILE, cols);
	}
	else if (strcmp(flag, "--reset") == 0)
	{
		// go to all tasks from doing and done
		for (int i = DOING; i <= DONE; i++)
		{
			for (int j = 0; j < cols[i].populated; j++)
			{
				// remove them and place into todo
				move_task(cols[i].tasks[j].id, &cols[0], TODO);
			}
		}
	}
	else if (strcmp(flag, "--help") == 0 || strcmp(flag, "-h") == 0)
	{
		print_help();
		free_space(table);
		return 0;
	}
	
	free_space(table);
	empty_table(table);
	read_from_file(&cols[0], table);
	print_table(table);
	free_space(table);
	free(DATA_FILE);
}

void move_task(int task_id, Column *cols, int dest_col_id)
{
	Task *t = malloc(sizeof(Task));
	Task *copy = malloc(sizeof(Task));

	t = find_task(task_id, cols);
	copy->id = t->id;
	copy->col_id = t->col_id;
	snprintf(copy->content, MAX_BUF, t->content);
	
	remove_task(t->id, cols);
	add_task(cols, copy->content, dest_col_id, copy->id);
	free(copy);
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
	char* titles[] = {"> to-do", "> doing", "> done"};
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
		for (int j = 0; j < s[i]->n_lines; j++)
		{
			if (s[i]->n_lines < biggest)
				free(s[i]->text_a[j]);
		}
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
		for (int j = 0; j < biggest; j++)
		{
			free(s[i]->text_a[j]);
		}
		free(s[i]);
	}
}

void print_help(void)
{
	printf("\nusage: ./neokanban <file> <option> [id,content]\n");
	printf("options:\n");
	printf("\t--add, -a \"content\"\n");
	printf("\t\tadds a task to the table\n");
	printf("\t--upgrade, -u id\n");
	printf("\t\tmoves task of given id to the next column\n");
	printf("\t--downgrade, -d id\n");
	printf("\t\tmoves task of given id to the previous column\n");
	printf("\t--remove, -r id\n");
	printf("\t\tremoves task of given id from table\n");
	printf("\t--clear, -c\n");
	printf("\t\tremoves all tasks from table\n");
	printf("\t--reset\n");
	printf("\t\tmoves all tasks to the \"todo\" column\n\n");
}

void remove_task(int task_id, Column *cols)
{
	Task *t = malloc(sizeof(Task));
	t = find_task(task_id, cols);
	// remove task and fix space left in column
	for (int i = 0; i < cols[t->col_id].populated - 1; i++)
	{
		if (&cols[t->col_id].tasks[i] != t)
			continue;
		t->id = cols[t->col_id].tasks[i + 1].id;
		t->content[0] = '\0';
		snprintf(t->content, MAX_BUF, cols[t->col_id].tasks[i + 1].content);

		t = &cols[t->col_id].tasks[i + 1];
	}
	t->content[0] = '\0';
	t->id = -1;
	cols[t->col_id].populated--;
	write_to_file(DATA_FILE, cols);
}

void write_to_file(const char *file_name, Column *cols)
{
	Column cols_a[N_COLS];
	for (int i = 0; i < N_COLS; i++)
	{
		cols_a[i] = cols[i];
	}

	FILE *f = fopen(file_name, "wb");
	fwrite(&cols_a, sizeof(Column) * N_COLS + 1, 1, f);
	fclose(f);
}

void empty_table(char* table[HEIGHT][N_COLS])
{
	// build empty table and empty cols
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < N_COLS; j++)
		{
			table[i][j] = calloc(1, MAX_BUF);
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

	Column *buf = calloc(1, sizeof(Column) + sizeof(Task) * HEIGHT + 1);

	int cur_col = TODO;
	while(fread(buf, sizeof(Column), 1, data))
	{
		cols[cur_col] = *buf;

		for (int i = 0; i < buf->populated; i++)
		{
			snprintf(table[i][buf->id], MAX_BUF, "[%d] %s", buf->tasks[i].id, buf->tasks[i].content);
		}
		cur_col++;
	}
	fclose(data);
	free(buf);
}

void add_task(Column *cols, char* content, size_t col_id, int id)
{
	Task task;

	if (id == -1)
	{
		task.id = -1;

		// go to all columns, check all ids and go for the smaller available
		for (int i = 0; i < N_COLS; i++)
		{
			if (cols[i].populated == 0)
			{
				continue;
			}
			for (int j = 0; j < cols[i].populated; j++)
			{
				if (cols[i].tasks[j].id > task.id)
					task.id = cols[i].tasks[j].id;
			}
		}
		task.id++; 
	}
	else
	{
		task.id = id;
	}

	task.col_id = col_id;
	task.content[0] = '\0';
	snprintf(task.content, MAX_BUF, content);
	cols[col_id].tasks[cols[col_id].populated] = task;
	cols[col_id].populated++;

	write_to_file(DATA_FILE, cols);
}

Task *find_task(size_t task_id, Column *cols)
{
	Task *t = malloc(sizeof(Task));
	for (int i = 0; i < N_COLS; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			if(cols[i].tasks[j].id == task_id)
			{
				t = &cols[i].tasks[j];
				return t;
			}	
		}
	}
	return NULL;
}

void upgrade_task(size_t task_id, Column *cols)
{
	Task *t = malloc(sizeof(Task));
	Task *copy = malloc(sizeof(Task));

	t = find_task(task_id, cols);
	copy->id = t->id;
	copy->col_id = t->col_id;
	snprintf(copy->content, MAX_BUF, t->content);
	
	if (t->col_id == DONE)
		return;
	remove_task(t->id, cols);
	add_task(cols, copy->content, copy->col_id + 1, copy->id);
	free(copy);
}

void downgrade_task(size_t task_id, Column* cols)
{
	Task *t = calloc(1, sizeof(Task));
	Task *copy = calloc(1, sizeof(Task));

	t = find_task(task_id, cols);
	copy->id = t->id;
	copy->col_id = t->col_id;
	snprintf(copy->content, MAX_BUF, t->content);

	if (t->col_id == TODO)
		return;
	remove_task(t->id, cols);
	add_task(cols, copy->content, copy->col_id - 1, copy->id);
	free(copy);
}

void free_space(char *table[HEIGHT][N_COLS])
{
	// CLEANUP
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < N_COLS; j++)
		{
			free(table[i][j]);
		}
	}
}
