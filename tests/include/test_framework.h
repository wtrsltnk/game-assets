#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <string>
#include <vector>

namespace test
{
class Result
{
    bool _passed;
public:
    bool Passed() { return this->_passed; }

public:
    static Result ReturnTrue(bool result);

};

typedef test::Result (*pfnTestFunc)();

class Test;
class TestFramework
{
    std::vector<Test*> _tests;
public:
    Test* AddTest(const std::string& filename, pfnTestFunc func, const std::string& name, const std::string& description = "");
    int Run();

    static TestFramework& singleton()
    {
        static TestFramework instance;
        return instance;
    }
};

extern TestFramework tests;

class Test
{
    friend class TestFramework;
    std::string _filename;
    pfnTestFunc _func;
    std::string _name;
    std::string _description;
public:
    Test(const std::string& filename, pfnTestFunc func, const std::string& name);
    Test(const std::string& filename, pfnTestFunc func, const std::string& name, const std::string& description);
};

}

#define ADD_TEST(x)  test::Result __TestFunc_##x(); \
    test::Test* t_##x = test::tests.AddTest(__FILE__, &__TestFunc_##x, #x); \
    test::Result __TestFunc_##x()

#define ADD_TEST_WITH_DESCRIPTION(x, y)  test::Result __TestFunc_##x(); \
    test::Test* t_##x = test::tests.AddTest(__FILE__, &__TestFunc_##x, #x, y); \
    test::Result __TestFunc_##x()

#endif // TEST_FRAMEWORK_H

