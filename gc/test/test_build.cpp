#include "build/config.hpp"
#include "build/parse_config.hpp"

#include "common/format.hpp"

#include <yaml-cpp/yaml.h>

#include <gtest/gtest.h>


TEST(Build_Config, LoadFromYaml)
{
    constexpr auto config_text = R"(
paths:
    cxx_compiler_executable: "g++"
    linker_executable: "ld"
compile_flags:
    common: "-Wall"
    debug: "-g"
    release: "-O3"
link_flags:
    common: "-lm"
    shared: "--shared"
)";

    constexpr auto expected_config_str =
        "{paths={cxx_compiler_executable=g++,linker_executable=ld},"
        "compile_flags={common=-Wall,debug=-g,release=-O3},"
        "link_flags={common=-lm,shared=--shared}}";

    auto config_yaml = YAML::Load(config_text);
    auto config = build::parse_config(config_yaml);

    EXPECT_EQ(config.paths.cxx_compiler_executable, "g++");
    EXPECT_EQ(config.paths.linker_executable, "ld");
    EXPECT_EQ(config.compile_flags.common, "-Wall");
    EXPECT_EQ(config.compile_flags.debug, "-g");
    EXPECT_EQ(config.compile_flags.release, "-O3");
    EXPECT_EQ(config.link_flags.common, "-lm");
    EXPECT_EQ(config.link_flags.shared, "--shared");

    EXPECT_EQ(common::format(config), expected_config_str);
}
