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


enum Scene
{
    Menu,
    Game
};
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
    Piece nextPiece;
    std::vector<int> linesToClear;
    PlayerData() 
    {
        lines = 0;
        level = 0;
        score = 0;
        cPiece.type = -1; 
        nextPiece.type = -1;
    }
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

int scoreTable[] = {
    40,  // 1 lines clear
    100, // 2 lines clears
    300, // 3 lines clears
    1200 // 4 lines clears
};

unsigned char boards[2][200];

float dropTimer = 0.0f;

float delay0 = 0.0f;
float delay1 = 0.0f;

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


constexpr float AutoRepeatTime = 0.1f;
constexpr float DASDelay = 0.266f;

float DASCharge = 0.0f;
float autoRepeatTimer = 0.0f;

float rightHeld = false;
float leftHeld = false;

bool OpponentConnected = false;

int windowWidth = ((tileSize * 12) + (4 * tileSize)) * 2;
int windowHeight = (tileSize * 21) + paddingY;

Scene scene = Menu;
bool connectedToServer = false;

Rectangle exitButton = {
                   5,
                   5,
                   (tileSize - 10) * 2,
                   tileSize - 10
};

void SpawnPiece(int playerIndex);

void HandleLineClears(int playerIndex);

void ClearLines(int playerIndex);

void StickCurrentPiece(int playerIndex);

void DropCurrentPiece(int playerIndex);

bool CanDrop(int playerIndex);

bool CanMoveCurrentPiece(int direction, int playerIndex);

void MoveCurrentPiece(int direction, int playerIndex);

bool CanRotateCurrentPiece(int playerIndex);

void RotateCurrentPiece(int playerIndex);

void HandleInputs();

void StartMatch(int seed);

bool ToppedOut(int playerIndex);

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

void  DisplayOpponentState();

void PrintStats(int playerIndex);

void DisplayStats(int playerIndex);

void DrawNextPiece(int playerIndex);

void ConnectServer();

int main() {
	std::cout << "Starting Client\n";
	
    

    //create window
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);

    InitWindow(windowWidth, windowHeight, "Tetris (Client)");

    // Load Required Textures
	if (LoadTiles() != EXIT_SUCCESS)
	{
		std::cout << "Failed to load Textures\nExiting ...";
		return EXIT_FAILURE;
	}

    //clears the board
	for (int i = 0; i < 200; i++) {
		boards[0][i] = 255;
		boards[1][i] = 255;
	}

   

	while (!WindowShouldClose())
	{
        BeginDrawing();

        ClearBackground({ 50, 50, 50, 255 });
        switch (scene)
        {
        case Game:

           

            if (!gameOver)
            {
                float dt = GetFrameTime();

                // DAS Charge Logic

                if (rightHeld || leftHeld)
                {
                    DASCharge += dt;

                    if (DASCharge > DASDelay)
                    {
                        autoRepeatTimer += dt;
                    }
                }

                // Main Logic
                if (delay1 != 0) {
                    if (delay1 > 0.0001)
                    {
                        delay1 -= dt;
                    }
                    else
                    {
                        delay1 = 0.0f;

                        ClearLines(1);
                    }
                }

                if (delay0 != 0) {
                    if (delay0 > 0.0001)
                    {
                        delay0 -= dt;
                    }
                    else
                    {
                        delay0 = 0.0f;

                        ClearLines(0);
                    }
                }

                else {

                    //----------------------
                    // Main Game Logic
                    //----------------------


                    dropTimer += dt;


                    for (int i = 0; i < 2; ++i)
                        playerData[i].level = (playerData[i].lines / 10);

                    // handles everything but softDrop
                    HandleInputs();

                    // Movement From DAS
                    if (leftHeld)
                    {
                        while (autoRepeatTimer > AutoRepeatTime)
                        {
                            if (CanMoveCurrentPiece(-1, 0))
                            {
                                MoveCurrentPiece(-1, 0);
                                SendData(-1, playerID, Move);
                            }
                            autoRepeatTimer = 0;
                        }
                    }
                    if (rightHeld)
                    {
                        while (autoRepeatTimer > AutoRepeatTime)
                        {
                            if (CanMoveCurrentPiece(1, 0))
                            {
                                MoveCurrentPiece(1, 0);
                                SendData(1, playerID, Move);
                            }
                            autoRepeatTimer = 0;
                        }
                    }


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

                            HandleLineClears(0);

                            SpawnPiece(0);
                            SendData(0, playerID, NewPiece);

                            if (ToppedOut(0))
                            {
                                SendData(0, playerID, End);
                                DisconnectENet(playerID);
                                gameOver = true;
                            }
                        }
                    }


                }
            }

            HandleEnetEvents();


            // Return To Menu Button
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (CheckCollisionPointRec(GetMousePosition(), exitButton))
                {
                    DisconnectENet(playerID);
                    gameOver = true;
                    scene = Menu;
                }
            }

            DrawBoards();

            for (int i = 0; i < 2; ++i)
            {
                DisplayStats(i);
                DrawNextPiece(i);
            }

            DisplayOpponentState();

            // draws to the screen the exit button
            DrawRectangleRec(exitButton, RED);

            break;

             case Menu:

                 Rectangle startButton = {
                     GetScreenWidth() / 2 - 100,
                     GetScreenHeight() / 2 - 50,
                     200,
                     100
                 };
                
                 DrawRectangleRec(startButton, GREEN);

                 if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                 {
                     if (CheckCollisionPointRec(GetMousePosition(), startButton))
                     {
                         ConnectServer();
                     }
                 }
                 
                 break;

        }
		
       
        EndDrawing();

	}
	// End of game loop

    if (connectedToServer)
    {
        DisconnectENet(playerID);
        connectedToServer = false;
    }

	return EXIT_SUCCESS;
}


