#include "gnode.hpp"

int main()
{

  // 1st node 'A'
  gnode::Node node = gnode::Node("nodeA");

  {
    gnode::Port p1 = gnode::Port("input##hidden_id",
                                 gnode::direction::in,
                                 gnode::dtype::dfloat,
                                 gnode::optional::yes);

    gnode::Port p2 = gnode::Port("input2",
                                 gnode::direction::in,
                                 gnode::dtype::dfloat,
                                 gnode::optional::no);

    gnode::Port p3 = gnode::Port("output",
                                 gnode::direction::out,
                                 gnode::dtype::dfloat);
    node.add_port(p1);
    node.add_port(p2);
    node.add_port(p3);
  }

  node.infos();

  node.treeview();

  // other node
  gnode::Node node2 = gnode::Node("node_2");

  {
    gnode::Port p = gnode::Port("output",
                                gnode::direction::out,
                                gnode::dtype::dfloat);
    node2.add_port(p);
  }

  // tree
  gnode::Tree tree = gnode::Tree("mytree");

  tree.add_node(std::make_shared<gnode::Node>(node));
  tree.add_node(std::make_shared<gnode::Node>(node2));

  tree.link("node_2", "output", "nodeA", "input2");

  tree.print_node_list();
  tree.print_node_links();

  tree.update();

  return 0;
}
