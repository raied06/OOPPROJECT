#include "PlayState.h"

// ─────────────────────────────────────────────────────────────────────────────
// Integer-to-buffer helper (no <cstdio> / <string> allowed — only iostream/SFML)
// ─────────────────────────────────────────────────────────────────────────────
// Writes a base-10 representation of `value` into `buf`. Always null-terminated.
static int intToBuf(int value, char* buf, int bufSize)
{
    if (bufSize < 2) { if (bufSize > 0) buf[0] = '\0'; return 0; }

    int i = 0;
    if (value < 0) { buf[i++] = '-'; value = -value; }
    if (value == 0) { buf[i++] = '0'; buf[i] = '\0'; return i; }

    // Largest power of 10 that fits the value, so we can write digits high→low.
    int divisor = 1;
    int temp = value;
    while (temp >= 10) { temp /= 10; divisor *= 10; }

    while (divisor > 0 && i < bufSize - 1) {
        buf[i++] = static_cast<char>('0' + (value / divisor));
        value %= divisor;
        divisor /= 10;
    }
    buf[i] = '\0';
    return i;
}

PlayState::PlayState()
    : level(nullptr), entities(nullptr), player(nullptr),
    cameraX(0.0f), cameraY(0.0f),
    lives(3), respawnTimer(-1.0f),
    spawnX(200.0f), spawnY(200.0f),
    respawnPrototype(nullptr),
    score(0), comboCount(0), comboTimer(0.0f),
    currentLevel(1), levelTransitionTimer(-1.0f),
    hudFontLoaded(false)
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

    if (!bgTexPlain.loadFromFile("Sprites/Plain_Biome.png"))
        std::cout << "ERROR: Plain_Biome texture failed\n";
    if (!bgTexAerial.loadFromFile("Sprites/Aerial_Biome.png"))
        std::cout << "ERROR: Aerial_Biome texture failed\n";
    if (!bgTexAquatic.loadFromFile("Sprites/Aquatic_Biome.png"))
        std::cout << "ERROR: Aquatic_Biome texture failed\n";

    bgSprite.setTexture(bgTexPlain);
    float scaleY = 900.0f / bgTexPlain.getSize().y;
    bgSprite.setScale(scaleY, scaleY);

    // ── HUD setup ────────────────────────────────────────────────────────────
    // Try a shipped font first, then fall back to common Windows system fonts.
    hudFontLoaded =
        hudFont.loadFromFile("Sprites/hud.ttf")            ||
        hudFont.loadFromFile("C:/Windows/Fonts/arial.ttf") ||
        hudFont.loadFromFile("C:/Windows/Fonts/consola.ttf");
    if (!hudFontLoaded)
        std::cout << "PlayState: no HUD font found — score will be hidden\n";

    scoreText.setFont(hudFont);
    scoreText.setCharacterSize(36);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(2.0f);
    scoreText.setPosition(20.0f, 16.0f);
    scoreText.setString("SCORE: 0");

    comboText.setFont(hudFont);
    comboText.setCharacterSize(28);
    comboText.setFillColor(sf::Color(255, 200, 50));  // gold
    comboText.setOutlineColor(sf::Color::Black);
    comboText.setOutlineThickness(2.0f);
    comboText.setPosition(20.0f, 60.0f);

    // Bottom-left: grenade counter
    grenadeText.setFont(hudFont);
    grenadeText.setCharacterSize(32);
    grenadeText.setFillColor(sf::Color(180, 255, 180));
    grenadeText.setOutlineColor(sf::Color::Black);
    grenadeText.setOutlineThickness(2.0f);
    grenadeText.setPosition(20.0f, 840.0f);
    grenadeText.setString("GRENADES: 0");

    // Bottom-middle: current level indicator
    levelText.setFont(hudFont);
    levelText.setCharacterSize(32);
    levelText.setFillColor(sf::Color::White);
    levelText.setOutlineColor(sf::Color::Black);
    levelText.setOutlineThickness(2.0f);
    // Position computed in startLevel() once the string width is known.

    // Center-screen "LEVEL N COMPLETE!" overlay
    levelCompleteText.setFont(hudFont);
    levelCompleteText.setCharacterSize(72);
    levelCompleteText.setFillColor(sf::Color(255, 230, 60));
    levelCompleteText.setOutlineColor(sf::Color::Black);
    levelCompleteText.setOutlineThickness(4.0f);

    // Spawn the first level's enemies and refresh the level HUD.
    startLevel(1);
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

    // ── Level-complete transition ────────────────────────────────────────────
    // While the overlay is showing, freeze the world — no entity updates, no
    // camera changes. When the timer expires, start the next level.
    if (levelTransitionTimer > 0.0f) {
        levelTransitionTimer -= dt;
        if (levelTransitionTimer <= 0.0f) {
            levelTransitionTimer = -1.0f;
            if (currentLevel < MAX_LEVEL) {
                startLevel(currentLevel + 1);
            }
            else {
                std::cout << "All levels complete!\n";
                // Stay on final level — could swap to a GameComplete state here.
            }
        }
        return; // freeze world during overlay
    }

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

    // ── Score: detect newly-dead enemies BEFORE removeDead() frees them ─────
    // getScoreValue() is a virtual method on Entity (returns 0 for non-enemies),
    // so this loop relies purely on dynamic dispatch — no dynamic_cast needed.
    {
        int n = entities->getCount();
        for (int i = 0; i < n; i++) {
            Entity* e = entities->getEntity(i);
            if (e && !e->getIsActive() && e->getScoreValue() > 0)
                awardKill(e->getScoreValue());
        }
    }

    // ── Tick combo window ───────────────────────────────────────────────────
    if (comboTimer > 0.0f) {
        comboTimer -= dt;
        if (comboTimer <= 0.0f) {
            comboCount = 0;
            comboText.setString(""); // hide combo indicator
        }
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

        // ── Level-end trigger ────────────────────────────────────────────────
        // Crossing LEVEL_END_X (near the right edge of the world) triggers
        // the "Level N Complete!" overlay. The next update() will run the
        // transition timer and call startLevel(currentLevel + 1).
        if (player->getPosX() >= LEVEL_END_X) {
            levelTransitionTimer = LEVEL_TRANSITION_DURATION;

            char digits[8];
            intToBuf(currentLevel, digits, sizeof(digits));
            sf::String s = "LEVEL ";
            s += digits;
            s += " COMPLETE!";
            levelCompleteText.setString(s);

            // Centre the overlay on screen now that the string is set.
            sf::FloatRect b = levelCompleteText.getLocalBounds();
            levelCompleteText.setPosition(800.0f - b.width * 0.5f,
                                          450.0f - b.height * 0.5f);
        }
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

    // ── HUD ─────────────────────────────────────────────────────────────────
    // Drawn LAST so it sits on top of everything, in screen-space (no camera).
    if (hudFontLoaded) {
        window.draw(scoreText);
        if (comboCount >= 1)
            window.draw(comboText);

        // Bottom-left grenade counter — refresh every frame.
        if (player) {
            char digits[8];
            intToBuf(player->getGrenadeCount(), digits, sizeof(digits));
            sf::String g = "GRENADES: ";
            g += digits;
            grenadeText.setString(g);
        }
        window.draw(grenadeText);

        // Bottom-middle current-level indicator.
        window.draw(levelText);

        // Big centre overlay during the level-complete transition.
        if (levelTransitionTimer > 0.0f)
            window.draw(levelCompleteText);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Level progression
// ─────────────────────────────────────────────────────────────────────────────

void PlayState::startLevel(int n)
{
    currentLevel = n;

    // Clear every entity except the player. This kills leftover projectiles
    // and any surviving enemies from the previous level. removeDead() will
    // free them on the next update() pass.
    int count = entities->getCount();
    for (int i = 0; i < count; i++) {
        Entity* e = entities->getEntity(i);
        if (e && e != player)
            e->deactivateEntity();
    }

    // Reset player to start of world.
    if (player) {
        player->setPosition(0.0f + spawnX, spawnY);
        player->setVelocity(0.0f, 0.0f);
    }
    cameraX = 0.0f;
    cameraY = 0.0f;

    // Place enemies for the chosen level.
    if (n == 1)      spawnLevel1Enemies();
    else if (n == 2) spawnLevel2Enemies();

    // Refresh "LEVEL N" indicator, recentred horizontally.
    char digits[8];
    intToBuf(currentLevel, digits, sizeof(digits));
    sf::String s = "LEVEL ";
    s += digits;
    levelText.setString(s);
    sf::FloatRect b = levelText.getLocalBounds();
    levelText.setPosition(800.0f - b.width * 0.5f, 840.0f);

    std::cout << "── Level " << currentLevel << " start ──\n";
}

void PlayState::spawnLevel1Enemies()
{
    // World biomes:
    //   Plain   : x =   0 –  4288  (cols 0–66)
    //   Aerial  : x = 4288 –  8576 (cols 67–133)
    //   Aquatic : x = 8576 – 12800 (cols 134+)
    //
    // Level 1 — introduction. Light enemy density across all three biomes.

    // ── Plain biome — basic infantry ────────────────────────────────────────
    spawnRebelBatch    ( 1400.0f, 200.0f, 3);
    spawnShieldedBatch ( 2600.0f, 200.0f, 2);
    spawnBazookaBatch  ( 3600.0f, 200.0f, 1);

    // ── Aerial biome — paratroopers + martians ──────────────────────────────
    spawnParatrooperBatch(4800.0f,   0.0f, 3);
    spawnMartianBatch    (6200.0f, 100.0f, 2);
    spawnGrenadeBatch    (7600.0f, 200.0f, 2);

    // ── Aquatic biome — undead + last stand of rebels ───────────────────────
    spawnZombieBatch (9000.0f, 200.0f, 3);
    spawnMummyBatch  (10400.0f, 200.0f, 2);
    spawnRebelBatch  (11800.0f, 200.0f, 3);
}

void PlayState::spawnLevel2Enemies()
{
    // Same biomes / same tile layout as Level 1 — only enemy density and mix
    // change. Tougher units appear earlier and more often.

    // ── Plain biome — heavier infantry presence ─────────────────────────────
    spawnRebelBatch    ( 1200.0f, 200.0f, 4);
    spawnShieldedBatch ( 2400.0f, 200.0f, 3);
    spawnBazookaBatch  ( 3400.0f, 200.0f, 2);
    spawnGrenadeBatch  ( 4000.0f, 200.0f, 2);

    // ── Aerial biome — denser air drop + martian patrol ─────────────────────
    spawnParatrooperBatch(4800.0f,   0.0f, 4);
    spawnMartianBatch    (6000.0f, 100.0f, 3);
    spawnGrenadeBatch    (7200.0f, 200.0f, 3);
    spawnParatrooperBatch(8000.0f,   0.0f, 2);

    // ── Aquatic biome — full undead push at the finale ──────────────────────
    spawnZombieBatch (9000.0f, 200.0f, 5);
    spawnMummyBatch  (10200.0f, 200.0f, 3);
    spawnZombieBatch (11200.0f, 200.0f, 3);
    spawnRebelBatch  (12000.0f, 200.0f, 4);
}

// ─────────────────────────────────────────────────────────────────────────────
// Scoring
// ─────────────────────────────────────────────────────────────────────────────

void PlayState::awardKill(int baseValue)
{
    // Combo grows only if the window from a previous kill is still open.
    if (comboTimer > 0.0f) {
        if (comboCount < COMBO_CAP) comboCount++;
    }
    else {
        comboCount = 0; // first kill of a new chain
    }
    comboTimer = COMBO_WINDOW;

    // Multiplier scales 1.0 → 3.0 as combo grows from 0 → 8.
    float mult = 1.0f + 0.25f * static_cast<float>(comboCount);
    score += static_cast<int>(baseValue * mult);

    // Rebuild "SCORE: <n>"
    char digits[16];
    intToBuf(score, digits, sizeof(digits));
    sf::String line = "SCORE: ";
    line += digits;
    scoreText.setString(line);

    // Rebuild "xN COMBO!"  (hidden when no combo)
    if (comboCount >= 1) {
        intToBuf(comboCount + 1, digits, sizeof(digits));
        sf::String c = "x";
        c += digits;
        c += " COMBO!";
        comboText.setString(c);
    }
    else {
        comboText.setString("");
    }
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
