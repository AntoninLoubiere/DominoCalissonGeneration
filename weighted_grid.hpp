#pragma once
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
  friend std::ostream &operator<<(std::ostream &os, const WeightedGrid &g);

  int start_x(int y) const;
  int end_x(int y) const;

  int size() const;

 private:
  int linearise(int x, int y) const;

  int size_;
  std::vector<WeightedCell> grid_;
};

WeightedGrid get_prev_weighted_grid(WeightedGrid &wg);
WeightedGrid weighted_grid_square(int size);
WeightedGrid weighted_grid_all(int size);
Grid get_random_weighted_grid(WeightedGrid wg);
