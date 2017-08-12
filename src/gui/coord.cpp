#include <gui/coord.hpp>

Coord::Coord(long double x, long double y) {
    this->x = x;
    this->y = y;
}

Coord::Coord() {
    Coord(0, 0);
}
