#include "calculator.h"

#include <cmath>
#include <exception>
#include <iostream>
#include <string>

using namespace std;

namespace
{

    // Accumulates the number of failed expectations.
    int failures = 0;

    // Expect that evaluating expr returns a value close to expected.
    // Uses a small epsilon because results are doubles.
    void expect_near(const string &expr, double expected, double eps = 1e-12)
    {
        try
        {
            double actual = calculator::evaluate(expr);
            if (fabs(actual - expected) > eps)
            {
                cerr << "FAIL: " << expr << " => expected " << expected << ", got " << actual << "\n";
                ++failures;
            }
        }
        catch (const exception &ex)
        {
            cerr << "FAIL: " << expr << " => unexpected exception: " << ex.what() << "\n";
            ++failures;
        }
    }

    // Expect that evaluating expr throws (any) exception.
    void expect_throws(const string &expr)
    {
        try
        {
            (void)calculator::evaluate(expr);
            cerr << "FAIL: " << expr << " => expected exception, got value\n";
            ++failures;
        }
        catch (const exception &)
        {
            // ok
        }
    }

} // namespace

// Minimal test runner (no external framework): returns 0 on success, 1 on failure.
int main()
{
    // Basic arithmetic + precedence
    expect_near("2+5*2", 12);
    expect_near("(2+5)*2", 14);
    expect_near("9 / 2", 4.5);
    expect_near("9/2", 4.5);

    // Whitespace + decimals
    expect_near("  .5 + .5 ", 1.0);
    expect_near("1.25 * 8", 10.0);

    // Unary minus
    expect_near("-3", -3);
    expect_near("-(2+1)*4", -12);
    expect_near("-3+10/2", 2);
    expect_near("2*-3", -6);

    // Error cases
    expect_throws("1/0");
    expect_throws("(1+2");
    expect_throws("1+2)");
    expect_throws("1++2");
    expect_throws("abc");

    if (failures == 0)
    {
        cout << "All tests passed.\n";
        return 0;
    }

    cerr << failures << " test(s) failed.\n";
    return 1;
}
