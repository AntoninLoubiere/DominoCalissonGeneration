#include "weighted_grid.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

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

int WeightedGrid::size() const { return size_; }

int WeightedGrid::linearise(int x, int y) const {
  return (y % size_) * size_ + x;
}

void update_weighted_grid_cell(WeightedGrid &nwg, int x, int y,
                               WeightedGrid &wg) {
  double *cell = nwg.cell(x, y);
  double coeffs[4];
  for (int didx = 0; didx < 4; didx++) {
    const Dir &d = DIRS[(didx + 2) % 4];
    coeffs[didx] = wg.cell(x + d.dx, y + d.dy)[didx];
  }

  const double dp = coeffs[0] * coeffs[2] + coeffs[1] * coeffs[3];
  const double tot = coeffs[0] + coeffs[1] + coeffs[2] + coeffs[3];
  if (dp != 0) {
    for (int d = 0; d < 4; d++) {
      cell[d] = coeffs[d] / dp;
    }
  } else if (tot != 0) {
    for (int d = 0; d < 4; d++) {
      cell[d] = coeffs[d] / tot;
    }
  } else {
    for (int d = 0; d < 4; d++) {
      cell[d] = INV_SQRT2;
    }
  }
}

WeightedGrid get_prev_weighted_grid(WeightedGrid &wg) {
  const int size = wg.size() - 1;
  WeightedGrid nwg(size);

  for (int y = 0; y < size; y++) {
    for (int x = 0; x <= y; x++) {
      update_weighted_grid_cell(nwg, x, y, wg);
    }

    for (int x = y + 1; x < size; x++) {
      update_weighted_grid_cell(nwg, x, y + size, wg);
    }
  }

  return nwg;
}

void update_wgrid_from_previous(Grid &g, WeightedGrid &wg, int wx, int wy,
                                int gx, int gy, const Grid &prev) {
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
    const double *cell = wg.cell(wx, wy);
    const double h = cell[0] * cell[2];
    const double v = cell[1] * cell[3];
    const double dp = h + v;
    const double val = dp * rand();
    assert(dp != 0);
    if (val < v * RAND_MAX) {
      g.set_square_vertical(gx, gy);
    } else {
      g.set_square_horizontal(gx, gy);
    }
  }
}

Grid get_random_weighted_grid(WeightedGrid wg) {
  int size = wg.size();
  Grid g1 = Grid(size);
  Grid g2 = Grid(size);
  Grid &prev_g = g1;
  Grid &new_g = g2;

  std::vector<WeightedGrid> wgs(size, WeightedGrid(0));
  wgs[size - 1] = wg;
  for (int i = size - 2; i >= 0; i--) {
    wgs[i] = get_prev_weighted_grid(wgs[i + 1]);
  }

  for (int s = 1; s <= size; s++) {
    Grid &temp = prev_g;
    prev_g = new_g;
    new_g = temp;
    WeightedGrid wg = wgs[s - 1];

    const int offset = size - s;
    for (int i = 0; i < s - 1; i++) {
      for (int j = 0; j <= i; j++) {
        update_wgrid_from_previous(new_g, wg, j, i, size + 2 * j - i - 1,
                                   offset + i, prev_g);
        update_wgrid_from_previous(new_g, wg, j + s - i - 1, 2 * s - i - 2,
                                   size + 2 * j - i - 1, offset + 2 * s - i - 2,
                                   prev_g);
      }
    }

    for (int j = 0; j < s; j++) {
      update_wgrid_from_previous(new_g, wg, j, s - 1, offset + 2 * j, size - 1,
                                 prev_g);
    }
  }
  return new_g;
}
