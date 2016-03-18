#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <string>
#include <vector>

extern void AddTests();

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

class Assert
{
public:
    static void IsTrue(bool expression, const std::string& expression_name = "IsTrue = false");
    static void IsFalse(bool expression, const std::string& expression_name = "IsFalse == false");

};

typedef void (*pfnTestFunc)();

class Test;
class TestFramework
{
    std::vector<Test*> _tests;
public:
    Test* AddTest(const std::string& filename, pfnTestFunc func, const std::string& name, const std::string& description = "");
    Test* AddTest(Test* test);
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

#define ADD_TEST(x, y) test::Test* t_##x = test::tests.AddTest(__FILE__, &__TestFunc_##x, #x);

#define TEST(x) void __TestFunc_##x()

#endif // TEST_FRAMEWORK_H

