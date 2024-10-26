#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 256
#define COL_WIDTH 24
#define N_COLS 3
#define HEIGHT 5

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

	Column cols[N_COLS];

	Column *buf = malloc(sizeof(Column));
	while(fread(buf, sizeof(buf), 1, data))
	{
		for (int i = 0; i < buf.populated; i++)
		{
			table[i][buf.id] = buf.task[i].content;
		}
	}
	//
	// for (int i = 0; i < N_COLS; i++)
	// {
	// 	cols[i].id = i;
	// 	cols[i].populated = 0;
	// }
	//
	return 0;
}
