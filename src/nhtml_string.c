/*
 * string.c
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#include "nhtml_string.h"
#include <stdlib.h> // needs: malloc, free
#include <memory.h> // needs: memcpy, memset
#include <string.h> // needs: strlen, strdup

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

int string_concat(string_t * dest, string_t *src) {
	if (src->c_str == NULL) {
		return 0;
	}

	if (dest->c_str == NULL) {
		dest->len++;
	}

	char * new_ptr = realloc(dest->c_str, dest->len + src->len-1);
	if (new_ptr == NULL) {
		errno = ENOMEM;
		return -1;
	}

	memcpy(dest->c_str + dest->len-1, src->c_str, src->len);
	return 0;
}

void string_destroy(string_t *s) {
	free(s->c_str);
	memset(s, 0, sizeof(string_t));
}

string_t string_copy(string_t old) {
	string_t tmp = {}; // initialize with 0
	if (old.len == 0) {
		return tmp;
	}

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

string_t string_from_cstr(const char * str) {
	string_t temp;
	temp.len = strlen(str)+1;
	temp.c_str = strdup(str);
	return temp;
}
