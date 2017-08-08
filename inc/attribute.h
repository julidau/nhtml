/*
 * attribute.h
 *
 *  Created on: 08.08.2017
 *      Author: julian
 */

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include <errno.h>

#include "nhtml_string.h"


typedef struct {
	string_t name, value;
} attr_t;

/**
 * \brief copy the give attribute
 * \return a copy of attr
 */
attr_t attr_copy(attr_t * attr);
/**
 * \brief reset the attribute
 * Deletes all strings
 */
void attr_destroy(attr_t * attr);


typedef struct {
	attr_t * arr;
	size_t len;
} attr_set_t;

/**
 * \brief search in attribute set for key
 * \param set The Attribute set
 * \param key The Key to search
 * \return the pointer to the attribute in the set mathching the key
 * \return NULL if there is no matching attribute
 */
attr_t * attr_set_find(attr_set_t *set, const char * key);
// Append new Attribute to set
/**
 * \brief Append new attribute pair to set
 * \return 0 on success
 * \return -1 on failure (error can be found to errno)
 */
int attr_set_append(attr_set_t * set, attr_t *new_entry);


#endif /* ATTRIBUTE_H_ */
