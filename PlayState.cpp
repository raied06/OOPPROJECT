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

PlayState::PlayState() {
    std::cout << "PlayState Initialized.\n";

    // Build the world. Width = 200 tiles, Height = 30, cell = 64px.
    level = new Level(200, 14, 64);
    level->buildTestMap();

    // Player setup (temporary - will move to PlayerSoldier).
    if (!playerTex.loadFromFile("Sprites/Armaghan.png")) {
        std::cout << "ERROR: player texture failed\n";
    }
    playerSprite.setTexture(playerTex);
    playerSprite.setScale(0.2f, 0.2f);
    playerWidth = playerTex.getSize().x * 0.2f;
    playerHeight = playerTex.getSize().y * 0.2f;

    playerX = 200.0f;
    playerY = 200.0f;
    maxSpeed = 300.0f;
    gravity = 1500.0f;
    velocityX = 0.0f;
    velocityY = 0.0f;
    isOnGround = false;
    initialJumpSpeed = -700.0f;

    cameraX = 0.0f;
    cameraY = 0.0f;
    if (!bgTex.loadFromFile("Sprites/Background.png")) {
        std::cout << "ERROR: background texture failed\n";
    }
    bgSprite.setTexture(bgTex);

    // Stretch it to fill the whole window (1600x900)
    float scaleY = 900.0f / bgTex.getSize().y;
    bgSprite.setScale(scaleY, scaleY);   // uniform scale - no stretching
}

PlayState::~PlayState() {
    delete level;
}

void PlayState::handleInput(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        window.close();
    }
}

void PlayState::update(float dt) {
    // ----- Horizontal input -----
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))      velocityX = maxSpeed;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) velocityX = -maxSpeed;
    else                                                  velocityX = 0.0f;

    playerX += velocityX * dt;

    // ----- Gravity -----
    velocityY += gravity * dt;
    playerY += velocityY * dt;

    // ----- Ground collision (delegates to Level) -----
    float feetY = playerY + playerHeight + 1.0f;
    float leftX = playerX + 2.0f;
    float rightX = playerX + playerWidth - 2.0f;

    int hitRow;
    if (level->checkGroundBelow(feetY, leftX, rightX, hitRow)) {
        if (velocityY > 0.0f) {
            playerY = hitRow * level->getCellSize() - playerHeight;
            velocityY = 0.0f;
            isOnGround = true;
        }
    }
    else {
        isOnGround = false;
    }

    // ----- Jump -----
    if (isOnGround && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        velocityY = initialJumpSpeed;
        isOnGround = false;
    }

    playerSprite.setPosition(playerX - cameraX, playerY - cameraY);

    // ----- Camera follows player (centered horizontally) -----
    // Player's current position on screen (not in world)
    float screenPosX = playerX - cameraX;

    float leftThreshold = 1600.0f / 3.0f;        // ~533px - left boundary of dead zone
    float rightThreshold = 2.0f * 1600.0f / 3.5f; // ~1067px - right boundary of dead zone

    if (screenPosX < leftThreshold) {
        cameraX = playerX - leftThreshold;
    }
    else if (screenPosX > rightThreshold) {
        cameraX = playerX - rightThreshold;
    }
    // else: player is in middle third, camera does not move

    // Clamp so camera never goes past level edges
    if (cameraX < 0.0f) cameraX = 0.0f;
    float maxCamX = (float)(level->getWidthInPixels() - 1600);
    if (cameraX > maxCamX) cameraX = maxCamX;
}

void PlayState::render(sf::RenderWindow& window) {
    // --- Tiling parallax background ---
    float bgWidth = (float)bgTex.getSize().x * bgSprite.getScale().x;
    float bgHeight = (float)bgTex.getSize().y * bgSprite.getScale().y;

    // How far the background has scrolled (parallax at 20% speed)
    float bgScrollX = cameraX * 0.2f;

    // Find the starting offset so tiles line up seamlessly
    // fmod gives the remainder - tells us where the first tile begins
    float startOffset = bgScrollX - (int)(bgScrollX / bgWidth) * bgWidth;

    // Draw enough copies to always cover the full screen width
    // We need ceil(1600 / bgWidth) + 1 tiles to guarantee no gap
    int numTiles = (int)(1600.0f / bgWidth) + 2;

    for (int i = 0; i < numTiles; i++) {
        bgSprite.setPosition(i * bgWidth - startOffset, 0.0f);
        window.draw(bgSprite);
    }

    level->render(window, cameraX, cameraY);
    window.draw(playerSprite);
}