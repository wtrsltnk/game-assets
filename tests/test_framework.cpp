#include <test_framework.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)
using namespace std;

test::TestFramework test::tests;

int main()
{
    return test::tests.Run();
}

test::Test::Test(const std::string& filename, pfnTestFunc func, const std::string& name)
    : _filename(filename), _func(func), _name(name)
{ }

test::Test::Test(const std::string& filename, pfnTestFunc func, const std::string& name, const std::string& description)
    : _filename(filename), _func(func), _name(name), _description(description)
{ }

test::Test* test::TestFramework::AddTest(const std::string& filename, pfnTestFunc func, const std::string& name, const std::string& description)
{
    std::string tmp = filename;
    std::replace( tmp.begin(), tmp.end(), '\\', '/');
    Test* result = new Test(tmp, func, name, description);

    test::tests._tests.push_back(result);

    return result;
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
        test::Result result = test->_func();
        clock_t elapsed = clock() - start;

        if (result.Passed() == false)
        {
            cout << "==== Failed";
            failed++;
        }
        else
            cout << "==== Passed";
        cout << " test \"" << test->_name << "\" in " << elapsed << " milliseconds." << endl;
        cout << "===========================================================================" << endl << endl;

        run++;
    }
    return 0;
}

test::Result test::Result::ReturnTrue(bool result)
{
    return test::Result();
}
