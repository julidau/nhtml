/*
 * attribute.c
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#include "attribute.h"
#include <string.h> // needs: strcmp
#include <stdlib.h> // needs: realloc

// linear search in set
// returns NULL on failure to find entry
attr_t * attr_set_find(attr_set_t *set, const char * name) {
	size_t current = 0;

	for (; current != set->len; ++current) {
		if (strcmp(set->arr[current].name.c_str, name) == 0) {
			return set->arr + current;
		}
	}

	return NULL;
}

// Append new Attribute to set
int attr_set_append(attr_set_t * set, attr_t *new_entry) {
	if (new_entry->name.c_str== NULL) return -1; // reject empty entries

	// search first
	attr_t * new_ptr = attr_set_find(set, new_entry->name.c_str);
	if (new_ptr != NULL) {
		// already contained in set
		// just change entries value
		string_destroy(new_ptr->value);
		new_ptr->value = string_copy(new_entry->value);
		return 0;
	}

	new_ptr = realloc(set->arr, (set->len+1)*sizeof(attr_t));
	if (new_ptr == NULL) {
		return -1;
	}

	// append and quit
	set->arr = new_ptr;
	set->arr[set->len] = attr_copy(new_entry);
	set->len++;
	return 0;
}

attr_t attr_copy(attr_t * attr) {
	attr_t temp;
	temp.name = string_copy(attr->name);
	temp.value = string_copy(attr->value);
	return temp;
}

void attr_destroy(attr_t * attr) {
	// clear memory
	string_destroy(attr->name);
	string_destroy(attr->value);

	// reset memory content
	memset(attr, 0, sizeof(attr_t));
}


