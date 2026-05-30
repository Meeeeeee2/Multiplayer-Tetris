#include "main.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <raylib.h>




// -----------------
//  Client Code
// -----------------

struct Piece
{
    int type = 0;
    int rotation = 0;
    int x = 3;
    int y = 0;
};

struct PlayerData
{
	int lines;
	int level;
	int score;
    Piece cPiece;
};



int playerID = -1;

bool gameRunning = true;

const int tileSize = 50;

const int paddingY = tileSize;

const float softDropSpeed = 0.06f;
const float gravityTable[20] =
{
	0.800f, // 0
	0.717f, // 1
	0.633f, // 2
	0.550f, // 3
	0.467f, // 4
	0.383f, // 5
	0.300f, // 6
	0.217f, // 7
	0.133f, // 8
	0.100f, // 9
	0.083f, // 10
	0.083f, // 11
	0.083f, // 12
	0.067f, // 13 
	0.067f, // 14
	0.067f, // 15 
	0.050f, // 16
	0.050f, // 17 
	0.033f, // 18 
	0.017f  // 19
};

unsigned char boards[2][200];

float delay = 0.0f;

std::vector<int> linesToClear;

PlayerData playerData[2];

bool gameOver = true;

Texture2D tileSet[4];


const int tetrominoes[7][4][4][4] =
{
    // I
    {
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0}
        },
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0}
        }
    },

    // O
    {
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        }
    },

    // T
    {
        {
            {0,1,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },

    // S
    {
        {
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        }
    },

    // Z
    {
        {
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },

    // J
    {
        {
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {1,1,0,0},
            {0,0,0,0}
        }
    },

    // L
    {
        {
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {1,0,0,0},
            {0,0,0,0}
        },
        {
            {1,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    }
};

int windowWidth = ((tileSize * 12) + (4 * tileSize)) * 2;
int windowHeight = (tileSize * 21) + paddingY;

void UnPackData(PackedData pData) {

	switch (pData.type) {
	case ROOM:
		std::cout << "Joining Room: " << pData.playerID << "\n";
		playerID = pData.playerID;
		break;

	case Start:
		gameOver = false;
        std::cout << "Game Starting";
		break;

   

	}
		
}

int idx(int x, int y)
{
	return y * 10 + x;
}

Vector2 coord(int idx)
{
	Vector2 result;
	result.x =  (idx % 10);
	result.y = floor(idx / 10);
	return result;
}

void DrawBoards()
{
	// Sides 
	for (int i = 0; i < 21; i++) {
		// Player 1
		DrawTexture(tileSet[3], 0, tileSize * i + paddingY, WHITE);
		DrawTexture(tileSet[3], tileSize * 11, tileSize * i + paddingY, WHITE);

		// Player 2
		DrawTexture(tileSet[3], windowWidth / 2, tileSize * i + paddingY, WHITE);
		DrawTexture(tileSet[3], tileSize * 11 + windowWidth / 2, tileSize * i + paddingY, WHITE);
	}
	// Bottom
	for (int i = 0; i < 10; i++) {
		// Player 1
		DrawTexture(tileSet[3], tileSize * (i + 1), tileSize * (20  + paddingY / tileSize), WHITE);
		
		// Player 2
		DrawTexture(tileSet[3], tileSize * (i + 1) + windowWidth / 2, tileSize * (20 + paddingY / tileSize), WHITE);
	}

	
	for (int j = 0; j < 2; j++)
	{
		int startPixelX = tileSize + (windowWidth / 2) * j;
		int startPixelY = paddingY;
		for (int i = 0; i < 200; i++)
		{
			int pieceIndex = boards[j][i];
			DrawTexture(tileSet[pieceIndex], 
				tileSize * (i % 10) + startPixelX,
				tileSize * floor(i / 10) + startPixelY,
				WHITE);
		}
	}
	
}

bool LoadTiles()
{
	for (int i = 0; i < 4; i++) {

		std::string path = std::string(RESOURCES_PATH) + "/tile" + std::to_string(i + 1) + ".png";
		Image tile = LoadImage(path.c_str());

		if (tile.data == nullptr)
		{
			std::cout << "Failed to load: " << path << "\n";
			continue;
		}
		
		tileSet[i] = LoadTextureFromImage(tile);
		UnloadImage(tile);

		tileSet[i].width *= tileSize / 10;
		tileSet[i].height *= tileSize / 10;
	}
	return EXIT_SUCCESS;
}

int main() {
	std::cout << "Starting Client\n";
	
	while (true)
	{
		if (InitEnet() == EXIT_FAILURE)
		{
			std::cout << "ENet Connection Failed, Trying again\n";
		}
		else 
		{
			std::cout << "Server Connection Established\n";
			break;
		}
	}
	
	
	
	//create window
	

	InitWindow(windowWidth, windowHeight, "Tetris (Client)");
	std::cout << "Width, Height" << windowWidth << "," << windowHeight << "\n";
	if (LoadTiles() != EXIT_SUCCESS)
	{
		std::cout << "Failed to load Textures\nExiting ...";
		return EXIT_FAILURE;
	}


	// Game Loop Goes here 
	for (int i = 0; i < 200; i++) {
		boards[0][i] = 255;
		boards[1][i] = 255;
	}


	while (!WindowShouldClose())
	{
		if (!gameOver)
		{
			//----------------------
			// Main Game Logic
			//----------------------
		}
		BeginDrawing();
		ClearBackground({ 50, 50, 50, 255 });
		DrawBoards();
		EndDrawing();

		HandleEnetEvents();

		
	}
	
	


	// End of game loop

	DisconnectENet(playerID);

	return EXIT_SUCCESS;
}