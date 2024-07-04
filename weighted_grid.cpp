#include "weighted_grid.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <ostream>

#include "grid.hpp"

typedef struct {
  int dx;
  int dy;
} Dir;

const Dir DIRS[4] = {{0, 0}, {0, 1}, {1, 2}, {1, 1}};
const double INV_SQRT2 = 0.7071067811865475244008443622;

WeightedGrid::WeightedGrid(int size) : size_(size), grid_(size * size){};

void WeightedGrid::set_constant(int val) {
  for (int i = 0; i < size_; i++) {
    for (int j = 0; j < size_; j++) {
      for (int k = 0; k < 4; k++) {
        grid_[i * size_ + j].cells[k] = val;
      }
    }
  }
}

void WeightedGrid::remove_square() {
  const int sq_size = size_ / 2;
  for (int x = 0; x < sq_size; x++) {
    grid_[linearise(x, sq_size - 1)].cells[1] = 0;
    grid_[linearise(x, sq_size - 1)].cells[3] = 0;
  }

  for (int i = 0; i < sq_size; i++) {
    const int y = size_ + 2 * i - sq_size;
    grid_[linearise(i, y)].cells[0] = 0;
    grid_[linearise(i, y)].cells[2] = 0;
    grid_[linearise(y - i, y)].cells[0] = 0;
    grid_[linearise(y - i, y)].cells[2] = 0;
  }

  for (int x = size_ - sq_size; x < size_; x++) {
    grid_[linearise(x, size_ - sq_size - 1)].cells[1] = 0;
    grid_[linearise(x, size_ - sq_size - 1)].cells[3] = 0;
  }
}

void WeightedGrid::remove_hex() {
  const int hex_size = hex_size_from_wg_size(size_);

  for (int i = 0; i < 2 * hex_size - 1; i++) {
    *vertical_segment(hex_size + i, size_ - hex_size - 1) = 0;
    *vertical_segment(hex_size + i, size_ + hex_size - 1) = 0;
  }

  for (int y = 0; y < hex_size; y++) {
    *horizontal_segment(3 * hex_size - 1 + y, size_ - hex_size + y) = 0;
    *vertical_segment(3 * hex_size - 1 + y, size_ - hex_size + y - 1) = 0;
    *horizontal_segment(3 * hex_size - 1 + y, size_ + hex_size - y - 1) = 0;
    *vertical_segment(3 * hex_size - 1 + y, size_ + hex_size - y - 1) = 0;

    for (int x = 0; x < y + hex_size; x++) {
      *vertical_segment(hex_size - y + 2 * x, size_ - hex_size + y) = 0;
      *vertical_segment(hex_size - y + 2 * x, size_ + hex_size - y - 2) = 0;
    }
  }
}

void WeightedGrid::import_inside_square(std::istream &is) {
  const int sq_size = size_ / 2;

  for (int y = sq_size; y < 2 * size_ - sq_size - 1; y++) {
    for (int x = sq_size; x < 2 * size_ - sq_size; x++) {
      is >> *vertical_segment(x, y);
    }
  }

  for (int y = sq_size; y < 2 * size_ - sq_size; y++) {
    for (int x = sq_size; x < 2 * size_ - sq_size - 1; x++) {
      is >> *horizontal_segment(x, y);
    }
  }
}

double *WeightedGrid::cell(int x, int y) {
  return grid_[linearise(x, y)].cells;
}

std::ostream &operator<<(std::ostream &os, const WeightedGrid &g) {
  for (int y = 0; y < g.size_; y++) {
    for (int x = 0; x <= y; x++) {
      os << "[";
      for (int d = 0; d < 4; d++) {
        os << g.grid_[g.linearise(x, y)].cells[d] << ", ";
      }
      os << "] ";
    }
    os << "\n";
  }

  for (int y = 0; y < g.size_ - 1; y++) {
    for (int x = y + 1; x < g.size_; x++) {
      os << "[";
      for (int d = 0; d < 4; d++) {
        os << g.grid_[g.linearise(x, y)].cells[d] << ", ";
      }
      os << "] ";
    }
    os << "\n";
  }
  return os;
}

const int CELL_SIZE = 20;
const int SPACE_BETWEEN = 26;
const int EL_SIZE = CELL_SIZE + SPACE_BETWEEN;
void cell_svg(std::ostream &os, int x, int y) {
  os << "<rect x=\"" << x * EL_SIZE << "\" y=\"" << y * EL_SIZE << "\" width=\""
     << CELL_SIZE << "\" height=\"" << CELL_SIZE << "\"/>";
}

