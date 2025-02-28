#include "build/config.hpp"
#include "build/parse_config.hpp"
#include "build/scratch_dir.hpp"

#include "common/format.hpp"

#include <yaml-cpp/yaml.h>

#include <gtest/gtest.h>

#include <fstream>


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

TEST(Build_Util, ScratchDir)
{
    namespace fs = std::filesystem;
    auto scratchdir_path = fs::path{};
    auto file_path = fs::path{};
    {
        auto scratch_dir = build::ScratchDir{};
        scratchdir_path = scratch_dir.path();
        EXPECT_TRUE(fs::exists(scratchdir_path));

        file_path = scratchdir_path / "hello.txt";
        auto content_written = std::string{"Hello"};
        std::ofstream(file_path) << content_written;

        EXPECT_TRUE(fs::exists(file_path));

        std::ifstream s(file_path);
        auto content_read = std::string{"Hello"};
        getline(s, content_read);
        EXPECT_EQ(content_written, content_read);
    }

    EXPECT_FALSE(fs::exists(scratchdir_path));
    EXPECT_FALSE(fs::exists(file_path));
}
