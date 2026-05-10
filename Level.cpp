#include "Level.h"
#include <iostream>

Level::Level(int w, int h, int cell_size) : gridHeight(h), gridWidth(w), cell_size(cell_size)
{
    lvl = new char* [gridHeight];
    for (int i = 0; i < gridHeight; i++) {
        lvl[i] = new char[gridWidth];
        for (int j = 0; j < gridWidth; j++) {
            lvl[i][j] = '\0'; // Air
        }
    }

    if (!grassTex.loadFromFile("Sprites/blocks/grass_block_side.png")) {
        std::cout << "ERROR: grass texture failed to load\n";
    }
    if (!stoneTex.loadFromFile("Sprites/blocks/stone.png")) {
        std::cout << "ERROR: stone texture failed to load\n";
    }
    if (!dirtTex.loadFromFile("Sprites/blocks/dirt.png")) {
        std::cout << "ERROR: dirt texture failed to load\n";
    }
    if (!bedrockTex.loadFromFile("Sprites/blocks/bedrock.png")) {
        std::cout << "ERROR: dirt texture failed to load\n";
    }
}

Level::~Level()
{
    for (int i = 0; i < gridHeight; i++) {
        delete[] lvl[i];
        lvl[i] = nullptr;
    }
    delete[] lvl;
    lvl = nullptr;
}

char Level::getBlockType(int row, int col) const
{
    if (row < 0 || row >= gridHeight) return '\0';
    if (col < 0 || col >= gridWidth)  return '\0';
    return lvl[row][col];
}

int Level::getCellSize() const
{
    return cell_size;
}

int Level::getGridWidth() const
{
    return gridWidth;
}

int Level::getGridHeight() const
{
    return gridHeight;
}

int Level::getWidthInPixels() const
{
    return (gridWidth * cell_size);
}

int Level::getHeightInPixels() const
{
    return (gridHeight * cell_size);
}

void Level::setBlock(int row, int col, char blockType)
{
    if (blockType == '\0' || blockType == 'g' || blockType == 's' ||
        blockType == 'd' || blockType == 'b')
    {
        if (row < 0 || row >= gridHeight) return;
        if (col < 0 || col >= gridWidth)  return;
        lvl[row][col] = blockType;
    }
}

void Level::destroyBlock(int row, int col)
{
    if (row < 0 || row >= gridHeight) return;
    if (col < 0 || col >= gridWidth)  return;
    if (lvl[row][col] == 'b')         return; // bedrock is indestructible

    lvl[row][col] = '\0';
}

bool Level::isSolid(int row, int col) const
{
    if (row < 0 || row >= gridHeight) return false;
    if (col < 0 || col >= gridWidth)  return false;

    return (lvl[row][col] == 'g' || lvl[row][col] == 's' ||
        lvl[row][col] == 'd' || lvl[row][col] == 'b');
}

bool Level::isSolidAtPixel(float x, float y) const
{
    int col = (int)(x / cell_size);
    int row = (int)(y / cell_size);

    if (row < 0 || row >= gridHeight || col < 0 || col >= gridWidth) {
        return false;
    }


    return isSolid(row, col);
}

bool Level::checkGroundBelow(float feet, float leftX, float rightX, int& outRow) const
{
    int row = (int)(feet / cell_size);
    int leftCol = (int)(leftX / cell_size);
    int rightCol = (int)(rightX / cell_size);

    if (isSolid(row, leftCol) || isSolid(row, rightCol)) {
        outRow = row;
        return true;
    }
    return false;
}

bool Level::checkCeiling(float headY, float leftX, float rightX, int& outRow) const
{
    int row = (int)(headY / cell_size);
    int leftCol = (int)(leftX / cell_size);
    int rightCol = (int)(rightX / cell_size);

    if (isSolid(row, leftCol) || isSolid(row, rightCol)) {
        outRow = row;
        return true;
    }
    return false;
}

bool Level::checkLeftWall(float leftX, float topY, float bottomY) const
{
    int col       = (int)(leftX   / cell_size);
    int topRow    = (int)(topY    / cell_size);
    int bottomRow = (int)(bottomY / cell_size);

    for (int row = topRow; row <= bottomRow; row++)
        if (isSolid(row, col)) return true;
    return false;
}

bool Level::checkRightWall(float rightX, float topY, float bottomY) const
{
    int col       = (int)(rightX  / cell_size);
    int topRow    = (int)(topY    / cell_size);
    int bottomRow = (int)(bottomY / cell_size);

    for (int row = topRow; row <= bottomRow; row++)
        if (isSolid(row, col)) return true;
    return false;
}

void Level::render(sf::RenderWindow& window, float cameraX, float cameraY)
{
    // Computing that how much panels/tiles fall inside the visible 
    // window, without calculating this, the background will still move
    // and game will work, but, it would lag drastically and fps will drop
    // bcz a level with width 250 would render 200*60 = 12000 sprites per
    // frame.

    int startCol = (int)(cameraX / cell_size);
    int startRow = (int)(cameraY / cell_size);
    int endCol = startCol + ((int)window.getSize().x / cell_size) + 2;
    int endRow = startRow + ((int)window.getSize().y / cell_size) + 2;

    if (startCol < 0)        startCol = 0;
    if (startRow < 0)        startRow = 0;
    if (endCol > gridWidth)  endCol = gridWidth;
    if (endRow > gridHeight) endRow = gridHeight;

    for (int i = startRow; i < endRow; i++) {
        for (int j = startCol; j < endCol; j++) {
            char c = lvl[i][j];
            if (c == '\0') continue;

            if (c == 'g') blockSprite.setTexture(grassTex);
            else if (c == 's') blockSprite.setTexture(stoneTex);
            else if (c == 'd') blockSprite.setTexture(dirtTex);
            else if (c == 'b') blockSprite.setTexture(bedrockTex);
            else continue;

            // The position of the block would be World position - camera offset.
            float screenX = j * (float)cell_size - cameraX;
            float screenY = i * (float)cell_size - cameraY;
            blockSprite.setPosition(screenX, screenY);
            window.draw(blockSprite);
        }
    }
}

