/*
 * html.h
 *
 *  Created on: 07.08.2017
 *      Author: julian
 */

#ifndef HTML_H_
#define HTML_H_

#include <stdio.h> // needs FILE
#include "nhtml_string.h"
#include "attribute.h"

extern int html_escape(int c, FILE* output);

typedef struct node {
	string_t name;
	attr_set_t attributes;
} node_t;


/**
 * \brief emit html opening tag for \node
 * \param node the Node to create the opening tag for
 * \param output the File to write to
 */
void open_node(node_t * node, FILE * output);

/**
 * \brief emit html closing tag for \node
 * \param node the Node to create the closing tag for
 * \param output the File to write to
 */
void close_node(node_t * node, FILE * output);

#endif /* HTML_H_ */
