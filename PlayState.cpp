//#include "PlayState.h"
//
//PlayState::PlayState() {
//    std::cout << "PlayState Initialized.\n";
//
//    //initializing level constraints
//    cell_size = 64;
//    height = 14;
//    width = 110;
//
//    //manually allocate the 2D array with raw pointers. 
//    lvl = new char* [height];
//    for (int i = 0; i < height; i++) {
//        lvl[i] = new char[width] {'\0'};
//    }
//
//    lvl[11][5] = 'g';
//    lvl[11][6] = 'g';
//    lvl[11][7] = 'g';
//    lvl[11][8] = 'g';
//    lvl[11][9] = 'g';
//    lvl[11][10] = 'g';
//    lvl[11][11] = 'g';
//    lvl[11][12] = 'g';
//    lvl[11][13] = 'g';
//    lvl[11][14] = 'g';
//
//    if (!wallTex1.loadFromFile("Sprites/blocks/grass_block_side.png")) {
//        std::cout << "ERROR: Failed to load grass texture!\n";
//    }
//    wallSprite1.setTexture(wallTex1);
//
//    //initialize player stats
//    player_x = 380.0f;
//    player_y = 610.0f;
//    max_speed = 300.0f;
//    gravity = 1500.0f;
//    velocityX = 0.0f;
//    velocityY = 0.0f;
//    acceleration = 0.5f;
//
//    float scale_x = 0.2f;
//    float scale_y = 0.2f;
//    //int raw_img_y = 470;
//    //int Pheight = raw_img_y * scale_y;
//
//    isOnGround = true;
//    initialJumpSpeed = -700.0f; // The instant upward burst
//
//    if (!playerTex.loadFromFile("Sprites/Character.png")) {
//        std::cout << "ERROR: Failed to load player texture!\n";
//    }
//    playerSprite.setTexture(playerTex);
//    playerSprite.setScale(scale_x, scale_y);
//
//    playerHeight = playerTex.getSize().y * scale_y;
//    playerWidth = playerTex.getSize().x * scale_x;
//
//    playerSprite.setPosition(player_x, player_y);
//}
////destroying pointers
//PlayState::~PlayState() {
//    for (int i = 0; i < height; i++) {
//        delete[] lvl[i];
//    }
//    delete[] lvl;
//}
//
//void PlayState::handleInput(sf::Event& event, sf::RenderWindow& window) {
//    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
//        window.close();
//    }
//}
//
//void PlayState::update(float dt) {
//    // Horizontal Movement
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
//        velocityX = max_speed;
//    }
//    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
//        velocityX = -max_speed;
//    }
//    else {
//        velocityX = 0.0f;
//    }
//
//    // Apply horizontal speed multiplied by delta time
//    player_x += velocityX * dt;
//
//    // 2. Vertical Movement & Physics
//    // Gravity is ALWAYS pulling you down, every single frame.
//    velocityY += gravity * dt;
//
//    // Apply vertical speed multiplied by delta time
//    player_y += velocityY * dt;
//
//    // 3. GENERIC TILE COLLISION (No getGlobalBounds allowed)
//    // We manually calculate the feet and sides using our tracked variables
//    // 1. Guilty until proven grounded. ALWAYS reset this at the top.
//    //isOnGround = false;
//
//    // 2. Add an epsilon (+1.0f) to look exactly 1 pixel under the feet to dodge floating point errors.
//    // Inset the X coordinates by 5 pixels so you actually fall when walking off ledges.
//    float feetY = player_y + playerHeight + 1.0f;
//    float leftFootX = player_x + 2.0f;
//    float rightFootX = player_x + playerWidth - 2.0f;
//
//    // Translate pixel coordinates into 2D Array Indices safely
//    int rowBlock = static_cast<int>(feetY / cell_size);
//    int leftFootBlock = static_cast<int>(leftFootX / cell_size);
//    int rightFootBlock = static_cast<int>(rightFootX / cell_size);
//
//    // SAFETY FIRST: Array bounds checking to prevent segfaults
//    if (rowBlock >= 0 && rowBlock < height &&
//        leftFootBlock >= 0 && leftFootBlock < width &&
//        rightFootBlock >= 0 && rightFootBlock < width)
//    {
//        // Check the block under the left foot OR the right foot
//        if (lvl[rowBlock][leftFootBlock] == 'g' || lvl[rowBlock][rightFootBlock] == 'g') {
//
//            // Only snap if falling downward. Gravity is always pulling > 0 when falling.
//            if (velocityY > 0.0f) {
//                // Snap the player so their feet rest exactly on the top edge
//                player_y = (rowBlock * cell_size) - playerHeight;
//                velocityY = 0.0f; // Kill the downward momentum dead
//                isOnGround = true; // Safe!
//            }
//        }
//        else {
//            isOnGround = false;
//        }
//    }
//
//    // 4. The Jump Trigger
//    // You can ONLY jump if you are currently touching the ground
//    if (isOnGround && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
//        velocityY = initialJumpSpeed; // Apply instant negative (upward) speed
//        isOnGround = false;
//    }
//
//    // Update the visual sprite with the new math
//    playerSprite.setPosition(player_x, player_y);
//}
//
//void PlayState::render(sf::RenderWindow& window) {
//    // Note: window.clear() and window.display() are called by the Game loop, NOT here.
//
//    //render the level map first
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {
//            if (lvl[i][j] == 'g') {
//                wallSprite1.setPosition(j * cell_size, i * cell_size);
//                window.draw(wallSprite1);
//            }
//        }
//    }
//
//    //render the player on top
//    window.draw(playerSprite);
//}
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
    // EntityManager destructor deletes all entities including player.
    // Delete manager before level so entities can't reference a freed level
    // during their own destructors (not an issue currently but good habit).
    delete entities;
    entities = nullptr;
    player = nullptr; // dangling pointer guard - manager already freed it

    delete level;
    level = nullptr;
}

void PlayState::handleInput(sf::Event& event, sf::RenderWindow& window)
{
    // Per academic constraints: no event.key.code.
    // Escape is handled via event.type only, checking scancode instead.
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scan::Escape) {
            window.close();
        }
    }
    // Continuous player input (movement, jump) is polled inside
    // Player::handleInput() which runs every update() tick via isKeyPressed.
    // No need to route it through the event queue here.
}

void PlayState::update(float dt)
{
    // Cap dt so a lag spike doesn't teleport entities through walls.
    // At 30fps minimum, dt never exceeds ~33ms. Physics stays stable.
    if (dt > 1.0f / 30.0f) dt = 1.0f / 30.0f;

    entities->updateAll(dt);
    entities->removeDead(); // sweep dead entities at END of update, never mid-loop

    // After removeDead(), check if player got cleaned up (died, fell out of world, etc.)
    // If player is gone, don't crash trying to follow it with the camera.
    if (player != nullptr && !player->getIsActive()) {
        player = nullptr; // manager already freed it, just null our reference
    }

    // Camera follows the player (dead-zone style, same logic as before)
    if (player != nullptr) {
        float playerScreenX = player->getPosX() - cameraX; // This tells the player's position on the screen rn
        //float leftThreshold = 1600.0f / 3.0f;
        float rightThreshold = 1600.0f - (1600.0f / 3); //2.0f * 1600.0f / 3.5f; // Background only moves when player enters the 3rd 1/3 of the screen.

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