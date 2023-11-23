#include <iostream>

#include "gnode.hpp"

// ----------------------------------------
// Nodes definition, all derivates from gnode::Node
// ----------------------------------------

enum MyTypes : int
{
  tfloat
};

struct Value : gnode::Node
{
  float value;

  Value(std::string id, float value) : gnode::Node(id), value(value)
  {
    this->add_port(
        gnode::Port("output", gnode::direction::out, MyTypes::tfloat));
    this->update_inner_bindings();
  }

  void set_value(float new_value)
  {
    this->value = new_value;
    this->force_update();
  }

  void compute()
  {
    // this node does actually nothing, it is just a container for a
    // float value
    std::cout << id.c_str() << ": computing node...\n";
  }

  void update_inner_bindings()
  {
    this->set_p_data("output", (void *)&(this->value));
  }
};

struct Add : gnode::Node
{
  float value_add; // output value

  Add(std::string id) : gnode::Node(id)
  {
    this->add_port(
        gnode::Port("input 1", gnode::direction::in, MyTypes::tfloat));
    this->add_port(
        gnode::Port("input 2", gnode::direction::in, MyTypes::tfloat));
    this->add_port(
        gnode::Port("output", gnode::direction::out, MyTypes::tfloat));
    this->update_inner_bindings();
  }

  void compute()
  {
    std::cout << id.c_str() << ": computing node...\n";

    // retrieve and recast inputs
    float *p_in1 = static_cast<float *>(this->get_p_data("input 1"));
    float *p_in2 = static_cast<float *>(this->get_p_data("input 2"));

    // and add the inputs
    this->value_add = *p_in1 + *p_in2;
  }

  void update_inner_bindings()
  {
    // inputs are set when links between nodes are created
    this->set_p_data("output", (void *)&(this->value_add));
  }
};

struct Print : gnode::Node
{
  Print(std::string id) : gnode::Node(id)
  {
    this->add_port(gnode::Port("input", gnode::direction::in, MyTypes::tfloat));
    // NB - no inner binding, since there is only one input
  }

  void compute()
  {
    std::cout << id.c_str() << ": computing node...\n";

    // retrieve and recast inputs
    float *p_in = static_cast<float *>(this->get_p_data("input"));
    std::cout << ">>>>>> PRINT: " << *p_in << "\n";
  }
};

void my_callback(gnode::Node *p_node)
{
  std::cout << "node: " << p_node->id.c_str() << ", ";
  std::cout << "CALLBACK! I just have been updated\n";
}

// ----------------------------------------
// MAIN
// ----------------------------------------

int main()
{
  gnode::Tree tree = gnode::Tree("calculator");

  tree.add_node(std::make_shared<Value>("value a", 1.f));
  tree.add_node(std::make_shared<Value>("value b", 2.f));
  tree.add_node(std::make_shared<Add>("add 1"));

  tree.add_node(std::make_shared<Value>("value c", 3.f));
  tree.add_node(std::make_shared<Add>("add 2"));

  tree.add_node(std::make_shared<Print>("print"));

  // a + b
  tree.link("value a", "output", "add 1", "input 1");
  tree.link("value b", "output", "add 1", "input 2");

  // (a + b) + c
  tree.link("add 1", "output", "add 2", "input 1");
  tree.link("value c", "output", "add 2", "input 2");

  // and print
  tree.link("add 2", "output", "print", "input");

  // compute (PRINT should print '6')
  tree.update();

  tree.infos();

  tree.print_node_links();

  // change 'value c', this will automatically update only necessary
  // nodes (not all the tree), and PRINT should print 10

  // NB1 - base gnode methods can be accessed directly. This one can be
  // used to a callback called after the node has been updated
  tree.get_node_ref_by_id("value c")->set_post_update_callback(
      (std::function<void(gnode::Node *)>)&my_callback);

  // NB2 - to access the methods of the derived class, the node
  // reference needs to be recasted (using a template already
  // implemented)
  tree.get_node_ref_by_id<Value>("value c")->set_value(7.f);

  tree.get_node_ref_by_hash_id<Value>(231603572)->set_value(10.f);

  return 0;
}
