#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "neokanban.h"

int next_wlen(char* str)
{
	int len = 0;
	while (str[len] != ' ' && str[len] != '\0' && str[len] != '\n')
	{
		len++;
	}
	return len;
}

//////////
void format(size_t extra_lines,
						size_t col_id,
						char* str, 
						int limit, 
						formatted_str *container)
{
	char* word = calloc(1, MAX_BUF);
	char* line = calloc(1, MAX_BUF);

	char* begin = col_id == 0 ? "|" : "";
	char space = ' ';
	char* end = " |";
	int len_e = strlen(end);
	int len_b = strlen(begin);
	int len_w = 0;
	size_t n_lines = 0;
	size_t n_spaces;

	if (str[0] == '\0')
	{
		n_spaces = limit - len_b - len_e;
		for (int i = 0; i < extra_lines + 1; i++)	
		{
			line[0] = '\0';
			strcat(line, begin);
			for (int j = 0; j < n_spaces; j++)
			{
				strncat(line, &space, 1);
			}
			strcat(line, end);
			container->text_a[i] = malloc(MAX_BUF);
			container->text_a[i][0] = '\0';
			strcpy(container->text_a[i], line);
			n_lines++;
		}

		container->n_lines = n_lines;

		free(word);
		free(line);
		return;
	}

	// string not empty
	// go through every character and store it in a word var if it's not space
	// if it is space, lookup next word
	// if next word is bigger than the remaining space left, add array to cont
	// if next word still fits, just save the word to the line and continue
	//
	
	strcpy(line, begin);
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] != ' ')
		{
			strncat(word, &str[i], 1);
			continue;
		}
		strncat(line, &space, 1);
		strcat(line, word);
		word[0] = '\0';
		int nxt_len = next_wlen(&str[i + 1]);
		if (strlen(line) + nxt_len + len_e >= limit)
		{
			// next word doesn't fit
			// add spaces and end
			n_spaces = limit - strlen(line) - len_e;
			for (int j = 0; j < n_spaces; j++)
			{
				strncat(line, &space, 1);
			}
			strcat(line, end);
			// store and reset line
			container->text_a[n_lines] = calloc(1, MAX_BUF);
			strcpy(container->text_a[n_lines], line);
			line[0] = '\0';
			strcpy(line, begin);
			n_lines++;
		}
	}

	if (word[0] != '\0')
	{
		if (strlen(line) + strlen(word) + len_e >= limit)
		{
			// word + line overflows
			// write line first and setup a new one just for the word
			n_spaces = limit - strlen(line) - len_e;
			for (int i = 0; i < n_spaces; i++)
			{
				strncat(line, &space, 1);
			}
			strcat(line, end);
			container->text_a[n_lines] = calloc(1, MAX_BUF);
			strcpy(container->text_a[n_lines], line);
			n_lines++;
			line[0] = '\0';
			strcpy(line, begin);
		}

		strncat(line, &space, 1);
		strcat(line, word);
		n_spaces = limit - strlen(line) - len_e;
		for (int i = 0; i < n_spaces; i++)
		{
			strncat(line, &space, 1);
		}
		strcat(line, end);
		container->text_a[n_lines] = calloc(1, MAX_BUF);
		strcpy(container->text_a[n_lines], line);
		n_lines++;
	}
	// now extra lines
	n_spaces = limit - len_b - len_e;
	for (int i = 0; i < extra_lines; i++)	
	{
		line[0] = '\0';
		strcat(line, begin);
		for (int j = 0; j < n_spaces; j++)
		{
			strncat(line, &space, 1);
		}
		strcat(line, end);
		container->text_a[n_lines] = calloc(1, MAX_BUF);
		strcpy(container->text_a[n_lines], line);
		n_lines++;
	}

	container->n_lines = n_lines;

	free(word);
	free(line);
}
