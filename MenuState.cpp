#include "MenuState.h"
#include "PlayState.h"

MenuState::MenuState(GameStateManager& gsm, sf::RenderWindow& window)
    : gsm(gsm), windowRef(window), closeTimer(-1.0f)
{
    // Background image
    if (!bgTexture.loadFromFile("Sprites/Starting Menu.png"))
        bgTexture.create(1600, 900); // blank fallback

    bgSprite.setTexture(bgTexture, true);

    // Scale the background to fill the 1600x900 window exactly.
    float sx = 1600.0f / static_cast<float>(bgTexture.getSize().x);
    float sy =  900.0f / static_cast<float>(bgTexture.getSize().y);
    bgSprite.setScale(sx, sy);

    // Font — same fallback chain as the HUD
    font.loadFromFile("Sprites/hud.ttf")            ||
    font.loadFromFile("C:/Windows/Fonts/arial.ttf") ||
    font.loadFromFile("C:/Windows/Fonts/consola.ttf");

    // "THIS MODE DOES NOT EXIST YET" message (hidden until campaign is chosen)
    msgText.setFont(font);
    msgText.setString("THIS MODE DOES NOT EXIST YET");
    msgText.setCharacterSize(52);
    msgText.setFillColor(sf::Color(255, 50, 50));
    msgText.setOutlineColor(sf::Color::Black);
    msgText.setOutlineThickness(3.0f);
    msgText.setStyle(sf::Text::Bold);

    // Centre the message on screen
    sf::FloatRect bounds = msgText.getLocalBounds();
    msgText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    msgText.setPosition(800.0f, 450.0f);
}

void MenuState::handleInput(sf::Event& event, sf::RenderWindow& window)
{
    // Ignore input while the "not exist" message is showing.
    if (closeTimer > 0.0f) return;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Num1) {
            // Survival mode — push PlayState on top (menu goes to background).
            gsm.pushState(new PlayState());
        }
        else if (event.key.code == sf::Keyboard::Num2) {
            // Campaign mode — show message then close.
            closeTimer = 2.5f;
        }
    }
}

void MenuState::update(float dt)
{
    if (closeTimer > 0.0f) {
        closeTimer -= dt;
        if (closeTimer <= 0.0f)
            windowRef.close();
    }
}

void MenuState::render(sf::RenderWindow& window)
{
    window.draw(bgSprite);

    if (closeTimer > 0.0f)
        window.draw(msgText);
}
