#include "gnode.hpp"
#include "nodesoup.hpp"
#include <iostream>

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

void my_callback_pre(gnode::Node *p_node)
{
  std::cout << "node: " << p_node->id.c_str() << "\n";
  std::cout << "CALLBACK! (pre-update)\n";
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
    node3.set_pre_update_callback(
        (std::function<void(gnode::Node *)>)&my_callback_pre);
    node3.set_post_update_callback(
        (std::function<void(gnode::Node *)>)&my_callback);
  }

  MyNode node4 = MyNode("node_4");
  {
    gnode::Port p = gnode::Port("input",
                                gnode::direction::in,
                                gnode::dtype::dfloat);
    node4.add_port(p);
  }

  // tree
  gnode::Tree tree = gnode::Tree("mytree");

  tree.add_node(std::make_shared<MyNode>(node));
  tree.add_node(std::make_shared<MyNode>(node2));
  tree.add_node(std::make_shared<MyNode>(node3));
  tree.add_node(std::make_shared<MyNode>(node4));

  tree.link("node_2", "output", "nodeA", "input2");
  tree.link("node_3", "output", "nodeA", "input##hidden_id");
  tree.link("nodeA", "output", "node_4", "input");

  tree.print_node_list();
  tree.print_node_links();

  tree.update();

  tree.get_node_ref_by_id("node_3")->force_update();

  std::cout << "Node layout (Fruchterman Reingold):\n";
  std::vector<gnode::Point> positions =
      tree.compute_graph_layout_fruchterman_reingold();

  for (auto &p : positions)
    std::cout << p.x << " " << p.y << "\n";

  //
  std::cout << "Node layout (Sugiyama):\n";
  positions = tree.compute_graph_layout_sugiyama();

  for (auto &p : positions)
    std::cout << p.x << " " << p.y << "\n";

  return 0;
}
