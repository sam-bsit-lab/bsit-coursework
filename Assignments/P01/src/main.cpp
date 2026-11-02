#include "Tests.hpp"
#include "ConsoleView.hpp"
#include "TuiView.hpp"

#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--test") return runTests();
    if (argc > 2 || (argc == 2 && std::string(argv[1]) != "--text"))
    {
        std::cerr << "Usage: game [--text | --test]\n";
        return 1;
    }

    VideoPoker game;
    std::unique_ptr<GameView> view;
    if (argc == 2)
        view = std::make_unique<ConsoleView>(game);
    else
        view = std::make_unique<TuiView>(game);
    view->run();
}
