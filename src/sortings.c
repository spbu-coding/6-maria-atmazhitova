#include "sortings.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void swap_strings(string_t *a, string_t *b) {
	string_t t = *a;
	*a = *b;
	*b = t;
}

void bubble(strings_array_t array, array_size_t array_size, comparator_func_t comp) {
	for (array_index_t i = 0; i + 1 < array_size; i++) {
		for (array_index_t j = 1; j < array_size - i; j++) {
			if (comp(array[j], array[j - 1]) < 0) {
				swap_strings(array + j - 1, array + j);
			}
		}
	}
}

void insertion(strings_array_t array, array_size_t array_size, comparator_func_t comp) {
	for (array_index_t i = 1; i < array_size; i++) {
		for (array_index_t j = i; j > 0 && comp(array[j], array[j - 1]) < 0 ; j--) {
			swap_strings(array + j - 1, array + j);
		}
	}
}

void merge_arrays(strings_array_t array1, array_size_t array1_size, strings_array_t array2, array_size_t array2_size, strings_array_t buffer, comparator_func_t comp) {
	array_index_t index1 = 0, index2 = 0;
	for (array_index_t buffer_index = 0; buffer_index < array1_size + array2_size; buffer_index++) {
		if (index1 >= array1_size) {
			buffer[buffer_index] = array2[index2];
			index2++;
		}
		else if (index2 >= array2_size) {
			buffer[buffer_index] = array1[index1];
			index1++;
		}
		else if (comp(array2[index2], array1[index1]) < 0) {
			buffer[buffer_index] = array2[index2];
			index2++;
		}
		else {
			buffer[buffer_index] = array1[index1];
			index1++;
		}
	}
}

void split_arrays(strings_array_t array1, array_size_t array1_size, strings_array_t array2, array_size_t array2_size, strings_array_t buffer) {
	for (array_index_t buffer_index = 0; buffer_index < array1_size; buffer_index++) {
		array1[buffer_index] = buffer[buffer_index];
	}
	for (array_index_t buffer_index = 0; buffer_index < array2_size; buffer_index++) {
		array2[buffer_index] = buffer[buffer_index + array1_size];
	}
}

void merge(strings_array_t array, array_size_t array_size, comparator_func_t comp) {
	strings_array_t buffer = (strings_array_t)malloc(array_size * sizeof(string_t));
	if (buffer == NULL) {
		fprintf(stderr, "Could not allocate memory for strings buffer in merge sort\n");
		exit(-1);
	}
	for (array_size_t partition_size = 1; partition_size < array_size; partition_size *= 2) {
		for (array_index_t i = 0; i < array_size; i += 2 * partition_size) {
			if (i + partition_size < array_size) {
				array_size_t second_partition_size = partition_size;
				if (i + 2 * partition_size > array_size) {
					second_partition_size = array_size - i - partition_size;
				}
				merge_arrays(array + i, partition_size, array + i + partition_size, second_partition_size, buffer, comp);
				split_arrays(array + i, partition_size, array + i + partition_size, second_partition_size, buffer);
			}
		}
	}
	free(buffer);
}

void quick(strings_array_t array, array_size_t array_size, comparator_func_t comp) {
	if (array_size <= 1)
		return;
	array_index_t i = 0, j = array_size - 1;
	string_t middle = array[rand() % array_size];
	
	while (i <= j) {
		
		while (i < array_size && comp(array[i], middle) < 0)
			i++;
		
		while (j > 0 && comp(middle, array[j]) < 0)
			j--;

		if (i >= j)
			break;
		swap_strings(array + i, array + j);
		i++;
		j--;
	}
	
	quick(array, j + 1, comp);
	quick(array + j + 1, array_size - j - 1, comp);
}

void radix(strings_array_t array, array_size_t array_size, comparator_func_t comp) {
	const array_size_t CHAR_DIFFERENT_VALUES = 256;
	const bool ascending = (comp("a", "b") < 0);
	array_size_t max_length = 0;
	for (array_index_t i = 0; i < array_size; i++) {
		array_size_t current_length = strlen(array[i]);
		if (max_length < current_length)
			max_length = current_length;
	}
	if (max_length == 0)
		return;
	array_index_t c[CHAR_DIFFERENT_VALUES];
	strings_array_t auxiliary_array = (strings_array_t)(malloc(array_size * sizeof(char *)));
	for (array_index_t i = max_length; i > 0; i--) {
		
		for (array_index_t j = 0; j < CHAR_DIFFERENT_VALUES; j++) {
			c[j] = 0;
		}
		
		for (array_index_t j = 0; j < array_size; j++) {
			c[(unsigned char)(array[j][i - 1])]++;
		}
		
		array_index_t counter = 0;
		if (ascending) {
			for (array_index_t j = 0; j < CHAR_DIFFERENT_VALUES; j++) {
				array_index_t temp = c[j];
				c[j] = counter;
				counter += temp;
			}
		}
		else {
			for (array_index_t j = CHAR_DIFFERENT_VALUES; j > 0; j--) {
				array_index_t temp = c[j - 1];
				c[j - 1] = counter;
				counter += temp;				
			}
		}
		
		for (array_index_t j = 0; j < array_size; j++) {
			auxiliary_array[c[(unsigned char)array[j][i - 1]]] = array[j];
			c[(unsigned char)array[j][i - 1]]++;
		}
		
		for (array_index_t j = 0; j < array_size; j++) {
			array[j] = auxiliary_array[j];
		}
	}
	free(auxiliary_array);
}