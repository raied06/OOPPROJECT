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
    int col = (int)(leftX / cell_size);
    int topRow = (int)(topY / cell_size);
    int bottomRow = (int)(bottomY / cell_size);

    return (isSolid(topRow, col) || isSolid(bottomRow, col));
}

bool Level::checkRightWall(float rightX, float topY, float bottomY) const
{
    int col = (int)(rightX / cell_size);
    int topRow = (int)(topY / cell_size);
    int bottomRow = (int)(bottomY / cell_size);

    return (isSolid(topRow, col) || isSolid(bottomRow, col));
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

void Level::buildTestMap()
{
    for (int j = 0; j < gridWidth; j++) {
        lvl[gridHeight - 1][j] = 'b';
    }
    for (int j = 0; j < gridWidth; j++) {
        lvl[gridHeight - 2][j] = 'd';
        lvl[gridHeight - 3][j] = 'g';
    }
    for (int j = 20; j < 28; j++) lvl[gridHeight - 8][j] = 'g';
    for (int j = 35; j < 42; j++) lvl[gridHeight - 12][j] = 'g';
    for (int j = 55; j < 60; j++) lvl[gridHeight - 10][j] = 'g';
}