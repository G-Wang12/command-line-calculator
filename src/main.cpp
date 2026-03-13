#include "calculator.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

namespace
{
    // Remove leading/trailing whitespace from a line (used by the REPL).
    string trim(string s)
    {
        auto not_space = [](unsigned char ch)
        { return isspace(ch) == 0; };

        auto begin_it = find_if(s.begin(), s.end(), not_space);
        if (begin_it == s.end())
            return {};

        auto rbegin_it = find_if(s.rbegin(), s.rend(), not_space);
        return string(begin_it, rbegin_it.base());
    }

    // Print CLI usage help.
    void print_usage(const char *argv0)
    {
        const char *prog = (argv0 != nullptr && argv0[0] != '\0') ? argv0 : "toy-app";
        cerr << "Usage:\n"
             << "  " << prog << " \"(2+5)*2\"\n"
             << "  " << prog << " --interactive\n"
             << "  " << prog << " --help\n";
    }

    // Evaluate a single expression and print the result (or an error message).
    int run_once(string_view expr)
    {
        try
        {
            double result = calculator::evaluate(expr);
            cout << calculator::format_number(result) << "\n";
            return 0;
        }
        catch (const exception &ex)
        {
            cerr << "Error: " << ex.what() << "\n";
            return 2;
        }
    }

    // Interactive calculator loop.
    // Reads one expression per line until EOF or user enters "quit"/"exit".
    int run_repl()
    {
        cout << "Live calculator. Type an expression and press Enter.\n"
             << "Type 'exit' or 'quit' to stop.\n";

        string line;
        while (true)
        {
            cout << "> " << flush;
            if (!getline(cin, line))
            {
                cout << "\n";
                break;
            }

            string expr = trim(line);
            if (expr.empty())
            {
                continue;
            }
            if (expr == "exit" || expr == "quit")
            {
                break;
            }

            (void)run_once(expr);
        }

        return 0;
    }

} // namespace

// Entry point:
// - No args: start REPL
// - One arg: evaluate expression, or handle --help/--interactive
int main(int argc, char **argv)
{
    if (argc == 1)
    {
        return run_repl();
    }

    if (argc == 2)
    {
        string_view arg(argv[1]);
        if (arg == "--help" || arg == "-h")
        {
            print_usage(argv[0]);
            return 0;
        }
        if (arg == "--interactive" || arg == "-i")
        {
            return run_repl();
        }
        return run_once(arg);
    }

    print_usage(argv[0]);
    return 1;
}
