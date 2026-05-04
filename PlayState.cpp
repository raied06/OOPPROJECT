#include "PlayState.h"

PlayState::PlayState() {
    std::cout << "PlayState Initialized.\n";

    //initializing level constraints
    cell_size = 64;
    height = 14;
    width = 110;

    //manually allocate the 2D array with raw pointers. 
    lvl = new char* [height];
    for (int i = 0; i < height; i++) {
        lvl[i] = new char[width] {'\0'};
    }

    lvl[11][5] = 'g';
    lvl[11][6] = 'g';
    lvl[11][7] = 'g';
    lvl[11][8] = 'g';
    lvl[11][9] = 'g';
    lvl[11][10] = 'g';
    lvl[11][11] = 'g';
    lvl[11][12] = 'g';
    lvl[11][13] = 'g';
    lvl[11][14] = 'g';

    if (!wallTex1.loadFromFile("Sprites/blocks/grass_block_side.png")) {
        std::cout << "ERROR: Failed to load grass texture!\n";
    }
    wallSprite1.setTexture(wallTex1);

    //initialize player stats
    player_x = 380.0f;
    player_y = 610.0f;
    max_speed = 300.0f;
    gravity = 1500.0f;
    velocityX = 0.0f;
    velocityY = 0.0f;
    acceleration = 0.5f;

    float scale_x = 0.2f;
    float scale_y = 0.2f;
    int raw_img_y = 470;
    int Pheight = raw_img_y * scale_y;

    isOnGround = true;
    initialJumpSpeed = -700.0f; // The instant upward burst

    if (!playerTex.loadFromFile("Sprites/Character.png")) {
        std::cout << "ERROR: Failed to load player texture!\n";
    }
    playerSprite.setTexture(playerTex);
    playerSprite.setScale(scale_x, scale_y);
    playerSprite.setPosition(player_x, player_y);
}
//destroying pointers
PlayState::~PlayState() {
    for (int i = 0; i < height; i++) {
        delete[] lvl[i];
    }
    delete[] lvl;
}

void PlayState::handleInput(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        window.close();
    }
}

void PlayState::update(float dt) {
    // Horizontal Movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocityX = max_speed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocityX = -max_speed;
    }
    else {
        velocityX = 0.0f;
    }

    // Apply horizontal speed multiplied by delta time
    player_x += velocityX * dt;

    // 2. Vertical Movement & Physics
    // Gravity is ALWAYS pulling you down, every single frame.
    velocityY += gravity * dt;

    // Apply vertical speed multiplied by delta time
    player_y += velocityY * dt;

    // 3. Ground Collision (Hardcoded floor at 610.0f)
    if (player_y >= 610.0f) {
        player_y = 610.0f;     // Snap exactly to the floor
        velocityY = 0.0f;      // KILL the downward momentum (This was your bug)
        isOnGround = true;     // We are safely on the dirt
    }
    else {
        isOnGround = false;    // We are in the air
    }

    // 4. The Jump Trigger
    // You can ONLY jump if you are currently touching the ground
    if (isOnGround && sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        velocityY = initialJumpSpeed; // Apply instant negative (upward) speed
        isOnGround = false;
    }

    // Update the visual sprite with the new math
    playerSprite.setPosition(player_x, player_y);
}

void PlayState::render(sf::RenderWindow& window) {
    // Note: window.clear() and window.display() are called by the Game loop, NOT here.

    //render the level map first
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (lvl[i][j] == 'g') {
                wallSprite1.setPosition(j * cell_size, i * cell_size);
                window.draw(wallSprite1);
            }
        }
    }

    //render the player on top
    window.draw(playerSprite);
}