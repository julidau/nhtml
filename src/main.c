#include <stdio.h>  // needs: fgetc, fputs, fopen, fprintf
#include <stdlib.h> // needs: abort
#include <errno.h>
#include <string.h> // needs: sterror
#include <ctype.h> 	// needs: isspace

#include <getopt.h> // needs: getopt_long

// Project specific includes
#include <html.h>
#include <attribute.h>
#include <nhtml_string.h>
#include <includes.h>

// globals

int verbose = 0;

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
			attr_destroy(&current_attr);
			isKey = 1;
			continue;
		}
		if (isKey && buffer == '.') {
			isKey = 0;
			current_attr.name = string_from_cstr("class");
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
#ifdef DEBUG
	printf("parsed attr: %s=%s\n", current_attr.name.c_str, current_attr.value);
#endif

	attr_destroy(&current_attr);
	return strip(stream);
}

int parse_text(int end_char, FILE * stream, FILE * output) {
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
		if (!escaped && buffer == end_char) {
			break;
		}
		escaped = 0;
		if (end_char == '"' && html_escape(buffer, output)) {
			continue;
		}

		fputc(buffer, output);
	}

	return strip(stream);
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

// Parse comments
int parse_comment(int current, FILE * input) {
	current = fgetc(input);
	if (current == '/') {
		// line comment
		while((current = fgetc(input)) != EOF) {
			if (current == '\n') break;
		}
	} else
	if (current == '*') {
		// block comment
		unsigned char comment_done = 0;
		while((current = fgetc(input)) != EOF) {
			if (current == '*') {
				comment_done = 1;
			} else
			if (comment_done && current == '/') {
				break;
			} else
				comment_done = 0;
		}
	}

	return strip(input);
}

int parse_include(FILE * input, FILE * output) {
	// handle include
	int current = fgetc(input);
	string_t filename = {};

	// two possibilities
	// 1. include with "" <= supports arbitrary paths
	// 2. include without ""
	//    reads till a nonpath, char (!{")
	// 	  then strips empty chars from start and end of path
	// both methods then try to find the file in the include paths
	if (current == EOF) return current;

	if (current == '"') { // Method 1
		char escaped = 0;
		while((current = fgetc(input)) != EOF) {
			if (!escaped && current == '\\') {
				escaped = 1;
				continue;
			}
			if (!escaped && current == '"') {
				break;
			}

			escaped = 0;
			string_append(&filename, current);
		}
	} else {
		string_append(&filename, current);
		while((current = fgetc(input)) != EOF) {
			switch(current) {
			case '{':
			case '!':
			case '"':
			case '}':
				break;
			}
			if (isspace(current)) {
				break;
			}

			string_append(&filename, current);
		}
	}

	if (filename.c_str == NULL) {
		fprintf(stderr, "include command without filename!\n");
		return strip(input);
	}

	// handle filename
	if (verbose) printf("include file %s\n", filename.c_str);

	if (!include_file(&filename, output)) {
		fprintf(stderr, "could not include file %s\n", filename.c_str);
	}

	return strip(input);
}

int parse_node(int current, FILE * stream, FILE * output) {
	if (current == '"' || current == '(') {
		if (current == '(')	current = ')';
		return parse_text(current, stream, output);
	} else
	if (current == '/') {
		return parse_comment(current, stream);
	} else
	if (current == '@') {
		return parse_include(stream, output);
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



// long options
static struct option long_options[] = {
		{"output", required_argument, 0, 'o'},
		{"help", no_argument, 0, '?'},
		{}
};


void usage(int argc, char ** args) {
	printf("usage: %s [-o <filename>] file [file...]\n", args[0]);
	printf("-I\tAdd Path to include paths\n");
	printf("--output\n");
	printf("-o\tThe output file to write the html to\n");
	printf("\tWhen missing this option, stdout is used instead\n");
	printf("-v\tEnable verbose output\n");
	printf("--help Print this usage\n");
}

int main(int argc, char ** args) {
	int i = 0;
	FILE* output = NULL;

	// parse arguments
	while((i = getopt_long(argc, args, "o:I:v", long_options, NULL)) != -1) {
		switch(i) {
		case 'o':
			output = fopen(optarg, "w");
			if (output == NULL) {
				fprintf(stderr, "could not create output file: %s (%s)\n", optarg, strerror(errno));
				return -1;
			}
			break;
		case 'I':
			// add include dir
			include_add_path(optarg);
			break;
		case 'v':
			verbose = 1;
			break;
		case '?':
			usage(argc, args);
			return -1;
		default:
			// should not be reachable
			abort();
		}
	}

	if (output == NULL) {
		output = stdout;
		// disable output when printing to stdout
		verbose = 0;
	}

	char * filename= NULL;

	// parse all the files
	for(i = optind;i < argc; i++) {
		filename = args[i];
		if (verbose) printf("starting conversion of %s\n", filename);
		
		FILE * handle = fopen(filename, "r");

		if (handle == NULL) {
			fprintf(stderr, "could not open \"%s\": %s\n", filename, strerror(errno));
			continue;
		}
		
		int current = strip(handle);

		// parse the complete file
		while((current = parse_node(current, handle, output)) != EOF);

		fclose(handle);
	}

	if (output != stdout) fclose(output);
	if (verbose) printf("done compiling\n");
	return 0;
}
