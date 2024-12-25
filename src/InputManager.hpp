#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <unordered_map>

namespace eeng {

class InputManager {
public:
    enum class Key {
        // Letters
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Numbers
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        // Modifiers
        LeftShift, RightShift,
        LeftCtrl, RightCtrl,
        LeftAlt, RightAlt,

        // Navigation Keys
        Up, Down, Left, Right,

        // Space and Other Special Keys
        Space,
        Enter,
        Backspace,
        Tab,
        Escape,

        // Function Keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // Add any other keys you need...
    };

    struct MouseState {
        int x = 0, y = 0;
        bool leftButton = false;
        bool rightButton = false;
    };

    struct ControllerState {
        float axisLeftX = 0.0f, axisLeftY = 0.0f;
        float axisRightX = 0.0f, axisRightY = 0.0f;
        std::unordered_map<int, bool> buttonStates;
    };

    InputManager();
    ~InputManager();

    // Input handling
    void HandleEvent(const void* event); // Abstract to avoid SDL dependency
    void Update();

    // Query functions
    bool IsKeyPressed(Key key) const;
    bool IsMouseButtonDown(int button) const;
    const MouseState& GetMouseState() const;
    const ControllerState& GetControllerState(int controllerIndex) const;

private:
    struct Impl;                   // Forward declaration for the PImpl idiom
    Impl* pImpl;                   // Pointer to implementation
};

} // namespace eeng

using InputManagerPtr = std::shared_ptr<const eeng::InputManager>;

#endif // INPUTMANAGER_H
