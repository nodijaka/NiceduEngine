#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Audio.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

int main() {
    //sf::RenderWindow window(sf::VideoMode(800, 600), "SFML + ImGui");
    auto window = sf::RenderWindow {sf::VideoMode { 1920u, 1080u }, "SFML Project"};
    window.setFramerateLimit(144);

    // Setup ImGui
    ImGui::SFML::Init(window);

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("/Users/ag1498/GitHub/eduEngine/assets/sound/Juhani Junkala [Retro Game Music Pack] Title Screen.wav")) {
        // Error loading the sound file
    }

    // Sound test
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
    //while (sound.getStatus() == sf::Sound::Playing) {} // Wait for sound to finishes

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
