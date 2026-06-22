#include <gtest/gtest.h>

#include "gnode.hpp"

void global_init() {}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  global_init();

  gnode::Logger::log()->set_level(spdlog::level::off);

  return RUN_ALL_TESTS();
}
