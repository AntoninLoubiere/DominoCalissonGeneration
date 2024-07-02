#include "weighted_grid.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <ostream>

#include "grid.hpp"

typedef struct {
  int dx;
  int dy;
} Dir;

const Dir DIRS[4] = {{0, 0}, {0, 1}, {1, 2}, {1, 1}};
const double INV_SQRT2 = 0.7071067811865475244008443622;

WeightedGrid::WeightedGrid(int size) : size_(size), grid_(size * size){};

WeightedGrid weighted_grid_all(int size) {
  WeightedGrid wg(size);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < 4; k++) {
        wg.grid_[i * size + j].cells[k] = 1;
      }
    }
  }
  return wg;
}

WeightedGrid weighted_grid_empty(int size) {
  WeightedGrid wg(size);
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < 4; k++) {
        wg.grid_[i * size + j].cells[k] = 0;
      }
    }
  }
  return wg;
}

WeightedGrid weighted_grid_square(int size) {
  WeightedGrid wg = weighted_grid_all(size);

  const int sq_size = size / 2;
  for (int x = 0; x < sq_size; x++) {
    wg.grid_[wg.linearise(x, sq_size - 1)].cells[1] = 0;
    wg.grid_[wg.linearise(x, sq_size - 1)].cells[3] = 0;
  }

  for (int i = 0; i < sq_size; i++) {
    const int y = size + 2 * i - sq_size;
    wg.grid_[wg.linearise(i, y)].cells[0] = 0;
    wg.grid_[wg.linearise(i, y)].cells[2] = 0;
    wg.grid_[wg.linearise(y - i, y)].cells[0] = 0;
    wg.grid_[wg.linearise(y - i, y)].cells[2] = 0;
  }

  for (int x = size - sq_size; x < size; x++) {
    wg.grid_[wg.linearise(x, size - sq_size - 1)].cells[1] = 0;
    wg.grid_[wg.linearise(x, size - sq_size - 1)].cells[3] = 0;
  }

  return wg;
}

WeightedGrid weighted_grid_import_square(int size, std::istream &is) {
  WeightedGrid wg = weighted_grid_square(size);
  const int sq_size = size / 2;

  for (int y = sq_size; y < 2 * size - sq_size - 1; y++) {
    for (int x = sq_size; x < 2 * size - sq_size; x++) {
      is >> *wg.vertical_segment(x, y);
    }
  }

  for (int y = sq_size; y < 2 * size - sq_size; y++) {
    for (int x = sq_size; x < 2 * size - sq_size - 1; x++) {
      is >> *wg.horizontal_segment(x, y);
    }
  }
  return wg;
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
  double coeffs[4];
  for (int didx = 0; didx < 4; didx++) {
    const Dir &d = DIRS[(didx + 2) % 4];
    coeffs[didx] = wg->cell(x + d.dx, y + d.dy)[didx];
  }

  const double dp = coeffs[0] * coeffs[2] + coeffs[1] * coeffs[3];
  const double tot = coeffs[0] + coeffs[1] + coeffs[2] + coeffs[3];
  if (dp != 0) {
    for (int d = 0; d < 4; d++) {
      c[d] = coeffs[d] / dp;
    }
  } else if (tot != 0) {
    for (int d = 0; d < 4; d++) {
      c[d] = coeffs[d] / tot;
    }
  } else {
    for (int d = 0; d < 4; d++) {
      c[d] = INV_SQRT2;
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
    if (val < v * (RAND_MAX + 1.)) {
      g.set_square_vertical(gx, gy);
    } else {
      g.set_square_horizontal(gx, gy);
    }
  }
}

Grid WeightedGrid::get_random_weighted_grid() {
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
    std::cout << s << std::endl;
    std::cout << new_g;
  }
  return new_g;
}