void Level::fillColumn(int col, int surfaceRow)
{
    if (surfaceRow < 0 || surfaceRow > 20) return;
    lvl[surfaceRow][col] = 'g';
    for (int r = surfaceRow + 1; r <= 20; r++)
        lvl[r][col] = (r <= surfaceRow + 4) ? 'd' : 's';
}

void Level::buildTestMap()
{
    // Grid: 200 cols wide, 22 rows tall (rows 0-21), 64px per cell.
    // Biomes: Plain(0-66), Aerial(67-133), Aquatic(134-199). Bedrock at row 21.

    for (int j = 0; j < gridWidth; j++)
        lvl[21][j] = 'b';

    // ── PLAIN BIOME (cols 0-66) ───────────────────────────────────────────────
    // Continuous terrain — NO pits, NO gaps. Max height change = 2 rows so
    // player can always jump up. Grass top, dirt below, stone deeper.
    for (int j = 0; j < 67; j++) {
        int surf = 19; // default flat floor

        // Hill 1: 1-col ramps, 2-row rise max, 9-wide peak
        if (j == 2)                surf = 18;
        if (j >= 3  && j <= 11)    surf = 17; // peak (9 wide)
        if (j == 12)               surf = 18;
        // flat valley (cols 13-21, 9 wide)
        // Hill 2
        if (j == 25)               surf = 18;
        if (j >= 26 && j <= 32)    surf = 17; // peak (9 wide)
        if (j == 33)               surf = 18;
        // flat valley (cols 33-41, 9 wide)
        // Hill 3
        if (j == 45)               surf = 18;
        if (j >= 46 && j <= 53)    surf = 17; // peak (9 wide)
        if (j == 54)               surf = 18;
        // flat valley (cols 53-61, 9 wide) — leads into aerial
        // small ramp at end
        if (j >= 64 && j <= 66)    surf = 18;

        fillColumn(j, surf);
    }

    // ── AERIAL BIOME (cols 67-133) ────────────────────────────────────────────
    // Stone floor at row 18 prevents enemies from falling to bedrock.
    // Floating stone platforms rise up to 2 rows per step above this floor.
    // 8-col platforms, 2-col gaps — enemies patrol the stone floor in gaps.

    // Floating platforms — each 2 rows above the previous (max jump rule)
    // Platform 1 — row 16, cols 67-74
    for (int j = 67; j <= 74; j++) { 
        lvl[16][j] = 's'; 
        lvl[18][j + 1] = 's';
    }
    // gap 75-76 (enemies walk on floor at row 18)
    // Platform 2 — row 14, cols 77-84
    for (int j = 75;  j <= 84;  j++) 
    {
        lvl[14][j] = 's';
        lvl[18][j] = 's';
    }
    lvl[17][77] = 's';
    lvl[17][78] = 's';
    lvl[17][79] = 's';
    // gap 85-86
    // Platform 3 — row 12, cols 87-94
    for (int j = 85;  j <= 94;  j++) lvl[12][j] = 's';
    // gap 95-96
    // Platform 4 — row 10, cols 97-104  (highest)
    for (int j = 95;  j <= 104; j++) lvl[10][j] = 's';
    // gap 105-106
    // Platform 5 — row 12, cols 107-114
    for (int j = 105; j <= 114; j++) lvl[12][j] = 's';
    // gap 115-116
    // Platform 6 — row 14, cols 117-124
    for (int j = 116; j <= 124; j++) lvl[14][j] = 's';
    // gap 125-126
    // Platform 7 — row 16, cols 127-133
    for (int j = 126; j <= 133; j++) lvl[16][j] = 's';

    // ── AQUATIC BIOME (cols 134-199) ──────────────────────────────────────────
    // Floating GRASS platforms — open pits below (enemies fall into water).
    // Max 2-row height difference between consecutive platforms.
    // 8-col wide, 2-col gaps.

    // Platform 1 — row 16, cols 134-141  (transition from aerial floor)
    for (int j = 134; j <= 141; j++) lvl[16][j] = 'g';
    // gap 142-143
    // Platform 2 — row 14, cols 144-151
    for (int j = 144; j <= 151; j++) lvl[14][j] = 'g';
    // gap 152-153
    // Platform 3 — row 12, cols 154-161
    for (int j = 154; j <= 161; j++) lvl[12][j] = 'g';
    // gap 162-163
    // Platform 4 — row 14, cols 164-171
    for (int j = 164; j <= 171; j++) lvl[14][j] = 'g';
    // gap 172-173
    // Platform 5 — row 16, cols 174-181  (deep underwater)
    for (int j = 174; j <= 181; j++) { 
        lvl[16][j] = 'g'; 
        lvl[18][j - 4] = 's';
    }
    // gap 182-183
    // Platform 6 — row 14, cols 184-191
    for (int j = 184; j <= 191; j++)
    {
        lvl[14][j] = 'g';
        lvl[18][j - 2] = 's';
    }
    // gap 192-193
    // Landing pad — row 19, cols 194-199
    for (int j = 194; j <= 199; j++) lvl[19][j] = 'g';
}