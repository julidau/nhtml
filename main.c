#include <stdio.h>  // needs: fgetc, fputs, fopen, fprintf
#include <stdlib.h> // needs: realloc, malloc, free
#include <string.h> // needs: strcmp, memcpy
#include <errno.h> 
typedef struct {
	char * c_str;
	size_t len;
} string_t;

int string_append(string_t *str, char c) {
	if (str->c_str == NULL) {
		// new string, need to emit EOS
		str->len++;
	}

	char * new_ptr  = realloc(str->c_str, str->len+1);
	if (new_ptr == NULL) {
		return -1;
	}

	// append char
	str->c_str = new_ptr;
	str->c_str[str->len-1] = c;
	str->c_str[str->len] = 0; // make sure
	str->len++;
}

void string_destroy(string_t s) {
	free(s.c_str);
}

string_t string_copy(string_t old) {
	string_t tmp;
	tmp.c_str = malloc(old.len);
	tmp.len = old.len;
	memcpy(tmp.c_str, old.c_str, tmp.len);
	return tmp;
}

typedef struct {
	string_t name, value;
} attr_t;

typedef struct {
	attr_t * arr;
	size_t len;
} attr_set_t;

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
	set->arr[set->len] = *new_entry;
	set->len++;
	return 0;
}

int parse_attr(FILE * stream, attr_set_t * output) {
	fprintf(stderr, "stub: parse_attr\n");
	return 0;
}

int parse_text(FILE * stream, FILE * output) {
	// STUB
	fprintf(stderr, "stub: parse_text\n");
	return 0;
}

typedef struct node {
	string_t name;
	attr_set_t attributes;
} node_t;


// write out node
void print_node(node_t * node, FILE * output) {
	// start by writing tag
	fputc('<', output);

	// follow with tag name
	fputs(node->name.c_str,output);

	// add attributes
	size_t i = 0;
	attr_t * current = node->attributes.arr;
	for (; i < node->attributes.len; i++,current++) {
		fprintf(output, " %s=\"%s\"", current->name.c_str, current->value.c_str);
	}

	// close tag
	fputc('>', output);
}

int print_tag(FILE * stream, FILE * output) {
	node_t new_node;
	int buffer = 0;

	while ((buffer = fgetc(stream)) != EOF) {
		switch(buffer) {
		case '[': // parse attributes
			if (parse_attr(stream, &new_node.attributes) != -1) {
				fprintf(stderr, "could not parse attributes: %d", -1);
				return -1;
			}
			break;
		case '"':
			// raw text
			parse_text(stream, output);
			break;
		default:
			if (buffer != ' ' && buffer != '\t' && buffer != '\n') {
				string_append(&new_node.name, buffer);
			}
		}
	}

	
	return 0;
}

int main(int argc, char ** args) {
	char * filename= NULL;
	for(;argc != 1; --argc) {
		filename = args[argc-1];
		printf("starting conversion of %s\n", filename);
		
		FILE * handle = fopen(filename, "r");

		if (handle == NULL) {
			fprintf(stderr, "could not open \"%s\": %s\n", filename, strerror(errno));
			continue;
		}
		
		while(!feof(handle)) {
			if (print_tag(handle, stdout) != 0) {
				fprintf(stderr, "error during parsing of node\n");
				break;
			}
		}	

		fclose(handle);
	}

	printf("done compiling\n");
	return 0;
}
