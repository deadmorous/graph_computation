#include "build/build.hpp"
#include "build/config.hpp"
#include "build/parse_config.hpp"
#include "build/scratch_dir.hpp"

#include "dlib/module.hpp"
#include "dlib/symbol.hpp"

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
    gclib: "-Dgclib_EXPORTS"
link_flags:
    common: "-lm"
    shared: "-Wl,-shared"
    shared_unwrapped: "-shared"
flag_rules:
    linker_wrapper_flag: "-Wl,"
    linker_wrapper_flag_sep: ","
    include_flag: "-I"
    include_system_flag: "-isystem "
gclib_include_dirs: "/an/include/dir"
)";

    constexpr auto expected_config_str =
        "{paths={cxx_compiler_executable=g++,linker_executable=ld},"
        "compile_flags={common=-Wall,shared=,debug=-g,release=-O3,"
        "gclib=-Dgclib_EXPORTS},link_flags={common=-lm,shared=-Wl,-shared,"
        "shared_unwrapped=-shared},flag_rules={linker_wrapper_flag=-Wl,,"
        "linker_wrapper_flag_sep=,,include_flag=-I,"
        "include_system_flag=-isystem },gclib_include_dirs=/an/include/dir}";

    auto config_yaml = YAML::Load(config_text);
    auto config = build::parse_config(config_yaml);

    EXPECT_EQ(config.paths.cxx_compiler_executable, "g++");
    EXPECT_EQ(config.paths.linker_executable, "ld");
    EXPECT_EQ(config.compile_flags.common, "-Wall");
    EXPECT_EQ(config.compile_flags.debug, "-g");
    EXPECT_EQ(config.compile_flags.release, "-O3");
    EXPECT_EQ(config.compile_flags.gclib, "-Dgclib_EXPORTS");
    EXPECT_EQ(config.link_flags.common, "-lm");
    EXPECT_EQ(config.link_flags.shared, "-Wl,-shared");
    EXPECT_EQ(config.link_flags.shared_unwrapped, "-shared");
    EXPECT_EQ(config.flag_rules.linker_wrapper_flag, "-Wl,");
    EXPECT_EQ(config.flag_rules.linker_wrapper_flag_sep, ",");
    EXPECT_EQ(config.flag_rules.include_flag, "-I");
    EXPECT_EQ(config.flag_rules.include_system_flag, "-isystem ");
    EXPECT_EQ(config.gclib_include_dirs, "/an/include/dir");

    EXPECT_EQ(common::format(config), expected_config_str);
    // std::cout << config << std::endl;
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

TEST(Build, Executable)
{
    auto scratch_dir = build::ScratchDir{};
    auto hw_src = R"(
#include <iostream>
int main()
{
    std::cout << "Hello World\n";
    return 0;
}
)";
    const auto& scratch = scratch_dir.path();
    auto config = build::default_config();
    auto input = scratch_dir.path() / "hw.cpp";
    std::ofstream(input) << hw_src;
    auto output = scratch_dir.path() / "hw";
    build::build(config, output, scratch_dir, build::InputVec{input});
    testing::internal::CaptureStdout();
    std::ignore = std::system(output.c_str());
    auto text = testing::internal::GetCapturedStdout();
    EXPECT_EQ(text, "Hello World\n");
}

TEST(Build, SharedObject)
{
    auto scratch_dir = build::ScratchDir{};
    auto add_src = R"(
extern "C" int add(int a, int b)
{
    return a + b;
}
)";
    const auto& scratch = scratch_dir.path();
    auto config = build::default_config();
    auto input = scratch_dir.path() / "hw.cpp";
    std::ofstream(input) << add_src;
    auto output = scratch_dir.path() / "hw.so";
    build::build(
        config,
        output,
        scratch_dir,
        build::InputVec{input},
        { .output_type = build::OutputType::SharedObject });

    auto module = dlib::Module{ output };
    auto add_symbol = module.symbol(dlib::SymbolNameView{"add"});
    auto add = add_symbol.as<int(int,int)>();
    EXPECT_EQ(add(2, 3), 5);
}
