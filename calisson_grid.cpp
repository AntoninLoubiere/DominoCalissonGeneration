#include "calisson_grid.hpp"

#include <cassert>

#include "grid.hpp"
#include "weighted_grid.hpp"

const char *BASE64_CONVERTER =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

using namespace Calissons;

Grid::Grid(int size) : size_(size), grid_(12 * size * size, Ext) {}

Grid::Grid(const Dominos::Grid &g)
    : Grid(Dominos::hex_size_from_wg_size(g.size())) {
  init_empty();
  for (int y = 0; y < size_; y++) {
    for (int x = 0; x < y + size_; x++) {
      const int gx = size_ - y + 2 * x - 1;
      const int gy = g.size() - size_ + y;
      const int ci = 1 + y;
      const int cj = size_ - y + x;
      if (g.cell(gx, gy) == Dominos::HL)
        grid_[linearise(ci - 1, cj, 0)] = Losange;
      if (g.cell(gx, gy) == Dominos::VT)
        grid_[linearise(ci, cj - 1, 1)] = Losange;
      if (g.cell(gx + 1, gy) == Dominos::HL)
        grid_[linearise(ci - 1, cj, 2)] = Losange;
      const int gx2 = size_ - y + 2 * x - 1;
      const int gy2 = g.size() + size_ - y - 2;
      const int ci2 = 2 * size_ - 1 - y;
      const int cj2 = 1 + x;
      if (g.cell(gx2, gy2 + 1) == Dominos::HL)
        grid_[linearise(ci2, cj2 - 1, 2)] = Losange;
      if (g.cell(gx2 + 1, gy2 + 1) == Dominos::HL)
        grid_[linearise(ci2, cj2, 0)] = Losange;
      if (g.cell(gx2 + 2, gy2) == Dominos::VT)
        grid_[linearise(ci2, cj2, 1)] = Losange;
    }
    // On the end
    const int gx = 3 * size_ + y - 1;
    const int gy = g.size() - size_ + y;
    if (g.cell(gx, gy) == Dominos::VT)
      grid_[linearise(1 + y, 2 * size_ - 1, 1)] = Losange;

    const int gx2 = size_ - y - 1;
    const int gy2 = g.size() + size_ - y - 2;
    if (g.cell(gx2, gy2) == Dominos::VT)
      grid_[linearise(2 * size_ - 1 - y, 0, 1)] = Losange;
  }
}

void Grid::init_empty() {
  for (int i = 0; i < 2 * size_; i++) {
    const int str = start_j(i);
    const int end = end_j(i);
    for (int j = str; j < end; j++) {
      if (j > str || i < size_) grid_[linearise(i, j, 0)] = None;
      if (i > 0) grid_[linearise(i, j, 1)] = None;
      if (j < end - 1 || i < size_) grid_[linearise(i, j, 2)] = None;
    }
  }
}

int Grid::start_j(int i) const {
  if (i > size_) return 0;
  return size_ - i;
}

int Grid::end_j(int i) const {
  if (i > size_) return 3 * size_ - i;
  return 2 * size_;
}

int Grid::size() const { return size_; }

int Grid::linearise(int i, int j, int dir) const {
  return dir + 3 * j + 6 * size_ * i;
}

std::ostream &operator<<(std::ostream &os, const Grid &c) {
  os << (char)(c.size() + 4 * 0x20);
  for (int i = 0; i < 2 * c.size_; i++) {
    const int str = c.start_j(i);
    const int end = c.end_j(i);
    for (int j = str; j < end; j++) {
      if (j > str || i < c.size_) os << (char)c.grid_[c.linearise(i, j, 0)];
      if (i > 0) os << (char)c.grid_[c.linearise(i, j, 1)];
      if (j < end - 1 || i < c.size_) os << (char)c.grid_[c.linearise(i, j, 2)];
    }
  }
  return os;
}