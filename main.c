#include <stdio.h>  // needs: fgetc, fputs, fopen, fprintf
#include <stdlib.h> // needs: realloc, malloc, free
#include <string.h> // needs: strcmp, memcpy
#include <errno.h> 
#include <ctype.h> // needs: isspace

#include <getopt.h> // needs: getopt_long

#include "html.h" // needs: html_escaped

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
	return 0;
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
	set->arr[set->len] = *new_entry;
	set->len++;
	return 0;
}

void attr_reset(attr_t * attr) {
	memset(attr, 0, sizeof(attr_t));
}

int strip(FILE * stream) {
	int current = 0;
	while((current = fgetc(stream)) != EOF) {
		if (!isspace(current))
			break;
	}

	return current;
}

int parse_attr(FILE * stream, attr_set_t * output) {
	attr_t current_attr = {};
#ifdef DEBUG
	printf("parse_attr\n");
#endif
	int buffer = 0;
	unsigned char isKey = 1;

	while((buffer = fgetc(stream)) != EOF) {
		// parse key=value pairs
		// check for delim
		//if (buffer == ' ' || buffer == '\t' || buffer == '\n'){
		if (isspace(buffer)) {
			attr_set_append(output, &current_attr);
#ifdef DEBUG
			printf("parsed attr: %s=%s\n", current_attr.name.c_str, current_attr.value);
#endif
			// reset attribute
			attr_reset(&current_attr);
			isKey = 1;
			continue;
		}
		if (buffer == '=') {
			isKey = 0;
			continue;
		} else
		if (buffer == ']') {
			break;
		}

		if (isKey) {
			string_append(&current_attr.name, buffer);
		} else {
			string_append(&current_attr.value, buffer);
		}
	}

	// append last attribute
	attr_set_append(output, &current_attr);

	//memset(output, 0, sizeof(attr_set_t));
	return strip(stream);
}

int parse_text(FILE * stream, FILE * output) {
#ifdef DEBUG
	printf("parse text\n");
#endif
	int buffer = 0;
	char escaped = 0;

	while((buffer = fgetc(stream)) != EOF) {
		if (!escaped && buffer == '\\') {
			escaped = 1;
			continue;
		}
		if (!escaped && buffer == '"') {
			break;
		}
		escaped = 0;

		if (html_escape(buffer, output)) {
			continue;
		}

		fputc(buffer, output);
	}

	//fprintf(stderr, "stub: parse_text\n");
	return strip(stream);
}

typedef struct node {
	string_t name;
	attr_set_t attributes;
} node_t;


// write out node
void open_node(node_t * node, FILE * output) {
	// check for empty node (text mostly)
	if (!node->name.c_str)  return;

	// start by writing tag
	fputc('<', output);

	// follow with tag name
	fputs(node->name.c_str,output);

	// add attributes
	size_t i = 0;
	attr_t * current = node->attributes.arr;
	for (; i < node->attributes.len; i++,current++) {
		if (current->value.c_str)
			fprintf(output, " %s=\"%s\"", current->name.c_str, current->value.c_str);
		else {
			fputc(' ', output);
			fputs(current->name.c_str, output);
		}
	}

	// close tag
	fputc('>', output);
}
void close_node(node_t * node, FILE * output) {
	if (node->name.c_str) {
		fprintf(output, "</%s>", node->name.c_str);
	}
}

int readName(FILE *stream, node_t *node) {
	int current = strip(stream);
	if (current == EOF) {
		return current;
	}

	do {
		if (current == '{' || current == '[') {
			break;
		}

		if (current == ' ' || current == '\t' || current == '\n') {
			current = strip(stream);
			break;
		}

		string_append(&node->name, current);
	} while((current = fgetc(stream)) != EOF);

	return current;
}


enum node_type {
	NODE_SELFCLOSING,
	NODE_TEXT,
	NODE_TAG
};

int parse_node(int current, FILE * stream, FILE * output) {
	if (current == '"') {
		return parse_text(stream, output);
	}

	// normal node
	node_t current_node = {};
	string_append(&current_node.name, current);
	current = readName(stream, &current_node);
	if (current == EOF) {
		goto done;
	}
#ifdef DEBUG
	printf("parse_node: %s\n", current_node.name.c_str);
#endif
	if (current == '[') {
		current = parse_attr(stream, &current_node.attributes);
	}

	if (current != '{') {
		// tag is selfclosing
		open_node(&current_node, output);
		return current;
	}
	current = strip(stream);

	open_node(&current_node, output);
	while(current != '}' && current != EOF) {
		current = parse_node(current, stream, output);
	}

	close_node(&current_node, output);
done:
	return strip(stream);
}

int main(int argc, char ** args) {
	//getopt_long(argc, args, NULL, NULL, NULL);

	char * filename= NULL;
	FILE * output = fopen("a.html", "w");
	if (output == NULL) {
		fprintf(stderr, "could not create output file\n");
		return -1;
	}

	for(;argc > 1; --argc) {
		filename = args[argc-1];
		printf("starting conversion of %s\n", filename);
		
		FILE * handle = fopen(filename, "r");

		if (handle == NULL) {
			fprintf(stderr, "could not open \"%s\": %s\n", filename, strerror(errno));
			continue;
		}
		
		int current = strip(handle);

		while((current = parse_node(current, handle, output)) != EOF) {
//			if () {
//				fprintf(stderr, "error during parsing of node\n");
//				break;
//			}
		}	

		fclose(handle);
	}

	fclose(output);
	printf("done compiling\n");
	return 0;
}
