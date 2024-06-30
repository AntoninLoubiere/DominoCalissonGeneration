#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>

// #include "grid.hpp"
#include "weighted_grid.hpp"

int main() {
  //   std::cout << "Taille grille" << std ::endl;
  int taille;
  std::cin >> taille;
  srand(time(NULL));

  //   Grid g(1);
  //   if (rand() % 2 == 0)
  //     g.set_square_horizontal(0, 0);
  //   else
  //     g.set_square_vertical(0, 0);

  //   g = Grid(4, g);
  //   g = Grid(g);
  //   g = Grid(g);
  //   std::cout << g;
  //   Grid g = get_random_grid(taille);
  //   std::cout << g;
  //   g.to_svg(std::cout);
  WeightedGrid wg = weighted_grid_square(taille);
  //   std::cout << wg << "\n";
  std::cout << get_random_weighted_grid(wg);
}
