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
      if (g.cell(gx, gy) == Dominos::HL) set_edge({ci - 1, cj, 0}, Losange);
      if (g.cell(gx, gy) == Dominos::VT) set_edge({ci, cj - 1, 1}, Losange);
      if (g.cell(gx + 1, gy) == Dominos::HL) set_edge({ci - 1, cj, 2}, Losange);
      const int gx2 = size_ - y + 2 * x - 1;
      const int gy2 = g.size() + size_ - y - 2;
      const int ci2 = 2 * size_ - 1 - y;
      const int cj2 = 1 + x;
      if (g.cell(gx2, gy2 + 1) == Dominos::HL)
        set_edge({ci2, cj2 - 1, 2}, Losange);
      if (g.cell(gx2 + 1, gy2 + 1) == Dominos::HL)
        set_edge({ci2, cj2, 0}, Losange);
      if (g.cell(gx2 + 2, gy2) == Dominos::VT) set_edge({ci2, cj2, 1}, Losange);
    }
    // On the end
    const int gx = 3 * size_ + y - 1;
    const int gy = g.size() - size_ + y;
    if (g.cell(gx, gy) == Dominos::VT)
      set_edge({1 + y, 2 * size_ - 1, 1}, Losange);

    const int gx2 = size_ - y - 1;
    const int gy2 = g.size() + size_ - y - 2;
    if (g.cell(gx2, gy2) == Dominos::VT)
      set_edge({2 * size_ - 1 - y, 0, 1}, Losange);
  }
}

void Grid::init_empty() {
  for (int i = 0; i < 2 * size_; i++) {
    const int str = start_j(i);
    const int end = end_j(i);
    for (int j = str; j < end; j++) {
      if (j > str || i < size_) set_edge({i, j, 0}, None);
      if (i > 0) set_edge({i, j, 1}, None);
      if (j < end - 1 || i < size_) set_edge({i, j, 2}, None);
    }
  }
}

void Grid::segmentify_seg(const Coord &c) {
  const EdgesNei nei = get_edg_nei(c);

  if ((is_edg_inside(nei.e[0]) && is_edg_inside(nei.e[2]) &&
       edge(nei.e[0]) == Losange && edge(nei.e[2]) == Losange) ||
      (is_edg_inside(nei.e[1]) && is_edg_inside(nei.e[3]) &&
       edge(nei.e[1]) == Losange && edge(nei.e[3]) == Losange)) {
    set_edge(c, Edge);
  }
}

void Grid::segmentify() {
  for (int i = 0; i < 2 * size_; i++) {
    const int str = start_j(i);
    const int end = end_j(i);
    for (int j = str; j < end; j++) {
      if (j > str || i < size_) segmentify_seg({i, j, 0});
      if (i > 0) segmentify_seg({i, j, 1});
      if (j < end - 1 || i < size_) segmentify_seg({i, j, 2});
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

bool Grid::is_edg_inside(const Coord &c) const {
  const int str = start_j(c.i);
  const int end = end_j(c.i);
  return 0 <= c.i && c.i < 2 * size_ && str <= c.j && c.j < end &&
         ((c.d == 0 && (c.j > str || c.i < size_)) || (c.d == 1 && c.i > 0) ||
          (c.d == 2 && (c.j < end - 1 || c.i < size_)));
}

EdgesNei Grid::get_edg_nei(const Coord &c) {
  EdgesNei res;

  res.e[0].d = (c.d + 1) % 3;
  res.e[0].i = c.i;
  res.e[0].j = c.j;

  res.e[1].d = (c.d + 2) % 3;
  res.e[1].i = c.i;
  res.e[1].j = c.j;

  if (c.d == 0) {
    res.e[2].d = 2;
    res.e[2].i = c.i;
    res.e[2].j = c.j - 1;

    res.e[3].d = 1;
    res.e[3].i = c.i + 1;
    res.e[3].j = c.j - 1;
  } else if (c.d == 1) {
    res.e[2].d = 0;
    res.e[2].i = c.i - 1;
    res.e[2].j = c.j + 1;

    res.e[3].d = 2;
    res.e[3].i = c.i - 1;
    res.e[3].j = c.j;
  } else {
    res.e[2].d = 1;
    res.e[2].i = c.i + 1;
    res.e[2].j = c.j;

    res.e[3].d = 0;
    res.e[3].i = c.i;
    res.e[3].j = c.j + 1;
  }
  return res;
}

int Grid::size() const { return size_; }

CalissonEdge Grid::edge(const Coord &c) const { return grid_[linearise(c)]; }

void Grid::set_edge(const Coord &c, CalissonEdge e) { grid_[linearise(c)] = e; }

int Grid::linearise(const Coord &c) const {
  return c.d + 3 * c.j + 6 * size_ * c.i;
}

std::ostream &operator<<(std::ostream &os, const Grid &c) {
  os << (char)(c.size() + 4 * 0x20);
  for (int i = 0; i < 2 * c.size_; i++) {
    const int str = c.start_j(i);
    const int end = c.end_j(i);
    for (int j = str; j < end; j++) {
      if (j > str || i < c.size_) os << (char)c.edge({i, j, 0});
      if (i > 0) os << (char)c.edge({i, j, 1});
      if (j < end - 1 || i < c.size_) os << (char)c.edge({i, j, 2});
    }
  }
  return os;
}