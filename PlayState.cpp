#include "PlayState.h"

PlayState::PlayState()
    : level(nullptr), entities(nullptr), player(nullptr),
    cameraX(0.0f), cameraY(0.0f),
    lives(3), respawnTimer(-1.0f),
    spawnX(200.0f), spawnY(200.0f),
    respawnPrototype(nullptr)
{
    std::cout << "PlayState Initialized.\n";

    // Creating world first, Player constructor needs a valid Level* to store, so
    // build the world. (Width = 200 tiles, Height = 14, cell = 64px)
    level = new Level(200, 22, 64);
    level->buildTestMap();

    entities = new EntityManager(64);

    // Start with Eri (she has the existing Char.png sprite).
    // EntityManager takes ownership; player is a non-owning view.
    player = new EriPlayer(200.0f, 200.0f, level, entities);
    entities->add(player);
    // Prototype mirrors the active character; used by respawnPlayer() via createSelf().
    respawnPrototype = player->createSelf(0.0f, 0.0f, level, entities);

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

    if (!bgTexPlain.loadFromFile("Sprites/Plain_Biome.png"))
        std::cout << "ERROR: Plain_Biome texture failed\n";
    if (!bgTexAerial.loadFromFile("Sprites/Aerial_Biome.png"))
        std::cout << "ERROR: Aerial_Biome texture failed\n";
    if (!bgTexAquatic.loadFromFile("Sprites/Aquatic_Biome.png"))
        std::cout << "ERROR: Aquatic_Biome texture failed\n";

    bgSprite.setTexture(bgTexPlain);
    float scaleY = 900.0f / bgTexPlain.getSize().y;
    bgSprite.setScale(scaleY, scaleY);
}

PlayState::~PlayState()
{
    delete respawnPrototype;
    respawnPrototype = nullptr;

    delete entities;
    entities = nullptr;
    player = nullptr; // dangling pointer guard (Entity manager will delete player)

    delete level;
    level = nullptr;
}

void PlayState::handleInput(sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scan::Escape)
            window.close();

        // ── Character switching via runtime polymorphism (TAB key) ────────────
        // player->createNext() is a pure virtual call — no if/else needed here.
        // Each concrete character class returns the next one in the cycle.
        if (event.key.scancode == sf::Keyboard::Scan::Z && player != nullptr) {
            Player* next = player->createNext(
                player->getPosX(), player->getPosY(), level, entities);
            // Update prototype so respawn recreates the newly selected character.
            delete respawnPrototype;
            respawnPrototype = next->createSelf(0.0f, 0.0f, level, entities);

            // Carry dev mode over to the new character before the old one is freed.
            bool wasDevMode = player->isDevMode();
            player->deactivateEntity();            // manager will free old player
            entities->add(next);
            player = next;
            if (wasDevMode) player->toggleDevMode();

            // Notify all enemies of the new player pointer so their AI retargets.
            // Without this, enemies keep chasing the old deactivated player.
            int n = entities->getCount();
            for (int i = 0; i < n; i++) {
                Entity* e = entities->getEntity(i);
                if (e && e->getIsActive())
                    e->onPlayerRespawn(player);
            }
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

    // Check player death BEFORE removeDead() frees the memory.
    // Save character index NOW while the pointer is still valid.
    if (player != nullptr && !player->getIsActive()) {
        // Clone the prototype BEFORE removeDead() frees the dead player's memory.
        delete respawnPrototype;
        respawnPrototype = player->createSelf(0.0f, 0.0f, level, entities);
        player = nullptr;
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
        // ── Horizontal camera (right deadzone) ───────────────────────────────
        float playerScreenX  = player->getPosX() - cameraX;
        float rightThreshold = 1600.0f - (1600.0f / 3.0f);
        if (playerScreenX > rightThreshold)
            cameraX = player->getPosX() - rightThreshold;

        if (cameraX < 0.0f) cameraX = 0.0f;
        float maxCamX = (float)(level->getWidthInPixels() - 1600);
        if (cameraX > maxCamX) cameraX = maxCamX;

        // ── Vertical camera (top/bottom deadzone) ────────────────────────────
        float playerScreenY   = player->getPosY() - cameraY;
        float topThreshold    = 900.0f / 3.0f;
        float bottomThreshold = 900.0f * 2.0f / 3.0f;

        if (playerScreenY < topThreshold)
            cameraY = player->getPosY() - topThreshold;
        if (playerScreenY > bottomThreshold)
            cameraY = player->getPosY() - bottomThreshold;

        if (cameraY < 0.0f) cameraY = 0.0f;
        float maxCamY = (float)(level->getHeightInPixels() - 900);
        if (cameraY > maxCamY) cameraY = maxCamY;
    }
}

