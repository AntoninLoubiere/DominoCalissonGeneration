#pragma once
#include <ostream>
#include <vector>

namespace Dominos {
class Grid;
};

std::ostream &operator<<(std::ostream &os, const Dominos::Grid &g);

namespace Dominos {

enum GridElement { Ext, None, VT, VB, HL, HR };

class Grid {
 public:
  Grid(int size);
  // Create a new grid of size size, and copy the content of the grid g
  Grid(int size, const Grid &g);

  GridElement cell(int x, int y) const;
  GridElement safe_cell(int x, int y) const;
  void set_cell(int x, int y, GridElement el);

  friend std::ostream & ::operator<<(std::ostream & os, const Grid & g);

  int start_x(int y) const;
  int end_x(int y) const;

  bool is_horizontal(int x, int y) const;
  void set_horizontal(int x, int y);
  bool is_vertical(int x, int y) const;
  void set_vertical(int x, int y);

  void set_square_vertical(int x, int y);
  void set_square_horizontal(int x, int y);

  int size() const;

  void to_svg(std::ostream &os) const;

 private:
  int linearise(int x, int y) const;

  int size_;
  std::vector<GridElement> grid_;
};

Grid get_random_grid(int size);
}  // namespace Dominos

std::ostream &operator<<(std::ostream &os, Dominos::GridElement el);
