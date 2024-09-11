#include "common/log.hpp"

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    common::start_logging();

    common::set_log_level(common::LogLevel::Debug);

    return RUN_ALL_TESTS();
}
