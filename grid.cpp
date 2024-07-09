#include "grid.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace Dominos;

Grid::Grid(int size) : size_(size), grid_((2 * size_) * (2 * size_), Ext) {
  for (int y = 0; y < 2 * size_; y++) {
    for (int x = start_x(y); x < end_x(y); x++) {
      set_cell(x, y, None);
    }
  }
}

Grid::Grid(int size, const Grid &g)
    : size_(size), grid_((2 * size_) * (2 * size_), Ext) {
  assert(size >= g.size_);
  const int size_diff = size_ - g.size_;
  for (int y = 0; y < size_diff; y++) {
    for (int x = start_x(y); x < end_x(y); x++) {
      set_cell(x, y, None);
      set_cell(x, 2 * size - y - 1, None);
    }
  }

  for (int y = size_diff; y < 2 * size_ - size_diff; y++) {
    const int start = start_x(y);
    const int end = end_x(y);

    for (int i = 0; i < size_diff; i++) {
      set_cell(start + i, y, None);
      set_cell(end - i - 1, y, None);
    }

    for (int x = start + size_diff; x < end - size_diff; x++) {
      set_cell(x, y, g.cell(x - size_diff, y - size_diff));
    }
  }
}

GridElement Grid::cell(int x, int y) const { return grid_[linearise(x, y)]; }

GridElement Grid::safe_cell(int x, int y) const {
  if (x < 0 || x >= 2 * size_ || y < 0 || y >= 2 * size_) return Ext;
  return grid_[linearise(x, y)];
}

void Grid::set_cell(int x, int y, GridElement el) {
  grid_[linearise(x, y)] = el;
}

int Grid::start_x(int y) const {
  if (y >= size_) return y - size_;
  return size_ - y - 1;
}

int Grid::end_x(int y) const { return 2 * size_ - start_x(y); }

bool Grid::is_horizontal(int x, int y) const {
  return cell(x, y) == HL && cell(x + 1, y) == HR;
}

void Grid::set_horizontal(int x, int y) {
  set_cell(x, y, HL);
  set_cell(x + 1, y, HR);
}

bool Grid::is_vertical(int x, int y) const {
  return cell(x, y) == VT && cell(x, y + 1) == VB;
}

void Grid::set_vertical(int x, int y) {
  set_cell(x, y, VT);
  set_cell(x, y + 1, VB);
}

void Grid::set_square_horizontal(int x, int y) {
  set_horizontal(x, y);
  set_horizontal(x, y + 1);
}

void Grid::set_square_vertical(int x, int y) {
  set_vertical(x, y);
  set_vertical(x + 1, y);
}

std::ostream &operator<<(std::ostream &os, const Grid &g) {
  //   os << "Taille: " << g.size_ << "\n";

  for (int y = 0; y < 2 * g.size_; y++) {
    for (int x = 0; x < 2 * g.size_; x++) {
      os << g.grid_[g.linearise(x, y)];
    }
    os << "\n";
  }

  return os;
}

int Grid::size() const { return size_; }

int Grid::linearise(int x, int y) const { return 2 * y * size_ + x; }

std::ostream &operator<<(std::ostream &os, GridElement el) {
  switch (el) {
    case Ext:
      return os << ' ';

    case None:
      return os << '.';

    case VT:
      return os << '^';
    case VB:
      return os << 'v';

    case HL:
      return os << '<';
    case HR:
      return os << '>';

    default:
      return os;
  }
}

void update_grid_from_previous(Grid &g, int x, int y, const Grid &prev) {
  if (prev.is_horizontal(x, y) && prev.is_horizontal(x, y + 1)) {
    return;
  } else if (prev.is_horizontal(x, y)) {
    g.set_horizontal(x, y + 1);
  } else if (prev.is_horizontal(x, y + 1)) {
    g.set_horizontal(x, y);
  } else if (prev.is_vertical(x, y) && prev.is_vertical(x + 1, y)) {
    return;
  } else if (prev.is_vertical(x, y)) {
    g.set_vertical(x + 1, y);
  } else if (prev.is_vertical(x + 1, y)) {
    g.set_vertical(x, y);
  } else if (rand() % 2 == 0) {
    g.set_square_vertical(x, y);
  } else {
    g.set_square_horizontal(x, y);
  }
}

typedef struct {
  int x;
  int y;
} SquarePos;

// std::vector<SquarePos> get_positions(int size, int s) {
//   std::vector<SquarePos> pos(s * s);
//   const int offset = size - s;
//   for (int i = 0; i < s - 1; i++) {
//     for (int j = 0; j <= i; j++) {
//       pos.push_back({.x = size + 2 * j - i - 1, .y = offset + i});
//       pos.push_back({.x = size + 2 * j - i - 1, .y = offset + 2 * s - i -
//       2});
//     }
//   }
//   for (int j = 0; j < s; j++) {
//     pos.push_back({.x = offset + 2 * j, .y = size - 1});
//   }
//   return pos;
// };

Grid Dominos::get_random_grid(int size, std::ostream *os) {
  Grid g1 = Grid(size);
  Grid g2 = Grid(size);
  Grid &prev_g = g1;
  Grid &new_g = g2;
  for (int s = 1; s <= size; s++) {
    Grid &temp = prev_g;
    prev_g = new_g;
    new_g = temp;

    const int offset = size - s;
    for (int i = 0; i < s - 1; i++) {
      for (int j = 0; j <= i; j++) {
        update_grid_from_previous(new_g, size + 2 * j - i - 1, offset + i,
                                  prev_g);
        update_grid_from_previous(new_g, size + 2 * j - i - 1,
                                  offset + 2 * s - i - 2, prev_g);
      }
    }

    for (int j = 0; j < s; j++) {
      update_grid_from_previous(new_g, offset + 2 * j, size - 1, prev_g);
    }

    if (os != NULL) {
      *os << s << "\n";
      *os << new_g;
    }
  }
  return new_g;
}

void Grid::to_svg(std::ostream &os) const {
  const int SIZE = 20;
  const int MARGIN = 0;
  const int INSIDE_SIZE = SIZE - 2 * MARGIN;
  os << "<svg viewBox=\"0 0 " << SIZE * 2 * size_ << " " << SIZE * 2 * size_
     << "\" xmlns=\"http://www.w3.org/2000/svg\">";

  for (int x = 0; x < 2 * size_; x++) {
    for (int y = 0; y < 2 * size_; y++) {
      const GridElement c = cell(x, y);
      if (c == VT) {
        os << "<rect x=\"" << SIZE * x + MARGIN << "\" y=\""
           << SIZE * y + MARGIN << "\" width=\"" << INSIDE_SIZE
           << "\" height=\"" << INSIDE_SIZE + SIZE << "\" fill=\""
           << ((x + y) % 2 == 0 ? "#F00000" : "#FFD700") << "\"/>";
      } else if (c == HL) {
        os << "<rect x=\"" << SIZE * x + MARGIN << "\" y=\""
           << SIZE * y + MARGIN << "\" width=\"" << INSIDE_SIZE + SIZE
           << "\" height=\"" << INSIDE_SIZE << "\" fill=\""
           << ((x + y) % 2 == 0 ? "#0000CE" : "#00D1D1") << "\"/>";
      }
    }
  }

  os << "</svg>";
}
