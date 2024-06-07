#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>

#include "buffer.h"
#include "class.h"
#include "result.h"

result parse_class(buffer *buf, class_file *class);

/* TODO: free */

#endif
