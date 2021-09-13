#include <cassert>
#include "ButtonOverlay.hpp"

const int MAX_BUTTONS_COUNT = 100;

const sf::Color SELECTED_COLOR = sf::Color(240, 197, 105);
const sf::Color PRESSED_COLOR  = sf::Color(255, 174, 0);

ButtonOverlay::ButtonOverlay(sf::RenderWindow &window, sf::Font &font) :
                  window(window),
                  buttonsCount(0)
{
    buttons = new Button[MAX_BUTTONS_COUNT];

    textRenderer.setFont(font);
    textRenderer.setFillColor(sf::Color::Black);
    textRenderer.setCharacterSize(14);
    textRenderer.setStyle(sf::Text::Bold);
}

ButtonOverlay::~ButtonOverlay()
{
    delete[] buttons;
}

ButtonHandle ButtonOverlay::addButton(sf::FloatRect rect, const char *text, sf::Color color)
{
    assert(text != nullptr);

    if (buttonsCount >= MAX_BUTTONS_COUNT)
        return -1;

    ButtonHandle handle = buttonsCount++;
    buttons[handle] = { rect, text, color, ButtonState::Idle };
    return handle;
}

bool ButtonOverlay::isButtonPressed(ButtonHandle handle)
{
    assert(handle >= 0 && handle < buttonsCount);

    return buttons[handle].state == ButtonState::Pressed;
}

bool ButtonOverlay::isButtonClicked(ButtonHandle handle)
{
    assert(handle >= 0 && handle < buttonsCount);

    return buttons[handle].state == ButtonState::Clicked;
}

bool ButtonOverlay::isButtonReleased(ButtonHandle handle)
{
    assert(handle >= 0 && handle < buttonsCount);

    return buttons[handle].state == ButtonState::Released;
}

void ButtonOverlay::update()
{
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    
    for (int handle = 0; handle < buttonsCount; handle++)
    {
        Button &button = buttons[handle];

        bool selected = button.rect.contains(mousePosition);
        
        if (selected && mousePressed)
        {
            // Button is pressed
            if (button.state == ButtonState::Selected)
                button.state = ButtonState::Clicked;
            else
                button.state = ButtonState::Pressed;
        }
        else if (selected)
        {
            // Button is selected

            if (button.state == ButtonState::Pressed)
                button.state = ButtonState::Released;
            else
                button.state = ButtonState::Selected;
        }
        else
        {
            if (button.state == ButtonState::Pressed)
                button.state = ButtonState::Released;
            else
                button.state = ButtonState::Idle;
        }
    }
}

void ButtonOverlay::draw()
{
    for (int handle = 0; handle < buttonsCount; handle++)
    {
        const Button &button = buttons[handle];

        rectRenderer.setPosition(sf::Vector2f(button.rect.left , button.rect.top   ));
        rectRenderer.setSize    (sf::Vector2f(button.rect.width, button.rect.height));

        switch (button.state)
        {
        case ButtonState::Clicked:
        case ButtonState::Pressed:
            rectRenderer.setFillColor(PRESSED_COLOR);
            break;

        case ButtonState::Released:
        case ButtonState::Selected:
            rectRenderer.setFillColor(SELECTED_COLOR);
            break;

        default:
            rectRenderer.setFillColor(button.color);
            break;
        }

        window.draw(rectRenderer);

        textRenderer.setString(button.text);
        sf::FloatRect textBounds = textRenderer.getLocalBounds();
        textRenderer.setPosition(sf::Vector2f(button.rect.left + (button.rect.width  - textBounds.width)  / 2,
                                              button.rect.top  + (button.rect.height - textBounds.height) / 2));

        window.draw(textRenderer);
    }
}