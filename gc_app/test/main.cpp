/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "mpk/mix/log.hpp"

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    mpk::mix::start_logging();

    mpk::mix::set_log_level(mpk::mix::LogLevel::Debug);

    return RUN_ALL_TESTS();
}
