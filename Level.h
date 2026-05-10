#pragma once
#include <SFML/Graphics.hpp>

class Level
{
private:
	char** lvl; // '\0' = air, 'g' = grass, 's' = stone, 'd' = dirt, 'b' = bedrock
	int gridHeight;
	int gridWidth;
	int cell_size;
//TEXTURES FOR BLOCKS
	sf::Texture grassTex;
	sf::Texture stoneTex;
	sf::Texture dirtTex;
	sf::Texture bedrockTex;
	sf::Sprite blockSprite; // reused for every draw call
public:
// CONSTRUCTOR
	Level(int w, int h, int cell_size);
	// Rule of Three / Five: this class owns raw memory.
	// Copying it would double-free. Forbid copy entirely.
	Level(const Level&) = delete;
	Level& operator=(const Level&) = delete;
//DESTRUCTOR
	~Level();
// GETTER
	char getBlockType(int row, int col) const;
	int getCellSize() const;
	int getGridWidth() const;
	int getGridHeight() const;
	int getWidthInPixels() const;
	int getHeightInPixels() const;
// SETTERS
	void setBlock(int row, int col, char blockType);
	void destroyBlock(int row, int col); // exception for bedrock block, not applicable on it
// OTHER FUNCTIONS
	bool isSolid(int row, int col) const;
	bool isSolidAtPixel(float x, float y) const;

	bool checkGroundBelow(float feet, float leftX, float rightX, int& outRow) const;
	bool checkCeiling(float headY, float leftX, float rightX, int& outRow) const;
	bool checkLeftWall(float leftX, float topY, float bottomY) const;
	bool checkRightWall(float rightX, float topY, float bottomY) const;

	void render(sf::RenderWindow& window, float cameraX, float cameraY); // Only draws visible tiles

	void fillColumn(int col, int surfaceRow); // fills grass/dirt/stone from surface down to row 20
	void buildTestMap();
};