void ConnectServer()
{
    // Established connection to server with ENet
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
    scene = Game;
    connectedToServer = true;
}

void  DisplayOpponentState()
{
    std::string text;
    int x;
    if (OpponentConnected)
    {
        text = "Opponent Connected";
        x = GetScreenWidth() / 2 + 150;
    }
    else
    {
        text = "No Opponent Connected ... Waiting";
        x = GetScreenWidth() / 2 + 40;
    }
    DrawText(text.c_str(),
        x,
        10,
        30,
        RAYWHITE);
}

void DisplayStats(int playerIndex)
{
    int x = tileSize * 12 + 35 + (playerIndex * GetScreenWidth() * 0.5);
    DrawText("Next:",
        x,
        tileSize + 20,
        30,
        RAYWHITE);

    DrawText(
        TextFormat("Level:\n\n %01i", playerData[playerIndex].level),
        x,
        tileSize * 5 + 30,
        30,
        RAYWHITE);

    DrawText(
        TextFormat("Lines:\n\n %01i", playerData[playerIndex].lines),
        x,
        tileSize * 5 + 100,
        30,
        RAYWHITE);

    DrawText(
        TextFormat("Score:\n\n %01i", playerData[playerIndex].score),
        x,
        tileSize * 5 + 170,
        30,
        RAYWHITE);
}

