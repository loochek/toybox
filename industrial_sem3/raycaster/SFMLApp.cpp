#include <stdexcept>
#include "SFMLApp.hpp"
#include "Vec3.hpp"

const int WINDOW_WIDTH  = 1920;
const int WINDOW_HEIGHT = 1080;

const float MOVEMENT_SPEED = 1.0f;

const Material MATERIALS_CYCLE[] = { MAT_COPPER, MAT_OBSIDIAN, MAT_CHROME, MAT_GOLD };
const int MATERIALS_CYCLE_COUNT  = 4;


SFMLApp::SFMLApp() : lampAngle(0), buttonOverlay(window, font), raycaster(window)
{
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "raycaster", sf::Style::Fullscreen);
    window.setVerticalSyncEnabled(true);

    if (!font.loadFromFile("Roboto-Light.ttf"))
        throw std::runtime_error("Unable to load font");

    raycaster.setCanvasSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));

    // Buttons setup
    // Camera control

    forwardButton = buttonOverlay.addButton(sf::FloatRect(100, 100, 60, 60), "Forward");
    backButton    = buttonOverlay.addButton(sf::FloatRect(100, 260, 60, 60), "Back");
    leftButton    = buttonOverlay.addButton(sf::FloatRect(20 , 180, 60, 60), "Left");
    rightButton   = buttonOverlay.addButton(sf::FloatRect(180, 180, 60, 60), "Right");

    upButton      = buttonOverlay.addButton(sf::FloatRect(260, 100, 60, 60), "Up");
    downButton    = buttonOverlay.addButton(sf::FloatRect(260, 260, 60, 60), "Down");

    // Material pickers

    spheresMatPickers[0] = buttonOverlay.addButton(sf::FloatRect(20, 340, 180, 60), "Sphere 1 material");
    spheresMatPickers[1] = buttonOverlay.addButton(sf::FloatRect(20, 420, 180, 60), "Sphere 2 material");

    // Raycaster setup
    // Sphere 1

    spheresMaterial[0] = 0;
    spheres[0].material = MATERIALS_CYCLE[spheresMaterial[0]];
    spheres[0].position = Vec3f(-2.0f, 0.0f, 0.0f);

    // Sphere 2

    spheresMaterial[1] = 1;
    spheres[1].material = MATERIALS_CYCLE[spheresMaterial[1]];
    spheres[1].position = Vec3f(2.0f, 0.0f, 0.0f);

    // Static lamp

    spheres[2].material = MAT_LAMP;
    spheres[2].position = Vec3f(0.0f, -2.5f, -1.0f);
    spheres[2].radius = 0.2f;

    lights[0].position  = spheres[2].position;

    // Dynamic lamp

    spheres[3].material = MAT_LAMP;
    spheres[3].radius = 0.2f;
}

void SFMLApp::run()
{
    sf::Clock clock;

    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Handle input

        buttonOverlay.update();

        float elapsedTime = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || buttonOverlay.isButtonPressed(forwardButton))
            raycaster.moveCamera(Vec3f(0.0f, MOVEMENT_SPEED * elapsedTime, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || buttonOverlay.isButtonPressed(backButton))
            raycaster.moveCamera(Vec3f(0.0f, -MOVEMENT_SPEED * elapsedTime, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || buttonOverlay.isButtonPressed(rightButton))
            raycaster.moveCamera(Vec3f(MOVEMENT_SPEED * elapsedTime, 0.0f, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || buttonOverlay.isButtonPressed(leftButton))
            raycaster.moveCamera(Vec3f(-MOVEMENT_SPEED * elapsedTime, 0.0f, 0.0f));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || buttonOverlay.isButtonPressed(upButton))
            raycaster.moveCamera(Vec3f(0.0f, 0.0f, MOVEMENT_SPEED * elapsedTime));

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || buttonOverlay.isButtonPressed(downButton))
            raycaster.moveCamera(Vec3f(0.0f, 0.0f, -MOVEMENT_SPEED * elapsedTime));

        for (int i = 0; i < 2; i++)
        {
            if (buttonOverlay.isButtonClicked(spheresMatPickers[i]))
            {
                spheresMaterial[i]++;
                spheresMaterial[i] %= MATERIALS_CYCLE_COUNT;
                spheres[i].material = MATERIALS_CYCLE[spheresMaterial[i]];
            }
        }
        
        // Update

        lampAngle += elapsedTime;

        spheres[3].position = Vec3f(4.0f * sin(lampAngle), 4.0f * cos(lampAngle), 2.0f);
        lights[1].position  = spheres[3].position;

        for (int i = 0; i < SPHERES_COUNT; i++)
            raycaster.sphereRendercall(&spheres[i]);
        
        for (int i = 0; i < LIGHTS_COUNT; i++)
            raycaster.pointLightRendercall(&lights[i]);

        // Draw
        
        window.clear(sf::Color::Green);
        raycaster.draw();
        buttonOverlay.draw();
        window.display();
    }
}