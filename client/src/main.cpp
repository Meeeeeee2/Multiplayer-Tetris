#include "main.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <raylib.h>
#include <random>
#include <thread>



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
    PlayerData() 
    {
        lines = 0;
        level = 0;
        score = 0;
        cPiece.type = -1;
    }
};



int playerID = -1;

bool gameRunning = true;

const int tileSize = 50;

const int paddingY = tileSize;

const float softDropSpeed = 0.06f;
const float gravityTable[20] =
{
	0.100f, // 0
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

float dropTimer = 0.0f;

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

struct SpawnData
{
    int x;
    int y;
    int rotation;
};

const SpawnData spawnData[7] =
{
    // I
    {3, -1, 0},

    // O
    {4, 0, 0},

    // T
    {4, -1, 2},

    // S
    {4, 0, 0},

    // Z
    {4, 0, 0},

    // J
    {4, -1, 2},

    // L
    {4, -1, 2}
};

std::mt19937 rng[2];
std::uniform_int_distribution<int> pieceDist(0, 6);




int windowWidth = ((tileSize * 12) + (4 * tileSize)) * 2;
int windowHeight = (tileSize * 21) + paddingY;

void SpawnPiece(int playerIndex);

void StickCurrentPiece(int playerIndex);

void DropCurrentPiece(int playerIndex);

bool CanDrop(int playerIndex);

bool CanMoveCurrentPiece(int direction, int playerIndex);

void MoveCurrentPiece(int direction, int playerIndex);

bool CanRotateCurrentPiece(int playerIndex);

void RotateCurrentPiece(int playerIndex);

void HandleInputs();

void StartMatch(int seed);

void UnPackData(PackedData pData);

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

void DrawBoards();

bool LoadTiles();


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
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);

	InitWindow(windowWidth, windowHeight, "Tetris (Client)");
	std::cout << "Width, Height" << windowWidth << "," << windowHeight << "\n";

    // Load Required Textures
	if (LoadTiles() != EXIT_SUCCESS)
	{
		std::cout << "Failed to load Textures\nExiting ...";
		return EXIT_FAILURE;
	}


	for (int i = 0; i < 200; i++) {
		boards[0][i] = 255;
		boards[1][i] = 255;
	}

   

	while (!WindowShouldClose())
	{
		if (!gameOver)
        {
            float dt = GetFrameTime();

            if (delay > 0.0001)
            {
                delay -= dt;
            }
            else {
                delay = 0.0f;

                //----------------------
                // Main Game Logic
                //----------------------


                dropTimer += dt;


                for (int i = 0; i < 2; ++i)
                    playerData[i].level = (playerData[i].lines / 10);
                
                // handles everything but softDrop
                HandleInputs();

                float gravity = gravityTable[std::min(playerData[0].level, 18)];

                // deals with soft drop
                if (IsKeyDown(KEY_S) || IsKeyDown(KEY_LEFT_SHIFT))
                    gravity = std::min(gravity, softDropSpeed);
                

                if (dropTimer > gravity)
                {
                    dropTimer = 0;
                    if (CanDrop(0))
                    {
                        DropCurrentPiece(0);
                        SendData(0, playerID, Drop);
                    }
                    else
                    {
                        StickCurrentPiece(0);
                        SendData(0, playerID, Stick);

                        SpawnPiece(0);
                        SendData(0, playerID, NewPiece);
                    }
                }
                

            }
		}


        HandleEnetEvents();


		BeginDrawing();
		ClearBackground({ 50, 50, 50, 255 });
		DrawBoards();
		EndDrawing();

		
	}
	// End of game loop

	DisconnectENet(playerID);

	return EXIT_SUCCESS;
}


void StartMatch(int seed)
{
    gameOver = false;

    srand(seed);
    rng[0].seed(seed);
    rng[1].seed(seed);

    SpawnPiece(0);

    SendData(0, playerID, NewPiece);

    delay = 1.0f;

    std::cout << "Starting Match.\n";
}

void SpawnPiece(int playerIndex)
{
    int piece = pieceDist(rng[playerIndex]);
    Piece* currentPiece = &playerData[playerIndex].cPiece;
    currentPiece->type = piece;
    currentPiece->rotation = spawnData[piece].rotation;
    currentPiece->x = spawnData[piece].x;
    currentPiece->y = spawnData[piece].y;
}

bool CanDrop(int playerIndex)
{
    Piece* currentPiece = &playerData[playerIndex].cPiece;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (tetrominoes[currentPiece->type][currentPiece->rotation][row][col])
            {
                int x = (int)(currentPiece->x + col);
                int y = (int)(currentPiece->y + row + 1);

                // check vertical bounds
                if (y >= 20) 
                    return false;
                if (y < 0)
                    continue;

                int index = idx(x, y);
                if (boards[playerIndex][index] != 255)                
                    return false;          
            }
        }
    }
    return true;
}

void DropCurrentPiece(int playerIndex)
{
    playerData[playerIndex].cPiece.y += 1;
}