void DrawNextPiece(int playerIndex)
{
    int type = playerData[playerIndex].nextPiece.type;
    if (type >= 0)
    {
        int startPixelX = tileSize * 12 + (playerIndex * GetScreenWidth() * 0.5);
        int startPixelY = tileSize + 60;

        for (int row = 0; row < 4; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                Piece* nextPiece = &playerData[playerIndex].nextPiece;
                
                if (tetrominoes[type][spawnData[type].rotation][row][col])
                {
                    int color = 3;
                    if (type == 0 || type == 1 || type == 2) { color = 0; }
                    else if (type == 3 || type == 5) { color = 1; }
                    else { color = 2; }

                   
                    DrawTexture(tileSet[color],
                        col * tileSize + startPixelX,
                        row * tileSize + startPixelY,
                        RAYWHITE);
                    
                }
            }
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

void PrintStats(int playerIndex)
{
    std::cout << "Player: " << playerIndex << "\n"
              << "Level: " << playerData[playerIndex].level << "\n"
              << "Lines: " << playerData[playerIndex].lines << "\n"
              << "Score: " << playerData[playerIndex].score << "\n";
}

void StartMatch(int seed)
{
    gameOver = false;

    srand(seed);
    rng[0].seed(seed);
    rng[1].seed(seed);

    playerData[0].cPiece.type = -1;
    playerData[1].cPiece.type = -1;

    SpawnPiece(0);

    SendData(0, playerID, NewPiece);

    for (int i = 0; i < 200; i++) {
        boards[0][i] = 255;
        boards[1][i] = 255;
    }

    delay0 = 1.0f;
    std::cout << "Starting Match.\n";
}

void SpawnPiece(int playerIndex)
{
    

    Piece* currentPiece = &playerData[playerIndex].cPiece;
    Piece* nextPiece = &playerData[playerIndex].nextPiece;

    // Handles the start of the game when next piece doesnt yet have a value 
    if (nextPiece->type == -1)
    {
        int piece = pieceDist(rng[playerIndex]);

        nextPiece->type = piece;
        nextPiece->rotation = spawnData[piece].rotation;
        nextPiece->x = spawnData[piece].x;
        nextPiece->y = spawnData[piece].y;
    }

    // Copies next Piece to the current Piece
    currentPiece->type = nextPiece->type;
    currentPiece->rotation = nextPiece->rotation;
    currentPiece->x = nextPiece->x;
    currentPiece->y = nextPiece->y;

    // Randomly Generates Next Piece
    int piece = pieceDist(rng[playerIndex]);

    nextPiece->type = piece;
    nextPiece->rotation = spawnData[piece].rotation;
    nextPiece->x = spawnData[piece].x;
    nextPiece->y = spawnData[piece].y;
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

void HandleLineClears(int playerIndex)
{
    std::vector<int> *linesToClear = &playerData[playerIndex].linesToClear;
    for (int y = 0; y < 20; ++y)
    {
        bool full = true;

        for (int x = 0; x < 10; ++x)
        {
            if (boards[playerIndex][idx(x, y)] == 255)
                full = false;
        }
        
        if (full)
            linesToClear->emplace_back(y);
    }

    for (int line : *linesToClear)
    {
        for (int x = 0; x < 10; ++x)
        {
            boards[playerIndex][idx(x, line)] = 255;
        }
    }
    if (playerIndex == 0)
    {
        delay0 = 0.2f;
        if (linesToClear->size() > 0)
            delay0 = 0.283;
    }
    else if (playerIndex == 1)
    {
        delay1 = 0.2f;
        if (linesToClear->size() > 0)
            delay1 = 0.283;
    }
        
    

    
}

void ClearLines(int playerIndex)
{
    std::vector<int>* linesToClear = &playerData[playerIndex].linesToClear;
    if (linesToClear->size() > 0)
    {
        // Handle stats
        playerData[playerIndex].score += scoreTable[linesToClear->size() - 1] * (playerData[playerIndex].level + 1);
        playerData[playerIndex].lines += linesToClear->size();


        // Handle shifting the board for the line clear 
        for (int line : *linesToClear)
        {
            memcpy(&boards[playerIndex][10], &boards[playerIndex][0], line * 10);
        }
        linesToClear->clear();
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
                if (y < 0)
                    continue;

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
   

    //DAS Requirements
    if (IsKeyDown(KEY_A))
        leftHeld = true;
    else
        leftHeld = false;

    if (IsKeyDown(KEY_D))
        rightHeld = true;
    else
        rightHeld = false;


    // Other Inputs
    if (IsKeyPressed(KEY_D))
    {
        if (CanMoveCurrentPiece(1, 0))
        {
            MoveCurrentPiece(1, 0);
            SendData(1, playerID, Move);
        }    
        DASCharge = 0.0f;
        autoRepeatTimer = 0.0f;

        rightHeld = true;
    }
    if (IsKeyPressed(KEY_A))
    {
        if (CanMoveCurrentPiece(-1, 0))
        {
            MoveCurrentPiece(-1, 0);
            SendData(-1, playerID, Move);
        }
        DASCharge = 0.0f;
        autoRepeatTimer = 0.0f;

        leftHeld = true;
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

bool ToppedOut(int playerIndex)
{
    Piece* currentPiece = &playerData[playerIndex].cPiece;
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            if (tetrominoes[currentPiece->type][currentPiece->rotation][row][col])
            {
                int x = currentPiece->x + col;
                int y = currentPiece->y + row;

                if (!((y >= 0 && y < 20) && (x >= 0 && x < 10)))
                    continue;

                if (boards[playerIndex][idx(x, y)] != 255)
                {
                    return true;
                }
            }
        }
    }
    return false;
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
        HandleLineClears(1);
        break;

    case Move:

        MoveCurrentPiece(pData.data, 1);
        break;

    case Rotate:

        RotateCurrentPiece(1);
        break;

    case End:
        gameOver = true;
        DisconnectENet(playerID);
        connectedToServer = false;
        break;

    case OpponentConnect:

        if(pData.data == 1)
            OpponentConnected = true;
        else 
            OpponentConnected = false;
        break;
    }
}
