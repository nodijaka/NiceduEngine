#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML + ImGui");

    // Setup ImGui binding
    ImGui::SFML::Init(window);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Start the ImGui frame
        ImGui::SFML::Update(window, sf::seconds(1.f / 60.f));

        // ImGui demo window
        ImGui::ShowDemoWindow();

        // Rendering
        window.clear();
        // Your SFML drawing goes here
        ImGui::SFML::Render(window);
        window.display();
    }

    // Cleanup
    ImGui::SFML::Shutdown();

    return 0;
}
