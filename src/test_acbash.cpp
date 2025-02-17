#include "ACBash.hpp"
#include <gtest/gtest.h>

TEST(ACBash, nocmd)
{
    ASSERT_THROW(ACBash::fromString(""), std::invalid_argument);
}

TEST(ACBash, echo)
{
    ACBash::fromString("echo Hello, World!");
}

TEST(ACBash, ls)
{
    ACBash::fromString("ls");
}