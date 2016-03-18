#include <test_framework.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)
using namespace std;

test::TestFramework test::tests;

int main()
{
    AddTests();
    return test::tests.Run();
}

test::Test::Test(const std::string& filename, pfnTestFunc func, const std::string& name)
    : _filename(filename), _func(func), _name(name)
{
    test::tests.AddTest(this);
}

test::Test::Test(const std::string& filename, pfnTestFunc func, const std::string& name, const std::string& description)
    : _filename(filename), _func(func), _name(name), _description(description)
{
    test::tests.AddTest(this);
}

test::Test* test::TestFramework::AddTest(const std::string& filename, pfnTestFunc func, const std::string& name, const std::string& description)
{
    std::string tmp = filename;
    std::replace( tmp.begin(), tmp.end(), '\\', '/');

    return new Test(tmp, func, name, description);
}

test::Test* test::TestFramework::AddTest(test::Test* test)
{
    test::tests._tests.push_back(test);

    return test;
}

int test::TestFramework::Run()
{
    int run = 0;
    int failed = 0;

    cout << "Start running all " << test::tests._tests.size() << " tests." << endl << endl;

    for (std::vector<Test*>::iterator itr = test::tests._tests.begin(); itr != test::tests._tests.end(); ++itr)
    {
        Test* test = *itr;

        cout << "===========================================================================" << endl;
        cout << "==== Starting test \"" << test->_name << "\"" << endl;
        clock_t start = clock();
        try
        {
            test->_func();
            clock_t elapsed = clock() - start;

            cout << "==== Passed test \"" << test->_name << "\" in " << elapsed << " milliseconds." << endl;
        }
        catch (const std::exception& ex)
        {
            clock_t elapsed = clock() - start;

            cout << "==== Failed test \"" << test->_name << "\" in " << elapsed << " milliseconds." << endl;
            cout << "     Failed on : " << ex.what() << endl;

            failed++;
        }

        cout << "===========================================================================" << endl << endl;

        run++;
    }
    return 0;
}

test::Result test::Result::ReturnTrue(bool result)
{
    test::Result r;
    r._passed = result;
    return r;
}

void test::Assert::IsTrue(bool expression, const std::string& expression_name)
{
    if (expression == false) throw std::runtime_error(expression_name);
}

void test::Assert::IsFalse(bool expression, const std::string& expression_name)
{
    if (expression == true) throw std::runtime_error(expression_name);
}