void StickCurrentPiece(int playerIndex)
{
    Piece* currentPiece = &playerData[playerIndex].cPiece;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (tetrominoes[currentPiece->type][currentPiece->rotation][row][col])
            {
                int x = (int)(currentPiece->x + col);
                int y = (int)(currentPiece->y + row);
                if (y >= 0) {
                    boards[playerIndex][idx(x,y)] = (int)(currentPiece->type);
                }
            }
        }
    }
}

bool CanMoveCurrentPiece(int direction, int playerIndex)
{
    Piece* currentPiece = &playerData[playerIndex].cPiece;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (tetrominoes[currentPiece->type][currentPiece->rotation][row][col])
            {
                int x = (int)(currentPiece->x + col + direction);
                int y = (int)(currentPiece->y + row);

                // check horizontal bounds
                if (x < 0)
                    return false;
                if (x >= 10)
                    return false;

                int index = idx(x, y);
                if (boards[playerIndex][index] != 255)
                    return false;
            }
        }
    }
    return true;
}

void MoveCurrentPiece(int direction, int playerIndex)
{
    playerData[playerIndex].cPiece.x += direction;
}

bool CanRotateCurrentPiece(int playerIndex)
{
    Piece* currentPiece = &playerData[playerIndex].cPiece;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (tetrominoes[currentPiece->type][(currentPiece->rotation + 1) % 4][row][col])
            {
                
                int x = (int)(currentPiece->x + col);
                int y = (int)(currentPiece->y + row);

                // check bounds
                if (x < 0 || x >= 10)
                    return false;
                if (y >= 20)
                    return false;

                //check board
                int index = idx(x, y);
                if (boards[playerIndex][index] != 255)
                    return false;
            }
        }
    }
}

void RotateCurrentPiece(int playerIndex)
{
    playerData[playerIndex].cPiece.rotation = (playerData[playerIndex].cPiece.rotation + 1) % 4;
}

void HandleInputs()
{
    //std::cout << "he\n";
    if (IsKeyPressed(KEY_D))
    {
        if (CanMoveCurrentPiece(1, 0))
        {
            MoveCurrentPiece(1, 0);
            SendData(1, playerID, Move);
        }    
    }
    if (IsKeyPressed(KEY_A))
    {
        if (CanMoveCurrentPiece(-1, 0))
        {
            MoveCurrentPiece(-1, 0);
            SendData(-1, playerID, Move);
        }
    }

    if (IsKeyPressed(KEY_W))
    {
        if (CanRotateCurrentPiece(0))
        {
            RotateCurrentPiece(0);
            SendData(0, playerID, Rotate);
        }
    }
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
        DrawTexture(tileSet[3], tileSize * (i + 1), tileSize * (20 + paddingY / tileSize), WHITE);

        // Player 2
        DrawTexture(tileSet[3], tileSize * (i + 1) + windowWidth / 2, tileSize * (20 + paddingY / tileSize), WHITE);
    }

    // draw the current board state
    for (int j = 0; j < 2; j++)
    {
        int startPixelX = tileSize + (windowWidth / 2) * j;
        int startPixelY = paddingY;
        for (int i = 0; i < 200; i++)
        {
            int pieceType = boards[j][i];
            if (pieceType == 255)
                continue;
            int color = 3;
            if (pieceType == 0 || pieceType == 1 || pieceType == 2) { color = 0; }
            else if (pieceType == 3 || pieceType == 5) { color = 1; }
            else { color = 2; }

            DrawTexture(tileSet[color],
                tileSize * (i % 10) + startPixelX,
                tileSize * floor(i / 10) + startPixelY,
                WHITE);
        }
    }

    // draw current falling pieces 
    
    for (int j = 0; j < 2; j++)
    {
        int type = playerData[j].cPiece.type;
        if (type >= 0) 
        {
            int startPixelX = tileSize + (windowWidth / 2) * j;
            int startPixelY = paddingY;

            for (int row = 0; row < 4; row++)
            {
                for (int col = 0; col < 4; col++)
                {
                    Piece* currentPiece = &playerData[j].cPiece;
                    if (tetrominoes[type][currentPiece->rotation][row][col])
                    {
                        int color = 3;
                        if (type == 0 || type == 1 || type == 2) { color = 0; }
                        else if (type == 3 || type == 5) { color = 1; }
                        else { color = 2; }

                        if (currentPiece->y + row >= 0) {
                            DrawTexture(tileSet[color],
                                (currentPiece->x + col) * tileSize + startPixelX,
                                (currentPiece->y + row) * tileSize + startPixelY,
                                RAYWHITE);
                        }
                    }
                }
            }
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

void UnPackData(PackedData pData) {


    switch (pData.type) {
    case ROOM:
        std::cout << "Joining Slot: " << pData.playerID << "\n";
        playerID = pData.playerID;
        break;

    case Start:

        StartMatch(pData.data);
        break;

    case NewPiece:

        SpawnPiece(1);
        break;

    case Drop:
        DropCurrentPiece(1);
        break;

    case Stick:

        StickCurrentPiece(1);
        break;

    case Move:

        MoveCurrentPiece(pData.data, 1);
        break;

    case Rotate:

        RotateCurrentPiece(1);
        break;

    case End:
        
        gameOver = true;
        break;

    case Clear:

        break;
    }
   
}