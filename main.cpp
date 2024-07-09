#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ostream>

// #include "grid.hpp"
#include "weighted_grid.hpp"
using namespace Dominos;

int main(int argc, char *argv[]) {
  //   std::cout << "Taille grille" << std ::endl;
  int taille;
  std::cin >> taille;
  srand(clock());

  //   Grid g(1);
  //   if (rand() % 2 == 0)
  //     g.set_square_horizontal(0, 0);
  //   else
  //     g.set_square_vertical(0, 0);

  //   g = Grid(4, g);
  //   g = Grid(g);
  //   g = Grid(g);
  // //   std::cout << g;
  // std::cout << taille << "\n";
  // Grid g = get_random_grid(taille, &std::cout);
  // std::cout << g;
  // g.to_svg(std::cout);
  WeightedGrid wg(taille);
  wg.set_constant(1);
  wg.remove_hex();
  // wg.import_inside_square(std::cin);
  // //   std::cout << wg << "\n";
  // std::cout << wg.get_random_weighted_grid();

  // WeightedGrid wg(taille);
  // wg.set_constant(1);
  // wg.remove_square();
  // wg.import_inside_square(std::cin);
  // wg.get_random_weighted_grid();
  // std::cout << taille << "\n";
  std::cout << wg.get_random_weighted_grid() << std::endl;
}
