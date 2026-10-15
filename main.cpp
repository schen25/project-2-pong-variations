/**

* Author: Stacey Chen

* Assignment: Pong Clone

* Date due: 10/16/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

#include "CS3113/cs3113.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS           = 60,
              SIZE          = 100 / 2,
              SPEED         = 250,
              SPRITE_SIZE   = 500,
              PADDLE_LENGTH = 300 / 2,
              PADDLE_WIDTH = 50 / 2,
              PADDLE_DISTANCE = SCREEN_WIDTH / 2 - 30,
              PADDLE_SPEED = 175,
              BORDER_THICKNESS = 20,
              SCORE_NUMS_SIZE = 40, 
              SCORE_SIDE_DISTANCE = SCREEN_WIDTH / 2 - 40,
              SCORE_TOP_DISTANCE = 40,
              MOVE_BUFFER = 5;

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN    = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                  BASE_SIZE = {static_cast<float>(SIZE), static_cast<float>(SIZE)};

constexpr char SPRITES_FP[] = "assets/sprites.png"; // drawn by me :]
constexpr char PADDLE_FP[] = "assets/paddle.png";
constexpr char BKGD_FP[] = "assets/bkgd.png";
constexpr char WIN_BKGD_FP[] = "assets/winBkgd.png";
constexpr char ONE_FP[] = "assets/1.png";
constexpr char TWO_FP[] = "assets/2.png";
constexpr char SCORE_NUMS_FP[] = "assets/scoreNumbers.png";
constexpr char START_SCREEN_FP[] = "assets/startScreen.png";
constexpr char PLAY_FP[] = "assets/playButtons.png";

// Global Variables
AppStatus   gAppStatus      = RUNNING;
float       gAngle          = 0.0f, 
            gPreviousTicks  = 0.0f;

Vector2     gBall1Position = ORIGIN,
            gBall1Movement = {0.0f, 0.0f},
            gBall2Position = ORIGIN,
            gBall2Movement = {0.0f, 0.0f},
            gBall3Position = ORIGIN,
            gBall3Movement = {0.0f, 0.0f},
            gBallSize = BASE_SIZE,
            gPaddle1Position = {ORIGIN.x - PADDLE_DISTANCE, ORIGIN.y},
            gPaddle1Movement = {0.0f, 0.0f},
            gPaddle2Position = {ORIGIN.x + PADDLE_DISTANCE, ORIGIN.y},
            gPaddle2Movement = {0.0f, 0.0f},
            gPaddleSize = {PADDLE_WIDTH, PADDLE_LENGTH},
            gBkgdPosition = ORIGIN,
            gBkgdSize = {SCREEN_WIDTH, SCREEN_HEIGHT},
            gScore1Position = {ORIGIN.x - SCORE_SIDE_DISTANCE, SCORE_TOP_DISTANCE},
            gScore2Position = {ORIGIN.x + SCORE_SIDE_DISTANCE, SCORE_TOP_DISTANCE},
            gScoreNumsSize = {SCORE_NUMS_SIZE, SCORE_NUMS_SIZE},
            gPlayPosition = {ORIGIN.x, ORIGIN.y + 30},
            gPlaySize = {200.0f, 75.0f},
            gMousePosition = GetMousePosition();


std::vector<std::vector<float>> gTextureCoords;
std::vector<float> gPaddleCoords = {0.0f, 0.0f, 50, 300};
std::vector<float> gBkgdCoords = {0.0f, 0.0f, 1600, 900};
std::vector<float> gScore1Coords = {0.0f, 0.0f, 100, 100};
std::vector<float> gScore2Coords = {0.0f, 0.0f, 100, 100};
std::vector<float> gPlayCoords = {0.0f, 0.0f, 400, 150};
std::vector<Vector2*> gBallPositions = {
    &gBall1Position,
    &gBall2Position,
    &gBall3Position
};
std::vector<Vector2*> gBallMovements = {
    &gBall1Movement,
    &gBall2Movement,
    &gBall3Movement
};

Texture2D gSpritesTexture;
Texture2D gPaddleTexture;
Texture2D gBkgdTexture;
Texture2D gWinBkgdTexture;
Texture2D gOneTexture;
Texture2D gTwoTexture;
Texture2D gScoreNumsTexture;
Texture2D gStartScreenTexture;
Texture2D gPlayTexture;

int gNumBalls = 1;
int gPlayer1Score = 0;
int gPlayer2Score = 0;
int gScreen = 0;
bool gIsTwoPlayer = true;
unsigned int gStartTime;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 *
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}


// takes int i, moves ball[i] to the origin and gives it a random movement vector
void resetBall(int i){
    gBallPositions[i] -> x = ORIGIN.x;
    gBallPositions[i] -> y = ORIGIN.y;

    gBallMovements[i] -> x = std::rand();
    gBallMovements[i] -> y = std::rand();

    while (gBallMovements[i] -> x < gBallMovements[i] -> y) {
        // so it doesn't travel too vertically
        gBallMovements[i] -> x = std::rand();
        gBallMovements[i] -> y = std::rand();
    }

    if (std::rand() % 2) gBallMovements[i] -> x *= -1;
    if (std::rand() %2) gBallMovements[i] -> y *= -1;

    Normalise(gBallMovements[i]);
}

// takes two position vectors, returns distance between the 2 points
float getDistance(const Vector2 *positionA, const Vector2 *positionB){
    return sqrtf(
        pow((positionA -> x - positionB -> x), 2) +
        pow((positionA -> y - positionB -> y), 2) 
    );
}

void renderObject(const Texture2D *texture, 
                  const std::vector<float>& textureCoords, const Vector2 *position,
                  const Vector2 *scale) // note textureCoords is not passed by ptr
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        textureCoords[0], textureCoords[1],

        // bottom-right corner (of texture)
        textureCoords[2], textureCoords[3]
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Variations");

    gStartTime = time(nullptr);
    std::srand(gStartTime);
    gSpritesTexture = LoadTexture(SPRITES_FP);
    gPaddleTexture = LoadTexture(PADDLE_FP);
    gBkgdTexture = LoadTexture(BKGD_FP);
    gWinBkgdTexture = LoadTexture(WIN_BKGD_FP);
    gOneTexture = LoadTexture(ONE_FP);
    gTwoTexture = LoadTexture(TWO_FP);
    gScoreNumsTexture = LoadTexture(SCORE_NUMS_FP);
    gStartScreenTexture = LoadTexture(START_SCREEN_FP);
    gPlayTexture = LoadTexture(PLAY_FP);

    for (int i = 0; i < 3; i++){
        gTextureCoords.push_back(std::vector<float>{0.0f, 1.0f*SPRITE_SIZE*i,
                                 1.0f*SPRITE_SIZE,
                                 1.0f*SPRITE_SIZE});

        resetBall(i);
        // gBallMovements[i] -> x = std::rand();
        // gBallMovements[i] -> y = std::rand();

        // if (std::rand() % 2) gBallMovements[i] -> x *= -1;
        // if (std::rand() % 2) gBallMovements[i] -> y *= -1;

        // Normalise(gBallMovements[i]);
    }

    SetTargetFPS(FPS);
}


void processInput(){
    if (gScreen == 1){
        gPaddle1Movement = {0.0f, 0.0f};
        gPaddle2Movement = {0.0f, 0.0f};

        if (IsKeyPressed(KEY_T)) gIsTwoPlayer = !gIsTwoPlayer;

        if (IsKeyDown(KEY_W)) gPaddle1Movement.y--;
        else if (IsKeyDown(KEY_S)) gPaddle1Movement.y++;

        if (gIsTwoPlayer){
            if (IsKeyDown(KEY_UP)) gPaddle2Movement.y--;
            else if (IsKeyDown(KEY_DOWN)) gPaddle2Movement.y++;
        }
        else {
            // automated motion
            float minXDistance = SCREEN_WIDTH;
            Vector2* closestBall = nullptr;
            for (int i = 0; i < gNumBalls; i++){
                if (gBallPositions[i] -> x < gPaddle2Position.x &&
                    gBallMovements[i] -> x > 0){
                    // ball is a candidate to follow if it is moving toward you
                    if (gPaddle2Position.x - gBallPositions[i] -> x < minXDistance){
                        minXDistance = gPaddle2Position.x - gBallPositions[i] -> x;
                        closestBall = gBallPositions[i];
                    }
                }
            }
            if (closestBall){
                if (gPaddle2Position.y > closestBall -> y + MOVE_BUFFER) gPaddle2Movement.y--;
                else if (gPaddle2Position.y < closestBall -> y - MOVE_BUFFER) gPaddle2Movement.y++;
            }
        }
        
        if (IsKeyPressed(KEY_ONE)){
            gNumBalls = 1;
            // reset balls 2, 3 (1 and 2 are the indices)
            resetBall(1);
            resetBall(2);

        }
        else if (IsKeyPressed(KEY_TWO)){
            gNumBalls = 2;
            resetBall(2);
        }
        else if (IsKeyPressed(KEY_THREE)){
            gNumBalls = 3;
        }
    }

    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update(){
    if (gScreen == 0){
        gMousePosition = GetMousePosition();
        if (gMousePosition.x >= gPlayPosition.x - gPlaySize.x / 2.0f &&
            gMousePosition.x <= gPlayPosition.x + gPlaySize.x / 2.0f &&
            gMousePosition.y >= gPlayPosition.y - gPlaySize.y / 2.0f &&
            gMousePosition.y <= gPlayPosition.y + gPlaySize.y / 2.0f){
                gPlayCoords[1] = 150;
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                    gScreen = 1;
                }
        }
        else{
            gPlayCoords[1] = 0;
        }
    }
    if (gScreen == 1){
        // delta time
        float ticks = static_cast<float>(GetTime());
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks = ticks;

        // update paddles
        float paddle1NewY = gPaddle1Position.y + PADDLE_SPEED * gPaddle1Movement.y * deltaTime;
        float paddle2NewY = gPaddle2Position.y + PADDLE_SPEED * gPaddle2Movement.y * deltaTime;
        if (paddle1NewY - PADDLE_LENGTH/2 > BORDER_THICKNESS &&
            paddle1NewY + PADDLE_LENGTH/2 < SCREEN_HEIGHT-BORDER_THICKNESS){
            gPaddle1Position.y = paddle1NewY;
        }
        if (paddle2NewY - PADDLE_LENGTH/2 > BORDER_THICKNESS &&
            paddle2NewY + PADDLE_LENGTH/2 < SCREEN_HEIGHT-BORDER_THICKNESS){
            gPaddle2Position.y = paddle2NewY;
        }

        // update balls
        for (int i = 0; i < gNumBalls; i++){ 
            gBallPositions[i] -> x += gBallMovements[i] -> x * SPEED * deltaTime;
            gBallPositions[i] -> y += gBallMovements[i] -> y * SPEED * deltaTime;

            if (isColliding(gBallPositions[i], &gBallSize, &gPaddle1Position, &gPaddleSize) &&
                (gBallPositions[i] -> x >= gPaddle1Position.x + SIZE/4)){
                gBallMovements[i] -> x *= -1;

                while (isColliding(gBallPositions[i], &gBallSize, &gPaddle1Position, &gPaddleSize)){
                        gBallPositions[i] -> x += gBallMovements[i] -> x * SPEED * deltaTime*1.0; // prevent trapping
                }
            }
            else if (isColliding(gBallPositions[i], &gBallSize, &gPaddle2Position, &gPaddleSize) &&
                (gBallPositions[i] -> x <= gPaddle2Position.x - SIZE/4)){
                    gBallMovements[i] -> x *= -1;

                    while (isColliding(gBallPositions[i], &gBallSize, &gPaddle2Position, &gPaddleSize)){
                            gBallPositions[i] -> x += gBallMovements[i] -> x * SPEED * deltaTime*1.0; // prevent trapping
                    }

            }
            if (gBallPositions[i] -> y - SIZE/2 <= BORDER_THICKNESS || 
                gBallPositions[i] -> y + SIZE/2 >= SCREEN_HEIGHT-BORDER_THICKNESS){
                    gBallMovements[i] -> y *= -1;
                    gBallPositions[i] -> y += gBallMovements[i] -> y * SPEED * deltaTime *5;
            }

            if (gBallPositions[i] -> x < -1*SIZE){ 
                gPlayer2Score++;
                resetBall(i);
            }
            if (gBallPositions[i] -> x > SCREEN_WIDTH + SIZE){
                gPlayer1Score++;
                resetBall(i);
            }
        }

        // update scoreboard
        gScore1Coords[0] = std::min(500, gPlayer1Score*100);
        gScore2Coords[0] = std::min(500, gPlayer2Score*100);


        if (gPlayer1Score >= 5 || gPlayer2Score >= 5) {
            gScreen = 2;
        }
    }

}

void render(){
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    if (gScreen == 0){
        renderObject(&gStartScreenTexture, gBkgdCoords, &gBkgdPosition, &gBkgdSize);
        renderObject(&gPlayTexture, gPlayCoords, &gPlayPosition, &gPlaySize);
    }
    else if (gScreen == 1){ // game runs
        // render background
        renderObject(&gBkgdTexture, gBkgdCoords, &gBkgdPosition, &gBkgdSize);

        for (int i = 0; i < gNumBalls; i++){ // render balls
            renderObject(&gSpritesTexture, gTextureCoords[i], gBallPositions[i], &gBallSize);
        }

        // render paddles
        renderObject(&gPaddleTexture, gPaddleCoords, &gPaddle1Position, &gPaddleSize);
        renderObject(&gPaddleTexture, gPaddleCoords, &gPaddle2Position, &gPaddleSize);

        // render score board
        renderObject(&gScoreNumsTexture, gScore1Coords, &gScore1Position, &gScoreNumsSize);
        renderObject(&gScoreNumsTexture, gScore2Coords, &gScore2Position, &gScoreNumsSize);
    }
    if (gScreen == 2){ // win/lose screen
        // render background
        renderObject(&gWinBkgdTexture, gBkgdCoords, &gBkgdPosition, &gBkgdSize);

        // render winner
        if (gPlayer1Score > gPlayer2Score){
            renderObject(&gOneTexture, gBkgdCoords, &gBkgdPosition, &gBkgdSize);
        }
        else {
            renderObject(&gTwoTexture, gBkgdCoords, &gBkgdPosition, &gBkgdSize);
        }
    }
    

    EndDrawing();
}

void shutdown(){

    UnloadTexture(gSpritesTexture);
    UnloadTexture(gPaddleTexture);
    UnloadTexture(gBkgdTexture);
    UnloadTexture(gWinBkgdTexture);
    UnloadTexture(gOneTexture);
    UnloadTexture(gTwoTexture);
    UnloadTexture(gScoreNumsTexture);
    UnloadTexture(gStartScreenTexture);
    UnloadTexture(gPlayTexture);

    CloseWindow();
}

int main(void){
    initialise();

    while (gAppStatus == RUNNING){
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}