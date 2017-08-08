/*
 * string.c
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#include "nhtml_string.h"
#include <stdlib.h> // needs: malloc, free
#include <memory.h> // needs: memcpy

int string_append(string_t *str, char c) {
	if (str->c_str == NULL) {
		// new string, need to emit EOS
		str->len++;
	}

	char * new_ptr  = realloc(str->c_str, str->len+1);
	if (new_ptr == NULL) {
		errno = ENOMEM;
		return -1;
	}

	// append char
	str->c_str = new_ptr;
	str->c_str[str->len-1] = c;
	str->c_str[str->len] = 0; // make sure
	str->len++;
	return 0;
}

void string_destroy(string_t s) {
	free(s.c_str);
}

string_t string_copy(string_t old) {
	string_t tmp = {}; // initialize with 0

	tmp.c_str = malloc(old.len);
	if (tmp.c_str == NULL) {
		// The Application will have to handle a out of mem
		// situation here
		return tmp;
	}

	tmp.len = old.len;
	memcpy(tmp.c_str, old.c_str, tmp.len);
	return tmp;
}