void cell_link(std::ostream &os, int fromx, int fromy, int tox, int toy,
               float val) {
  os << "<line stroke=\"black\" x1=\"" << EL_SIZE * fromx + CELL_SIZE / 2
     << "\" y1=\"" << EL_SIZE * fromy + CELL_SIZE / 2 << "\" x2=\""
     << EL_SIZE * tox + CELL_SIZE / 2 << " \" y2=\""
     << EL_SIZE * toy + CELL_SIZE / 2 << "\" opacity=\"" << (val >= 1 ? 1 : val)
     << "\"><title>" << val << "</title></line>";

  os << "<text font-size=\"10\" text-anchor=\"start\" x=\""
     << EL_SIZE * (fromx + tox) / 2 + CELL_SIZE / 2 + 2 << "\" y =\""
     << EL_SIZE * (fromy + toy) / 2 + CELL_SIZE / 2 - 2 << "\">" << val
     << "</text>";
}

void cell_activated(std::ostream &os, int x, int y) {
  os << "<rect x=\"" << x * EL_SIZE + CELL_SIZE / 2 << "\" y=\""
     << y * EL_SIZE + CELL_SIZE / 2 << "\" width=\"" << EL_SIZE
     << "\" height=\"" << EL_SIZE << "\" fill=\"lightgray\"/>";
}

void WeightedGrid::to_svg(std::ostream &os) {
  const int TOTAL_SIZE = (2 * size_) * EL_SIZE;
  os << "<svg viewBox=\"0 0 " << TOTAL_SIZE << " " << TOTAL_SIZE
     << "\" xmlns=\"http://www.w3.org/2000/svg\">";

  for (int y = 0; y < size_; y++) {
    for (int x = 0; x <= y; x++) {
      const int cx = 2 * x + size_ - y - 1;
      double *cell = grid_[linearise(x, y)].cells;
      cell_activated(os, cx, y);
      cell_link(os, cx, y, cx + 1, y, cell[0]);
      cell_link(os, cx, y, cx, y + 1, cell[1]);
      cell_link(os, cx, y + 1, cx + 1, y + 1, cell[2]);
      cell_link(os, cx + 1, y, cx + 1, y + 1, cell[3]);
    }
  }

  for (int y = 0; y < size_ - 1; y++) {
    for (int x = 0; x < size_ - y - 1; x++) {
      const int cx = 2 * x + y + 1;
      const int cy = y + size_;
      double *cell = grid_[linearise(x + y + 1, y)].cells;
      cell_activated(os, cx, cy);
      cell_link(os, cx, cy, cx + 1, cy, cell[0]);
      cell_link(os, cx, cy, cx, cy + 1, cell[1]);
      cell_link(os, cx, cy + 1, cx + 1, cy + 1, cell[2]);
      cell_link(os, cx + 1, cy, cx + 1, cy + 1, cell[3]);
    }
  }

  for (int y = 0; y < size_; y++) {
    for (int x = size_ - y - 1; x < size_; x++) {
      cell_svg(os, x, y);
      cell_svg(os, 2 * size_ - x - 1, y);
      cell_svg(os, x, 2 * size_ - y - 1);
      cell_svg(os, 2 * size_ - x - 1, 2 * size_ - y - 1);
    }
  }

  os << "</svg>";
}

int WeightedGrid::start_x(int y) const {
  if (y < size_) return 0;
  return y - size_ + 1;
}

int WeightedGrid::end_x(int y) const {
  if (y < size_) return y + 1;
  return size_;
}

double *WeightedGrid::vertical_segment(int tx, int ty) {
  if (ty < 0 || ty >= 2 * size_ - 1) return NULL;

  if (ty < size_) {
    const int stx = size_ - ty - 1;
    const int endx = size_ + ty + 1;
    if (tx < stx || tx >= endx) return NULL;
    return cell((tx - stx) / 2, ty) + 1 + 2 * ((tx - stx) % 2);
  } else {
    const int stx = ty - size_ + 1;
    const int endx = 3 * size_ - ty - 1;
    if (tx < stx || tx >= endx) return NULL;
    return cell(ty - size_ + 1 + (tx - stx) / 2, ty) + 1 + 2 * ((tx - stx) % 2);
  }
}

double *WeightedGrid::horizontal_segment(int tx, int ty) {
  if (ty < 0 || ty >= 2 * size_) return NULL;

  if (ty < size_) {
    const int stx = size_ - ty - 1;
    const int endx = size_ + ty;
    if (tx < stx || tx >= endx) return NULL;
    return cell((tx - stx) / 2, ty - (tx - stx) % 2) + 2 * ((tx - stx) % 2);
  } else {
    const int stx = ty - size_;
    const int endx = 3 * size_ - ty - 1;
    if (tx < stx || tx >= endx) return NULL;
    const int y = ty - 1 + (tx - stx) % 2;
    return cell(y - size_ + 1 + (tx - stx) / 2, y) + 2 * ((tx - stx + 1) % 2);
  }
}

int WeightedGrid::size() const { return size_; }

int WeightedGrid::linearise(int x, int y) const {
  return (y % size_) * size_ + x;
}

