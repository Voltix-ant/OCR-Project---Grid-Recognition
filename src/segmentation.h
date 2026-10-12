#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "matrix.h"
#include "region.h"
#include "projection.h"
#include <stdlib.h>

// Functions

void find_subregions(
        struct matrix *m, struct Region *p, enum cut_direction cut_dir,
        int threshold
);
void segment_region(struct matrix *m, struct Region *p,
                    enum cut_direction cut_dir, int threshold, int stalled);

#endif
