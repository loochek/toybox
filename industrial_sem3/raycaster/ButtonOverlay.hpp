#ifndef BUTTON_OVERLAY_HPP
#define BUTTON_OVERLAY_HPP

#include <SFML/Graphics.hpp>

typedef int ButtonHandle;

/**
 * Provides an overlay with clickable buttons for a render window
 */
class ButtonOverlay
{
public:
    /**
     * \param window Render window for drawing and mouse tracking
     * \param font Font to be used for text rendering 
     */
    ButtonOverlay(sf::RenderWindow &window, sf::Font &font);
    ~ButtonOverlay();

    /**
     * Adds a button to the overlay
     * 
     * \param rect  Button position and size
     * \param text  Button text
     * \param color Button color 
     * 
     * \return Button handle or -1 if failed
     */
    ButtonHandle addButton(sf::FloatRect rect,
                           const char   *text  = "",
                           sf::Color     color = sf::Color::Cyan);

    /**
     * Indicates if the button is pressed
     * 
     * \param handle Button handle
     */
    bool isButtonPressed(ButtonHandle handle);

    /**
     * Indicates if the button was clicked
     * 
     * \param handle Button handle
     */
    bool isButtonClicked(ButtonHandle handle);

    /**
     * Indicates if the button was released
     * 
     * \param handle Button handle
     */
    bool isButtonReleased(ButtonHandle handle);

    /**
     * Updates buttons state according to the mouse state
     */
    void update();

    /**
     * Draws the overlay in associated render window
     */
    void draw();

private:
    sf::RenderWindow &window;

    sf::RectangleShape rectRenderer;
    sf::Text           textRenderer;
    
    enum class ButtonState
    {
        Idle,
        Selected,
        Clicked,
        Pressed,
        Released
    };

    struct Button
    {
        sf::FloatRect rect;
        const char   *text;
        sf::Color     color;
        ButtonState   state;
    };
    
    Button *buttons;
    int     buttonsCount;
};

#endif