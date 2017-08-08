/*
 * html.c
 *
 *  Created on: 07.08.2017
 *      Author: julian
 */

#include "html.h"

int html_escape(int c, FILE * output) {
	switch(c) {
	case '<':
	 	fprintf(output, "&lt;");
	 	goto escaped;
	case '>':
		fprintf(output, "&gt;");
		goto escaped;
	case '&':
		fprintf(output, "&amp;");
		goto escaped;
	case '"':
		fprintf(output, "&quot;");
		goto escaped;
	/*case ' ':
		fprintf(output, "&nbsp;");
		goto escaped;*/
	case '\n':
		fprintf(output, "<br/>");
		goto escaped;
	}

	return 0;
escaped:
	return 1;
}

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



