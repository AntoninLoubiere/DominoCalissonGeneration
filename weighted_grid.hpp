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
  //   WeightedGrid(WeightedGrid &wg);

  double *cell(int x, int y);

  friend WeightedGrid weighted_grid_square(int size);
  friend WeightedGrid weighted_grid_all(int size);
  friend WeightedGrid weighted_grid_empty(int size);
  friend std::ostream &operator<<(std::ostream &os, const WeightedGrid &g);

  int start_x(int y) const;
  int end_x(int y) const;

  double *vertical_segment(int tx, int ty);
  double *horizontal_segment(int tx, int ty);

  WeightedGrid get_prev_weighted_grid();
  Grid get_random_weighted_grid();

  int size() const;

 private:
  int linearise(int x, int y) const;
  void update_wgrid_from_previous(Grid &g, int wx, int wy, int gx, int gy,
                                  const Grid &prev);
  void update_weighted_grid_cell(int x, int y, WeightedGrid *wg);

  int size_;
  std::vector<WeightedCell> grid_;
};

WeightedGrid weighted_grid_square(int size);
WeightedGrid weighted_grid_all(int size);
WeightedGrid weighted_grid_empty(int size);
WeightedGrid weighted_grid_import_square(int size, std::istream &is);
