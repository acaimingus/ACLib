#include "ACBash.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>

TEST(ACBash, nocmd)
{
    ASSERT_THROW(ACBash::fromString(""), std::invalid_argument);
}

TEST(ACBash, echo)
{
    std::cout << ACBash::fromString("echo Hello, World!");
}

TEST(ACBash, createFile) {
    ACBash::fromString("touch test.txt");
    ASSERT_TRUE(std::filesystem::exists("test.txt"));
}

TEST(ACBash, removeFile) {
    ACBash::fromString("rm test.txt");
    ASSERT_FALSE(std::filesystem::exists("test.txt"));
}