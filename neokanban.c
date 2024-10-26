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
	return 0;
}
