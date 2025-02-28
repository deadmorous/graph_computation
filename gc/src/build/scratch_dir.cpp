#include "build/scratch_dir.hpp"

#include "common/format.hpp"
#include "common/throw.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <random>


namespace build {

namespace {

struct Hex final
{
    unsigned int value;
};

auto operator<<(std::ostream& s, Hex h)
    -> std::ostream&
{ return s << std::hex << h.value; }

} // anonymous namespace

namespace fs = std::filesystem;

ScratchDir::ScratchDir(std::string_view name_prefix):
    ScratchDir{ {}, name_prefix }
{}

ScratchDir::ScratchDir(const fs::path& parent_dir,
                       std::string_view name_prefix)
{
    if (name_prefix.empty())
        name_prefix = "temp_dir";

    fs::path temp_dir = parent_dir.empty()
        ? fs::temp_directory_path()
        : parent_dir;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> dis(0, 65535);

    // Retry a few times
    for (int i = 0; i < 10; ++i)
    {
        auto subdir_name = common::format(name_prefix, '_', Hex{dis(gen)});
        fs::path unique_dir = temp_dir / subdir_name;
        if (fs::create_directories(unique_dir))
        {
            path_ = std::move(unique_dir);
            assert(unique_dir.empty());
            assert(!path_.empty());
            return;
        }
    }
    common::throw_(
        "Failed to create a unique temporary directory "
        "after several attempts.");
}

ScratchDir::~ScratchDir()
{
    if (empty())
        return;

    try
    {
        fs::remove_all(path_);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error removing directory: " << e.what() << std::endl;
    }
}

auto ScratchDir::empty() const noexcept
    -> bool
{ return path_.empty(); }

auto ScratchDir::detach() noexcept
    -> void
{ path_.clear(); }

auto ScratchDir::path() const noexcept
    -> const fs::path&
{ return path_; }

ScratchDir::operator const fs::path&() const noexcept
{ return path_; }

} // namespace build