void PlayState::render(sf::RenderWindow& window)
{
    // Pick background based on which biome the camera is currently in.
    // Plain: cols 0-66 (0 to 4288px), Aerial: cols 67-133 (4288-8576px), Aquatic: rest
    float plainEnd  = 67.0f  * 64.0f;  // 4288px
    float aerialEnd = 134.0f * 64.0f;  // 8576px

    sf::Texture* activeBgTex;
    if      (cameraX < plainEnd)  activeBgTex = &bgTexPlain;
    else if (cameraX < aerialEnd) activeBgTex = &bgTexAerial;
    else                          activeBgTex = &bgTexAquatic;

    bgSprite.setTexture(*activeBgTex, false);
    float scaleY = 900.0f / activeBgTex->getSize().y;
    bgSprite.setScale(scaleY, scaleY);

    float bgWidth     = (float)activeBgTex->getSize().x * scaleY;
    float bgScrollX   = cameraX * 0.2f;
    float startOffset = bgScrollX - (int)(bgScrollX / bgWidth) * bgWidth;
    int   numTiles    = (int)(1600.0f / bgWidth) + 2;

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
    // spawnX / spawnY are SCREEN offsets — convert to world coords using the
    // current camera so the player always reappears within the visible area.
    float worldX = cameraX + spawnX;
    float worldY = cameraY + spawnY;

    // Recreate the same character type via runtime polymorphism — no switch needed.
    // respawnPrototype was cloned from the dying player before removeDead() freed it.
    player = respawnPrototype->createSelf(worldX, worldY, level, entities);
    entities->add(player);

    // Update every living entity so enemies retarget the new player.
    int n = entities->getCount();
    for (int i = 0; i < n; i++) {
        Entity* e = entities->getEntity(i);
        if (e && e->getIsActive())
            e->onPlayerRespawn(player);
    }

    // No camera snap needed — player spawned inside the current view.
    respawnTimer = -1.0f;
    std::cout << "Player respawned at screen (" << spawnX << ", " << spawnY
              << "). Lives left: " << lives << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Enemy batch spawner
// ─────────────────────────────────────────────────────────────────────────────

void PlayState::spawnRebelBatch(float x, float y, int count)
{
    static constexpr float SPACING = 128.0f; // 2 blocks apart
    for (int i = 0; i < count; i++)
        entities->add(new RebelSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnShieldedBatch(float x, float y, int count)
{
    static constexpr float SPACING = 128.0f; // 2 blocks apart
    for (int i = 0; i < count; i++)
        entities->add(new ShieldedSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnBazookaBatch(float x, float y, int count)
{
    static constexpr float SPACING = 192.0f; // 3 blocks apart — rockets need room
    for (int i = 0; i < count; i++)
        entities->add(new BazookaSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnGrenadeBatch(float x, float y, int count)
{
    static constexpr float SPACING = 128.0f; // 2 blocks apart
    for (int i = 0; i < count; i++)
        entities->add(new GrenadeSoldier(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnParatrooperBatch(float x, float y, int count)
{
    static constexpr float SPACING = 192.0f; // 3 blocks apart — parachutes need clearance
    for (int i = 0; i < count; i++)
        entities->add(new Paratrooper(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnZombieBatch(float x, float y, int count)
{
    static constexpr float SPACING = 128.0f; // 2 blocks apart
    for (int i = 0; i < count; i++)
        entities->add(new Zombie(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnMummyBatch(float x, float y, int count)
{
    static constexpr float SPACING = 128.0f; // 2 blocks apart
    for (int i = 0; i < count; i++)
        entities->add(new MummyWarrior(x + i * SPACING, y, level, player, entities));
}

void PlayState::spawnMartianBatch(float x, float y, int count)
{
    static constexpr float SPACING = 256.0f; // 4 blocks apart — martians need wide spacing (flying phase)
    for (int i = 0; i < count; i++)
        entities->add(new Martian(x + i * SPACING, y, level, player, entities));
}
