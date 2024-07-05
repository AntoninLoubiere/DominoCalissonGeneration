#pragma once
#include <ostream>
#include <vector>

#include "grid.hpp"
namespace Calissons {
class Grid;
}
std::ostream& operator<<(std::ostream& os, const Calissons::Grid&);

namespace Calissons {
enum CalissonEdge { None = 0, Edge = 1, EdgeVar = 2, Losange = 3, Ext = 4 };

class Grid {
 public:
  Grid(int size);
  Grid(const Dominos::Grid& g);

  void init_empty();

  int start_j(int i) const;
  int end_j(int i) const;

  int size() const;

  friend std::ostream& ::operator<<(std::ostream & os, const Grid&);

 private:
  int linearise(int i, int j, int dir) const;

  int size_;
  std::vector<CalissonEdge> grid_;
};
}  // namespace Calissons