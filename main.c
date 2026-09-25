#include <err.h>
#include "img_loading.h"

int main(int argc, char **argv) {
    if (argc != 2)
        errx(1, "Incorrect number of paramters");
    (void *) argv;

    return 0;
}
