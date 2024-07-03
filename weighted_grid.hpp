#pragma once
#include <istream>
#include <ostream>
#include <vector>

#include "grid.hpp"

typedef struct {
  double cells[4];
} WeightedCell;

int hex_size_from_wg_size(int s);
int wg_size_from_hex_size(int s);

class WeightedGrid {
 public:
  WeightedGrid(int size);

  double *cell(int x, int y);

  void set_constant(int val);
  void remove_square();
  void remove_hex();
  void import_inside_square(std::istream &is);

  int start_x(int y) const;
  int end_x(int y) const;

  double *vertical_segment(int tx, int ty);
  double *horizontal_segment(int tx, int ty);

  WeightedGrid get_prev_weighted_grid();
  Grid get_random_weighted_grid(std::ostream *os = NULL);

  int size() const;
  friend std::ostream &operator<<(std::ostream &os, const WeightedGrid &g);

  void to_svg(std::ostream &os);

 private:
  int linearise(int x, int y) const;
  void update_wgrid_from_previous(Grid &g, int wx, int wy, int gx, int gy,
                                  const Grid &prev);
  void update_weighted_grid_cell(int x, int y, WeightedGrid *wg);

  int size_;
  std::vector<WeightedCell> grid_;
};
