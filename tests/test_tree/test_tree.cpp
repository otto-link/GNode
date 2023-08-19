#include <iostream>

#include "gnode.hpp"

struct MyNode : public gnode::Node
{
  MyNode(std::string id) : gnode::Node(id) {}

  void compute() { std::cout << id.c_str() << ": computing node...\n"; }
};

void my_callback(gnode::Node *p_node)
{
  std::cout << "node: " << p_node->id.c_str() << "\n";
  std::cout << "CALLBACK!\n";
}

int main()
{

  // 1st node 'A'
  MyNode node = MyNode("nodeA");

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
  MyNode node2 = MyNode("node_2");

  {
    gnode::Port p = gnode::Port("output",
                                gnode::direction::out,
                                gnode::dtype::dfloat);
    node2.add_port(p);

    node2.set_post_update_callback(
        (std::function<void(gnode::Node *)>)&my_callback);
  }

  MyNode node3 = MyNode("node_3");
  {
    gnode::Port p = gnode::Port("output",
                                gnode::direction::out,
                                gnode::dtype::dfloat);
    node3.add_port(p);

    node3.set_post_update_callback(
        (std::function<void(gnode::Node *)>)&my_callback);
  }

  // tree
  gnode::Tree tree = gnode::Tree("mytree");

  tree.add_node(std::make_shared<MyNode>(node));
  tree.add_node(std::make_shared<MyNode>(node2));
  tree.add_node(std::make_shared<MyNode>(node3));

  tree.link("node_2", "output", "nodeA", "input2");
  tree.link("node_3", "output", "nodeA", "input##hidden_id");

  tree.print_node_list();
  tree.print_node_links();

  tree.update();

  std::vector<std::vector<size_t>> g = tree.get_adjacency_matrix();

  for (size_t i = 0; i < g.size(); i++)
  {
    std::cout << i << ": ";
    for (size_t j = 0; j < g[i].size(); j++)
      std::cout << g[i][j] << ", ";
    std::cout << std::endl;
  }

  return 0;
}