void WeightedGrid::update_weighted_grid_cell(int x, int y, WeightedGrid *wg) {
  double *c = cell(x, y);
  for (int d = 0; d < 4; d++) {
    // Edge-erasing
    const int d_suiv = (d + 1) % 4;
    const Dir &dir_suiv = DIRS[d_suiv];
    const int d_prev = (d - 1) % 4;
    const Dir &dir_prev = DIRS[d_prev];
    if ((wg->cell(x + dir_suiv.dx, y + dir_suiv.dy)[d] == 0 &&
         wg->cell(x + dir_suiv.dx, y + dir_suiv.dy)[d_prev] == 0) ||
        (wg->cell(x + dir_prev.dx, y + dir_prev.dy)[d] == 0 &&
         wg->cell(x + dir_prev.dx, y + dir_prev.dy)[d_suiv] == 0)) {
      c[d] = 0;
    } else {
      // Pre-computation
      const Dir &dir = DIRS[d];
      const double *prev_cell = wg->cell(x + dir.dx, y + dir.dy);
      const float dp =
          prev_cell[0] * prev_cell[2] + prev_cell[1] * prev_cell[3];
      const float tot =
          prev_cell[0] + prev_cell[1] + prev_cell[2] + prev_cell[3];
      if (dp != 0) {
        c[d] = prev_cell[d] / dp;
      } else if (tot != 0) {
        c[d] = prev_cell[d] == 0 ? 1. / tot : prev_cell[d] / tot;
      } else {
        c[d] = INV_SQRT2;
      }
    }
  }
}

WeightedGrid WeightedGrid::get_prev_weighted_grid() {
  const int size = size_ - 1;
  WeightedGrid nwg(size);

  for (int y = 0; y < size; y++) {
    for (int x = 0; x <= y; x++) {
      nwg.update_weighted_grid_cell(x, y, this);
    }

    for (int x = y + 1; x < size; x++) {
      nwg.update_weighted_grid_cell(x, y + size, this);
    }
  }

  return nwg;
}

void WeightedGrid::update_wgrid_from_previous(Grid &g, int wx, int wy, int gx,
                                              int gy, const Grid &prev) {
  if (prev.is_horizontal(gx, gy) && prev.is_horizontal(gx, gy + 1)) {
    return;
  } else if (prev.is_horizontal(gx, gy)) {
    g.set_horizontal(gx, gy + 1);
  } else if (prev.is_horizontal(gx, gy + 1)) {
    g.set_horizontal(gx, gy);
  } else if (prev.is_vertical(gx, gy) && prev.is_vertical(gx + 1, gy)) {
    return;
  } else if (prev.is_vertical(gx, gy)) {
    g.set_vertical(gx + 1, gy);
  } else if (prev.is_vertical(gx + 1, gy)) {
    g.set_vertical(gx, gy);
  } else {
    const double *c = cell(wx, wy);
    const double h = c[0] * c[2];
    const double v = c[1] * c[3];
    const double dp = h + v;
    const double val = dp * rand();
    assert(dp != 0);
    if (/*h == 0 || */ val < v * (RAND_MAX + 1.)) {
      g.set_square_vertical(gx, gy);
    } else {
      g.set_square_horizontal(gx, gy);
    }
  }
}

Grid WeightedGrid::get_random_weighted_grid(std::ostream *os) {
  Grid g1 = Grid(size_);
  Grid g2 = Grid(size_);
  Grid &prev_g = g1;
  Grid &new_g = g2;

  std::vector<WeightedGrid> wgs(size_, WeightedGrid(0));
  wgs[size_ - 1] = *this;
  for (int i = size_ - 2; i >= 0; i--) {
    wgs[i] = wgs[i + 1].get_prev_weighted_grid();
  }

  for (int s = 1; s <= size_; s++) {
    Grid &temp = prev_g;
    prev_g = new_g;
    new_g = temp;
    WeightedGrid wg = wgs[s - 1];

    const int offset = size_ - s;
    for (int i = 0; i < s - 1; i++) {
      for (int j = 0; j <= i; j++) {
        wg.update_wgrid_from_previous(new_g, j, i, size_ + 2 * j - i - 1,
                                      offset + i, prev_g);
        wg.update_wgrid_from_previous(new_g, j + s - i - 1, 2 * s - i - 2,
                                      size_ + 2 * j - i - 1,
                                      offset + 2 * s - i - 2, prev_g);
      }
    }

    for (int j = 0; j < s; j++) {
      wg.update_wgrid_from_previous(new_g, j, s - 1, offset + 2 * j, size_ - 1,
                                    prev_g);
    }

    if (os != NULL) *os << s << std::endl << new_g;
  }
  return new_g;
}

int hex_size_from_wg_size(int s) { return (s + 1) / 3; }

int wg_size_from_hex_size(int s) { return 3 * s - 1; }