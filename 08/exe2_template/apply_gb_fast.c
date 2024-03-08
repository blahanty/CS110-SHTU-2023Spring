#include "main.h"

Image transpose(Image a) {
    /* img_sc() will return a copy of the given image */
    Image b = img_sc(a);
    b.dimX = a.dimY;
    b.dimY = a.dimX;
    for (int x = 0; x < (int) a.dimX; x += 20) {
        for (int y = 0; y < (int) a.dimY; ++y) {
            for (int x1 = 0; x1 < 20 && x + x1 < (int) a.dimX; ++x1) {
                b.data[(x + x1) * b.dimX + y] = a.data[y * a.dimX + x + x1];
            }
        }
    }
    return b;
}

Image apply_gb(Image a, FVec gv) {
    return transpose(gb_h(transpose(gb_h(a, gv)), gv));
}
