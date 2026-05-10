#include "PlayState.h"

PlayState::PlayState()
    : level(nullptr), entities(nullptr), player(nullptr),
    cameraX(0.0f), cameraY(0.0f),
    lives(3), respawnTimer(-1.0f),
    spawnX(200.0f), spawnY(200.0f)
{
    std::cout << "PlayState Initialized.\n";

    // Creating world first, Player constructor needs a valid Level* to store, so
    // build the world. (Width = 200 tiles, Height = 14, cell = 64px)
    level = new Level(200, 14, 64);
    level->buildTestMap();

    entities = new EntityManager(64);

    // Spawn the player. EntityManager takes ownership from this point.
    // We keep a raw non-owning pointer for camera tracking.
    // Player also needs the EntityManager* so it can spawn projectiles.
    player = new Player(200.0f, 200.0f, level, entities);
    entities->add(player); // manager owns it now

    // ── Spawn initial enemy batches ──────────────────────────────────────────
    // World is 12800px wide. 8 enemy types, one batch every 1400px.
    spawnRebelBatch      ( 1400.0f, 200.0f, 3);
    spawnShieldedBatch   ( 2800.0f, 200.0f, 2);
    spawnBazookaBatch    ( 4200.0f, 200.0f, 2);
    spawnGrenadeBatch    ( 5600.0f, 200.0f, 2);
    spawnZombieBatch     ( 7000.0f, 200.0f, 4);
    spawnMummyBatch      ( 8400.0f, 200.0f, 2);
    spawnMartianBatch    ( 9800.0f, 100.0f, 2);
    spawnParatrooperBatch(11200.0f,   0.0f, 3);

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

    // updateAll calls each projectile's update(), which internally calls
    // checkEntityCollisions() via virtual dispatch — no explicit collision
    // step needed here anymore.
    entities->updateAll(dt);

    // Clamp enemies to right screen edge — left side intentionally unclamped
    // so enemies left behind by the player stay where they are.
    for (int i = 0; i < entities->getCount(); i++) {
        Entity* e = entities->getEntity(i);
        if (e && e->getIsActive())
            e->applyScreenClamp(cameraX);
    }

    // Check player death BEFORE removeDead() frees the memory — after that the
    // pointer is dangling and getIsActive() would be undefined behaviour.
    if (player != nullptr && !player->getIsActive()) {
        player = nullptr; // drop the non-owning pointer; manager will free it below
        lives--;
        if (lives > 0) {
            respawnTimer = RESPAWN_DELAY;
            std::cout << "Player died. Lives remaining: " << lives << "\n";
        }
        else {
            std::cout << "Game Over — no lives remaining.\n";
        }
    }

    entities->removeDead();

    // Respawn countdown
    if (player == nullptr && lives > 0 && respawnTimer >= 0.0f) {
        respawnTimer -= dt;
        if (respawnTimer <= 0.0f)
            respawnPlayer();
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

// ─────────────────────────────────────────────────────────────────────────────
// Respawn
// ─────────────────────────────────────────────────────────────────────────────

void PlayState::respawnPlayer()
{
    // Create a fresh player at the spawn point and hand it to the manager.
    player = new Player(spawnX, spawnY, level, entities);
    entities->add(player);

    // Update every living entity so enemies retarget the new player.
    // Uses virtual dispatch — setNewPlayerTarget() is a no-op on non-enemies.
    int n = entities->getCount();
    for (int i = 0; i < n; i++) {
        Entity* e = entities->getEntity(i);
        if (e && e->getIsActive())
            e->onPlayerRespawn(player);
    }

    // Snap camera so the player appears in the left third of the screen.
    cameraX = spawnX - (1600.0f / 3.0f);
    if (cameraX < 0.0f) cameraX = 0.0f;
    respawnTimer = -1.0f;
    std::cout << "Player respawned. Lives left: " << lives << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Enemy batch spawner
// ─────────────────────────────────────────────────────────────────────────────

void PlayState::spawnRebelBatch(float x, float y, int count)
{
    static constexpr float SPACING = 80.0f;
    for (int i = 0; i < count; i++)
        entities->add(new RebelSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnShieldedBatch(float x, float y, int count)
{
    static constexpr float SPACING = 90.0f;
    for (int i = 0; i < count; i++)
        entities->add(new ShieldedSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnBazookaBatch(float x, float y, int count)
{
    static constexpr float SPACING = 120.0f; // wider gap — rockets need room
    for (int i = 0; i < count; i++)
        entities->add(new BazookaSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnGrenadeBatch(float x, float y, int count)
{
    static constexpr float SPACING = 100.0f;
    for (int i = 0; i < count; i++)
        entities->add(new GrenadeSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnParatrooperBatch(float x, float y, int count)
{
    static constexpr float SPACING = 120.0f;
    for (int i = 0; i < count; i++)
        entities->add(new Paratrooper(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnZombieBatch(float x, float y, int count)
{
    static constexpr float SPACING = 90.0f;
    for (int i = 0; i < count; i++)
        entities->add(new Zombie(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnMummyBatch(float x, float y, int count)
{
    static constexpr float SPACING = 100.0f;
    for (int i = 0; i < count; i++)
        entities->add(new MummyWarrior(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnMartianBatch(float x, float y, int count)
{
    static constexpr float SPACING = 200.0f; // martians need wide spacing (flying phase)
    for (int i = 0; i < count; i++)
        entities->add(new Martian(x + i * SPACING, y, level, player, entities));
}
