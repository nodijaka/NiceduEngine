#include "Engine.hpp"
#include "Game.hpp"
#include <memory>

int main(int argc, char* argv[])
{
    std::cout << "Starting eduEngine..." << std::endl;

    eeng::Engine engine;

    if (!engine.init("eduEngine", 1600, 900))
    {
        std::cerr << "Engine failed to initialize." << std::endl;
        return -1;
    }

    auto game = std::make_unique<Game>();
    engine.run(std::move(game));

    std::cout << "Exiting eduEngine." << std::endl;
    return 0;
}