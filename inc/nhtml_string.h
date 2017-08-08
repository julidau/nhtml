/*
 * nhtml_string.h
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#ifndef NHTML_STRING_H_
#define NHTML_STRING_H_

#include <stddef.h> // size_t
#include <errno.h> 	// errno

typedef struct {
	char * c_str;
	size_t len;
} string_t;

/**\brief append the char @c to @str
 * \param str to the String to append to
 * \param c the char to append
 * \return -1 on error, errno will be set to errorcode
 * \return 0 on success
 */
int string_append(string_t *str, char c);

/**
 * \brief Erase the String from memory
 */
void string_destroy(string_t s);

/**
 * \brief copy the contents of a string
 * \param old The String to copy
 * \return returns the new string
 */
string_t string_copy(string_t old);


#endif /* NHTML_STRING_H_ */
