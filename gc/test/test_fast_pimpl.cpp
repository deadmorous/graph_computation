/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve.mail.ru>
 */

#include "common/fast_pimpl.hpp"

#include <string>
#include <vector>

#include "gtest/gtest.h"

namespace {

// =========================================================================
// SIMULATED HEADER CONTEXT
// In a real project, this section would be in 'Widget.h'
// =========================================================================

// 1. Forward Declaration (Implementation is unknown here)
struct WidgetImpl;

// 2. Public Class
class Widget {
public:
    Widget(int id, std::string name);
    ~Widget(); // Dtor must be declared here, defined where Impl is known

    int getId() const;
    std::string getName() const;
    void addTag(const std::string& tag);
    size_t getTagCount() const;

    // Expose internal state for testing purposes
    bool isInitialized() const;
    void manualReset();

private:
    // We estimate the size.
    // In a real scenario, you'd check sizeof(WidgetImpl) once and set this.
    static constexpr size_t kImplSize = 128;
    static constexpr size_t kImplAlign = 8;

    common::FastPimpl<WidgetImpl, kImplSize, kImplAlign> pimpl;
};

// =========================================================================
// SIMULATED SOURCE CONTEXT
// In a real project, this section would be in 'Widget.cpp'
// =========================================================================

// 3. Full Implementation Definition
struct WidgetImpl {
    int id;
    std::string name;
    std::vector<std::string> tags;

    // Static counter to verify destructor calls
    static int destructor_call_count;

    WidgetImpl(int i, std::string n) : id(i), name(std::move(n)) {}

    ~WidgetImpl() {
        destructor_call_count++;
    }
};

// Initialize static counter
int WidgetImpl::destructor_call_count = 0;

// 4. Class Method Definitions (Where static_asserts happen)

Widget::Widget(int id, std::string name) {
    // This triggers the static_asserts inside FastPimpl::emplace
    pimpl.emplace(id, std::move(name));
}

Widget::~Widget() = default; // Invokes FastPimpl::~FastPimpl -> WidgetImpl::~WidgetImpl

int Widget::getId() const { return pimpl->id; }
std::string Widget::getName() const { return pimpl->name; }

void Widget::addTag(const std::string& tag) {
    pimpl->tags.push_back(tag);
}

size_t Widget::getTagCount() const {
    return pimpl->tags.size();
}

bool Widget::isInitialized() const { return pimpl.has_value(); }

void Widget::manualReset() { pimpl.destroy(); }

// =========================================================================
// TESTS
// =========================================================================

} // anonymous namespace


class FastPimplTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset counter before each test
        WidgetImpl::destructor_call_count = 0;
    }
};

TEST_F(FastPimplTest, ConstructionAndDestruction) {
    {
        Widget w(1, "TestWidget");
        EXPECT_TRUE(w.isInitialized());
        EXPECT_EQ(w.getId(), 1);
        EXPECT_EQ(w.getName(), "TestWidget");
    }
    // w goes out of scope here

    EXPECT_EQ(WidgetImpl::destructor_call_count, 1)
        << "Destructor should have been called exactly once.";
}

TEST_F(FastPimplTest, DataMutation) {
    Widget w(2, "Mutator");
    w.addTag("alpha");
    w.addTag("beta");

    EXPECT_EQ(w.getTagCount(), 2);
}

TEST_F(FastPimplTest, ConstCorrectness) {
    const Widget w(3, "ConstWidget");

    // accessors marked const should work
    EXPECT_EQ(w.getName(), "ConstWidget");

    // w.addTag("fail"); // This would fail to compile, as expected
}

TEST_F(FastPimplTest, ManualDestroy) {
    Widget w(4, "Manual");
    EXPECT_TRUE(w.isInitialized());

    w.manualReset();

    EXPECT_FALSE(w.isInitialized());
    EXPECT_EQ(WidgetImpl::destructor_call_count, 1);

    // Destructor of Widget runs here, but FastPimpl checks "initialized_"
    // and should not double-free.
}

TEST_F(FastPimplTest, ArrowOperator) {
    // Testing the pointer-like semantics internally
    // We create a direct instance of FastPimpl for granular testing
    // (Simulating internal usage)

    struct TinyImpl { int x; };
    common::FastPimpl<TinyImpl, sizeof(TinyImpl)> wrapper;

    wrapper.emplace(TinyImpl{42});

    EXPECT_EQ(wrapper->x, 42);
    EXPECT_EQ((*wrapper).x, 42);
}

/*
// COMPILE-TIME CHECK DEMONSTRATION
// Uncommenting this test should cause a compilation failure.
TEST(FastPimplTest, FailsCompilationIfTooSmall) {
    struct BigStruct { char data[100]; };
    // Trying to fit 100 bytes into 10 bytes:
    common::FastPimpl<BigStruct, 10> willFail;
    willFail.emplace(); // static_assert triggers here
}
//*/
