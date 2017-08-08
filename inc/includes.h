/*
 * includes.h
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#ifndef INC_INCLUDES_H_
#define INC_INCLUDES_H_

#include <nhtml_string.h>
#include <stdio.h>

void include_add_path(const char * path);

int include_file(string_t *filename, FILE * output);

#endif /* INC_INCLUDES_H_ */
