#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "sortings.h"

int compare_strings_ascending(const char *a, const char *b) {
	return strcmp(a, b);
}

int compare_strings_descending(const char *a, const char *b) {
	return strcmp(b, a);
}

bool is_numeric(char *string) {
	for (array_index_t i = 0; ; i++) {
		if (string[i] == 0)
			return 1;
		if (string[i] < '0' || string[i] > '9')
			return 0;
	}
}

int parse_arguments(char **argv, array_size_t *array_size, char **input_file_name, char **output_file_name, sorting_func_t *sorting, comparator_func_t *comparator) {
	static const array_index_t ARRAY_SIZE_INDEX = 1, INPUT_FILE_NAME_INDEX = 2, OUTPUT_FILE_NAME_INDEX = 3, SORTING_FUNC_INDEX = 4, COMPARATOR_NAME_INDEX = 5;
	static const int SUCCESS = 0, FAILURE = -1, BASE = 10;
	if (!is_numeric(argv[ARRAY_SIZE_INDEX])) {
		fprintf(stderr, "Expected positive integer number (amount of strings to be sorted) as first argument, but got %s\n", argv[ARRAY_SIZE_INDEX]);
		return FAILURE;
	}
	*array_size = (array_size_t)strtoull(argv[ARRAY_SIZE_INDEX], NULL, BASE);
	if (errno == ERANGE) {
		fprintf(stderr, "Too big amount of strings to be sorted :(\n");
		return FAILURE;
	}
	*input_file_name = argv[INPUT_FILE_NAME_INDEX];
	*output_file_name = argv[OUTPUT_FILE_NAME_INDEX];
	if (strcmp(argv[SORTING_FUNC_INDEX], "bubble") == SUCCESS) {
		*sorting = bubble;
	}
	else if (strcmp(argv[SORTING_FUNC_INDEX], "insertion") == SUCCESS) {
		*sorting = insertion;
	}
	else if (strcmp(argv[SORTING_FUNC_INDEX], "merge") == SUCCESS) {
		*sorting = merge;
	}
	else if (strcmp(argv[SORTING_FUNC_INDEX], "quick") == SUCCESS) {
		*sorting = quick;
	}
	else if (strcmp(argv[SORTING_FUNC_INDEX], "radix") == SUCCESS) {
		*sorting = radix;
	}
	else {
		fprintf(stderr, "Expected sorting algorithm name (one of the following: 'bubble', 'insertion', 'merge', 'quick', 'radix'), but got %s\n", argv[SORTING_FUNC_INDEX]);
		return FAILURE;
	}
	if (strcmp(argv[COMPARATOR_NAME_INDEX], "asc") == SUCCESS) {
		*comparator = compare_strings_ascending;
	}
	else if (strcmp(argv[COMPARATOR_NAME_INDEX], "des") == SUCCESS) {
		*comparator = compare_strings_descending;
	}
	else {
		fprintf(stderr, "Expected comparator name (one of the following: 'asc', 'des'), but got %s\n", argv[COMPARATOR_NAME_INDEX]);
		return FAILURE;
	}
	return SUCCESS;
}

void free_strings_array(strings_array_t strings_array, array_size_t number_of_elements) {
	for (array_index_t i = 0; i < number_of_elements; i++) {
		free(strings_array[i]);
	}
	free(strings_array);
}

int read_strings_array(strings_array_t strings_array, array_size_t array_size, FILE *input_file) {
	static const int SUCCESS = 0, FAILURE = -1;
	for (array_index_t i = 0; i < array_size; i++) {
		strings_array[i] = (string_t)(malloc((MAX_INPUT_STRING_SIZE + 2) * sizeof(char)));
		if (strings_array[i] == NULL) {
			fprintf(stderr, "Could not allocate memory for string #%zu in strings array\n", i + 1);
			free_strings_array(strings_array, i);
			return FAILURE;
		}

		char *reading_result = fgets(strings_array[i], MAX_INPUT_STRING_SIZE + 1, input_file);
		if (reading_result == NULL) {
			if (feof(input_file) != 0) {
				fprintf(stderr, "File contains %zu strings which is less than %zu\n", i, array_size);
			}
			else {
				fprintf(stderr, "An error occured while reading string #%zu\n", i + 1);
			}
			free_strings_array(strings_array, i + 1);
			return FAILURE;
		}

		array_index_t last_symbol_index = strlen(strings_array[i]);
		if (strings_array[i][last_symbol_index - 1] == '\n') {
			strings_array[i][last_symbol_index - 1] = 0;
		}
		for (array_index_t j = last_symbol_index; j < MAX_INPUT_STRING_SIZE + 2; j++) {
			strings_array[i][j] = 0;
		}

	}
	return SUCCESS;
}

int print_strings_array(strings_array_t strings_array, array_size_t array_size, FILE *output_file) {
	static const int SUCCESS = 0, FAILURE = -1;
	for (array_index_t i = 0; i < array_size; i++) {
		int printing_result = fprintf(output_file, "%s\n", strings_array[i]);
		if (printing_result != (int)(strlen(strings_array[i]) + 1)) {
			fprintf(stderr, "An error occured while printing string #%zu to output file\n", i + 1);
			free_strings_array(strings_array, array_size);
			return FAILURE;
		}
	}
	return SUCCESS;
}

int main(int argc, char *argv[]) {
	static const int ARGUMENTS_AMOUNT = 5, SUCCESS = 0, FAILURE = -1;
	if (argc != ARGUMENTS_AMOUNT + 1) {
		fprintf(stderr, "Expected 5 arguments, got %d\n", argc - 1);
		return FAILURE;
	}
	
	array_size_t array_size;
	char *input_file_name, *output_file_name;
	comparator_func_t comparator;
	sorting_func_t sorting;
	int parsing_result = parse_arguments(argv, &array_size, &input_file_name, &output_file_name, &sorting, &comparator);
	if (parsing_result == FAILURE) {
		return FAILURE;
	}
	
	if (array_size == 0) {
		FILE *output_file = fopen(output_file_name, "w");
		if (output_file == NULL) {
			fprintf(stderr, "Could not open file '%s' in writing mode\n", output_file_name);
			return FAILURE;
		}
		fprintf(output_file, "\n");
		fclose(output_file);
		return SUCCESS;
	}
	
	strings_array_t strings_array = (strings_array_t)(malloc(array_size * sizeof(string_t)));
	if (strings_array == NULL) {
		fprintf(stderr, "Could not allocate memory for array of strings\n");
		return FAILURE;
	}
	
	FILE *input_file = fopen(input_file_name, "r");
	
	if (input_file == NULL) {
		fprintf(stderr, "Could not open file '%s' in reading mode\n", input_file_name);
		return FAILURE;
	}
	
	int reading_result = read_strings_array(strings_array, array_size, input_file);
	fclose(input_file);
	if (reading_result == FAILURE) {
		return FAILURE;
	}
	
	srand(time(0));
	sorting(strings_array, array_size, comparator);
	
	FILE *output_file = fopen(output_file_name, "w");
	if (output_file == NULL) {
		fprintf(stderr, "Could not open file '%s' in writing mode\n", output_file_name);
		return FAILURE;
	}
	int printing_result = print_strings_array(strings_array, array_size, output_file);
	fclose(output_file);
	if (printing_result == FAILURE) {
		return FAILURE;
	}
	
	free_strings_array(strings_array, array_size);
	
	return SUCCESS;
}