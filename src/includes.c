/*
 * includes.c
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#include <includes.h>
#include <parser.h>
#include <nhtml_string.h>
#include <stdlib.h> // needs: malloc, realloc
#include <memory.h> // needs: memcpy

string_t* include_paths;
size_t num_includes = 0;

void include_paths_init() {
	include_paths = malloc(sizeof(string_t));
	num_includes = 1;

	include_paths[0] = string_from_cstr("."); // search in current dir by default
}

const char * extension(const string_t * path) {
	int i;
	for (i = path->len; i > 0;  i--) {
		if (path->c_str[i] == '/')
			return NULL;
		if (path->c_str[i] == '.')
			break;
	}

	return path->c_str + i;
}

void include_add_path(const char * path) {
	if (num_includes == 0) {
		include_paths_init();
	}

	include_paths = realloc(include_paths, sizeof(const char*)*(num_includes+1));
	include_paths[num_includes] = string_from_cstr(path);
	num_includes++;
}

int include_file(string_t *filename, FILE * output) {
	FILE * input = NULL;
	char * tempPath = NULL;

	if(num_includes == 0) {
		include_paths_init();
	}

	for (size_t i = 0; i < num_includes; ++i) {
		// actual length is length + 2 (2* end char)
		tempPath = malloc(filename->len + include_paths[i].len);
		if (tempPath == NULL) {
			continue;
		}

		// Concatenate the two paths
		// adding a safety slash to separate the two
		memcpy(tempPath, include_paths[i].c_str, include_paths[i].len-1);
		tempPath[include_paths->len-1] = '/';
		memcpy(tempPath + include_paths->len, filename->c_str, filename->len);

		input = fopen(tempPath, "r");
		if (input != NULL) {
			break;
		}
	}

	if (input == NULL) {
		return 0;
	}
	const char * ext = extension(filename);
#ifdef DEBUG
	printf("include %s, ext: %s\n", filename->c_str, ext);
#endif

	if (ext && !strcmp(ext, ".nhtml")) {
		// (re)parse whole file
		int current = strip(input);
		while((current = parse_node(current, input, output)) != EOF);
	} else {
		// copy file 1:1
		int current = 0;
		while((current = fgetc(input)) != EOF) {
			fputc(current, output);
		}
	}
	fclose(input);
	return 1;
}
