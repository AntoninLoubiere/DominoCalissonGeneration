#pragma once
#include <istream>
#include <ostream>
#include <vector>

#include "grid.hpp"

typedef struct {
  double cells[4];
} WeightedCell;

class WeightedGrid {
 public:
  WeightedGrid(int size);

  double *cell(int x, int y);

  void set_constant(int val);
  void set_square();
  void import_inside_square(std::istream &is);

  int start_x(int y) const;
  int end_x(int y) const;

  double *vertical_segment(int tx, int ty);
  double *horizontal_segment(int tx, int ty);

  WeightedGrid get_prev_weighted_grid();
  Grid get_random_weighted_grid(std::ostream *os = NULL);

  int size() const;
  friend std::ostream &operator<<(std::ostream &os, const WeightedGrid &g);

 private:
  int linearise(int x, int y) const;
  void update_wgrid_from_previous(Grid &g, int wx, int wy, int gx, int gy,
                                  const Grid &prev);
  void update_weighted_grid_cell(int x, int y, WeightedGrid *wg);

  int size_;
  std::vector<WeightedCell> grid_;
};
