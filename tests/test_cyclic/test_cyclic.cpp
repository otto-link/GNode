#include "gnode.hpp"
#include <iostream>

int main()
{

  std::vector<std::vector<size_t>> adj =
      {{1, 2}, {0, 2}, {1, 2}, {2, 0}, {2, 3}, {3, 3}};

  std::cout << gnode::is_graph_cyclic(adj) << " (expect 1)\n";

  adj = {{1}, {2}, {}};

  std::cout << gnode::is_graph_cyclic(adj) << " (expect 0)\n";

  return 0;
}
