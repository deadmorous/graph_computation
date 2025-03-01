#include "dlib/module.hpp"

#include "dlib/symbol.hpp"

#include "common/throw.hpp"

#include <dlfcn.h>


namespace dlib {

struct Module::Impl
{
    static constexpr int flags_ = RTLD_NOW | RTLD_LOCAL;

    Impl(std::filesystem::path path):
        path_{ std::move(path) },
        handle{ dlopen(path_.c_str(), flags_) }
    {
        if (!handle)
            common::throw_("Failed to load module ", path_, ": ", dlerror());
    }

    Impl(const Impl&) = delete;
    auto operator=(const Impl&) -> Impl& = delete;

    ~Impl()
    {
        if (dlclose(handle) != 0)
            std::cerr << "WARNING: Error unloading module "
                      << path_ << ": " << dlerror() << std::endl;
    }

    auto symbol(SymbolName::View name) const
        -> Symbol
    {
        auto name_str = std::string{name.v};
        dlerror();
        auto address = dlsym(handle, name_str.c_str());
        if (!address)
            common::throw_(
                "Failed to find symbol '", name, "' in module ", path_, ": ",
                dlerror());
        return { common::Impl, address };
    }

    std::filesystem::path path_;
    void* handle;
};

Module::Module() noexcept = default;

Module::Module(const std::filesystem::path& path):
    impl_{ std::make_unique<Impl>(path) }
{}

Module::~Module() = default;

auto Module::empty() const noexcept
    -> bool
{ return impl_.get() == nullptr; }

auto Module::symbol(SymbolName::View name) const
    -> Symbol
{ return impl_->symbol(name); }

} // namespace dlib
