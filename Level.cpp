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
}

Level::~Level()
{
	for (int i = 0; i < gridWidth; i++) {
		delete[] lvl[i];
	}
	delete[] lvl;
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
		blockType == 'd' || blockType == 's')
	{
		if (row < 0 || row >= gridHeight) return;
		if (col < 0 || col >= gridWidth)  return;
		lvl[row][col] = blockType;
	}
	else
		return;
}

void Level::destroyBlock(int row, int col)
{
	if (row < 0 || row >= gridHeight) 
		return;
	if (col < 0 || col >= gridWidth)  
		return;
	// exception: Bedrock is indestructible. So mandatrory check for it
	if (lvl[row][col] == 'b') 
		return;

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
	int row = (int)(x / cell_size);
	int col = (int)(y / cell_size);

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
	// Compute which range of tiles falls inside the visible window.
	// Without this, a 250-wide level renders 250*60 = 15000 sprites per
	// frame - that's a guaranteed lag spike.
	int startCol = (int)(cameraX / cell_size);
	int startRow = (int)(cameraY / cell_size);
	int endCol = startCol + ((int)window.getSize().x / cell_size) + 2;
	int endRow = startRow + ((int)window.getSize().y / cell_size) + 2;

	if (startCol < 0)             startCol = 0;
	if (startRow < 0)             startRow = 0;
	if (endCol > gridWidth)     endCol = gridWidth;
	if (endRow > gridHeight)    endRow = gridHeight;

	for (int i = startRow; i < endRow; i++) {
		for (int j = startCol; j < endCol; j++) {
			char c = lvl[i][j];
			if (c == '\0') continue;  // skip air tiles

			// Pick texture for this block type.
			// (Yes this is a tiny if-chain, but block types are strings of
			// raw data, not classes - polymorphism here would be overkill.)
			if (c == 'g') blockSprite.setTexture(grassTex);
			else if (c == 's') blockSprite.setTexture(stoneTex);
			else if (c == 'd') blockSprite.setTexture(dirtTex);
			else if (c == 'b') blockSprite.setTexture(stoneTex);  // placeholder
			else continue;

			// World position MINUS camera offset = screen position.
			float screenX = j * (float)cell_size - cameraX;
			float screenY = i * (float)cell_size - cameraY;
			blockSprite.setPosition(screenX, screenY);
			window.draw(blockSprite);
		}
	}
}

void Level::buildTestMap()
{
	// Fill the bottom row with bedrock (indestructible).
	for (int j = 0; j < gridWidth; j++) {
		lvl[gridHeight - 1][j] = 'b';
	}
	// Pile dirt under, grass on top, for the next 2 rows.
	for (int j = 0; j < gridWidth; j++) {
		lvl[gridHeight - 2][j] = 'd';
		lvl[gridHeight - 3][j] = 'g';
	}
	// Throw in a couple of floating platforms for jump-testing.
	for (int j = 20; j < 28; j++) lvl[gridHeight - 8][j] = 'g';
	for (int j = 35; j < 42; j++) lvl[gridHeight - 12][j] = 'g';
	for (int j = 55; j < 60; j++) lvl[gridHeight - 6][j] = 'g';
}

