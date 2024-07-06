#pragma once
#include <ostream>
#include <vector>

#include "grid.hpp"
namespace Calissons {
class Grid;
}
std::ostream& operator<<(std::ostream& os, const Calissons::Grid&);

namespace Calissons {
enum CalissonEdge { None = 0, Edge = 1, EdgeVar = 2, Losange = 3, Ext };

typedef struct {
  int i;
  int j;
  int d;
} Coord;

typedef struct {
  Coord e[4];
} EdgesNei;

class Grid {
 public:
  Grid(int size);
  Grid(const Dominos::Grid& g);

  CalissonEdge edge(const Coord& c) const;
  void set_edge(const Coord& c, CalissonEdge e);

  void init_empty();

  void segmentify();

  int start_j(int i) const;
  int end_j(int i) const;
  bool is_edg_inside(const Coord& c) const;
  static EdgesNei get_edg_nei(const Coord& c);

  int size() const;

  void to_image_blueprint(std::ostream& os);

  friend std::ostream& ::operator<<(std::ostream & os, const Grid&);

 private:
  void segmentify_seg(const Coord& c);
  void to_image_blueprint_seg(std::ostream& os, const Coord& c);

  int linearise(const Coord& c) const;

  int size_;
  std::vector<CalissonEdge> grid_;
};
}  // namespace Calissons