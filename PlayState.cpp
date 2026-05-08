#include "PlayState.h"

PlayState::PlayState()
    : level(nullptr), entities(nullptr), player(nullptr),
    cameraX(0.0f), cameraY(0.0f)
{
    std::cout << "PlayState Initialized.\n";

    // Creating world first, Player constructor needs a valid Level* to store, so
    // build the world. (Width = 200 tiles, Height = 14, cell = 64px)
    level = new Level(200, 14, 64);
    level->buildTestMap();

    entities = new EntityManager(32);

    // Spawn the player. EntityManager takes ownership from this point.
    // We keep a raw non-owning pointer for camera tracking.
    player = new Player(200.0f, 200.0f, level);
    entities->add(player); // manager owns it now

    if (!bgTex.loadFromFile("Sprites/Background.png")) {
        std::cout << "ERROR: background texture failed\n";
    }
    bgSprite.setTexture(bgTex);

    float scaleY = 900.0f / bgTex.getSize().y;
    bgSprite.setScale(scaleY, scaleY);
}

PlayState::~PlayState()
{
    delete entities;
    entities = nullptr;
    player = nullptr; // dangling pointer guard (Entity manager will delete player)

    delete level;
    level = nullptr;
}

void PlayState::handleInput(sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scan::Escape) {
            window.close();
        }
    }
}

void PlayState::update(float dt)
{
    // Clamping dt at a max value of 0.0333 so that if fps drop enourmously, dt doesn't get too much and the 
    // player doesn't start to teleport
    if (dt > 1.0f / 30.0f) 
        dt = 1.0f / 30.0f;

    entities->updateAll(dt);
    entities->removeDead();

    // After removeDead(), check if player got cleaned up (died, fell out of world, etc.)
    // If player is gone, don't crash trying to follow it with the camera.
    if (player != nullptr && !player->getIsActive()) {
        player = nullptr; // means manager already freed it, we make it point nothing
    }

    // Clamping the player to the left boundary of the screen and right boundary of the world
    if (player != nullptr) {
        if (player->getPosX() < cameraX) {
            player->setPosition(cameraX, player->getPosY());
            player->setVelocity(0.0f, player->getVelocityY());
        }
        if (player->getPosX() + player->getEntityWidth() > cameraX + 1600.0f) {
            player->setPosition(cameraX + 1600.0f - player->getEntityWidth(), player->getPosY());
            player->setVelocity(0.0f, player->getVelocityY());
        }
    }

    if (player != nullptr) {
        float playerScreenX = player->getPosX() - cameraX; // This tells the player's position on the screen rn
        float rightThreshold = 1600.0f - (1600.0f / 3); // Background only moves when player enters the 3rd 1/3 of the screen.

        if (playerScreenX > rightThreshold) {
        cameraX = player->getPosX() - rightThreshold;
        }
     // else: player is in the first and middle third, camera does not move

        // Clamping camera values
        if (cameraX < 0.0f) cameraX = 0.0f;
        float maxCamX = (float)(level->getWidthInPixels() - 1600);
        if (cameraX > maxCamX) cameraX = maxCamX;
    }
}

void PlayState::render(sf::RenderWindow& window)
{
    float bgWidth = (float)bgTex.getSize().x * bgSprite.getScale().x;
    float bgScrollX = cameraX * 0.2f; // How far the background has scrolled (speed is 20% of player's speed)
    // This gives us that how much pixels (starting from left of a panel) have passed and how much I have to display for thr first panel
    // This logic basically loops the image in the background, four tiles of bg image run in a loop in background, instead of loading all 
    // background pixles with background every frame (to avoid lagging)

    float startOffset = bgScrollX - (int)(bgScrollX / bgWidth) * bgWidth;
    
    // Finding that how many tiles of bg image do we need to run in a loop so that it seems to be never-ending
    int numTiles = (int)(1600.0f / bgWidth) + 2;

    for (int i = 0; i < numTiles; i++) {
        bgSprite.setPosition(i * bgWidth - startOffset, 0.0f);
        window.draw(bgSprite);
    }

    level->render(window, cameraX, cameraY);
    entities->renderAll(window, cameraX, cameraY);
}