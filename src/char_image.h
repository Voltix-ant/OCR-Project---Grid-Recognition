#ifndef CHAR_IMAGE_H
#define CHAR_IMAGE_H

#include "components.h"
#include "matrix.h"
#include <math.h>

#define CHAR_RATIO 28

// Functions

struct matrix *letter_to_28(struct matrix *img, struct box b);

#endif
