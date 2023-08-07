#include "gnode.hpp"

int main()
{

  // empty port
  gnode::Port p = gnode::Port();
  p.infos();

  // non-empty
  p = gnode::Port("input##hidden_id",
                  gnode::direction::in,
                  gnode::dtype::dfloat,
                  gnode::optional::yes);
  p.infos();

  return 0;
}
