/*
 * parser.h
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#ifndef INC_PARSER_H_
#define INC_PARSER_H_

#include <stdio.h>

int strip(FILE * input);
int parse_node(int current, FILE * stream, FILE * output);


#endif /* INC_PARSER_H_ */
