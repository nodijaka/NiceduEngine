/*#include <iostream>

int main(int, char**){
    std::cout << "Hello, from project!\n";
    
}
*/

#include <SFML/Graphics.hpp>

int main() {
    
    // Create a window with dimensions 800x600
    //sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
    sf::Window window(sf::VideoMode(sf::Vector2u(800, 600)), "SFML Window");

    // Main loop that runs until the window is closed
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window with a black color
        //window.clear(sf::Color::Black);

        // Display the contents of the window
        window.display();
    }

    return 0;
}


