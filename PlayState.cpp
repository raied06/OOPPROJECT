#include "PlayState.h"

PlayState::PlayState() {
    std::cout << "PlayState Initialized.\n";

    //initialize level constraints
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

    if (!wallTex1.loadFromFile("Sprites/blocks/grass_block_side.png")) {
        std::cout << "ERROR: Failed to load grass texture!\n";
    }
    wallSprite1.setTexture(wallTex1);

    //initialize player stats
    player_x = 380.0f;
    player_y = 610.0f;
    max_speed = 5.0f;
    velocityX = 0.0f;
    velocityY = 0.0f;
    acceleration = 0.5f;

    float scale_x = 0.2f;
    float scale_y = 0.2f;
    int raw_img_y = 470;
    int Pheight = raw_img_y * scale_y;

    maxJumpHeight = Pheight * 1.5f;
    isJumping = false;
    positionBeforeJump = 0.0f;
    jumpStart = 0.0f;
    fall = false;

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
    //real-time continuous input checking goes here

    //horizontal Movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocityX += acceleration;
        if (velocityX > max_speed) velocityX = max_speed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocityX -= acceleration;
        if (velocityX < -max_speed) velocityX = -max_speed;
    }
    else {
        velocityX = 0.0f;
    }

    player_x += velocityX;

    // Vertical Movement (Jump Logic)
    if (!isJumping && !fall) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            isJumping = true;
            positionBeforeJump = player_y;
        }
    }

    if (isJumping) {
        velocityY -= acceleration;
        if (velocityY < -max_speed) velocityY = -max_speed;

        player_y += velocityY;
        jumpStart += velocityY;

        if (jumpStart <= -maxJumpHeight) {
            isJumping = false;
            jumpStart = 0.0f;
            fall = true;
        }
    }
    else if (fall) {
        velocityY += acceleration;
        player_y += velocityY;

        if (player_y >= positionBeforeJump) {
            fall = false;
            player_y = positionBeforeJump; //snap to ground to prevent sinking
        }
    }
    else {
        velocityY = 0.0f;
    }

    //apply the final calculated position to the sprite
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