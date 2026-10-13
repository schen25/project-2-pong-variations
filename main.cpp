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
              SPEED         = 200,
              SPRITE_SIZE   = 500,
              PADDLE_LENGTH = 300 / 2,
              PADDLE_WIDTH = 50 / 2,
              PADDLE_DISTANCE = SCREEN_WIDTH / 2 - 30,
              PADDLE_SPEED = 150;

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN    = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                  BASE_SIZE = {static_cast<float>(SIZE), static_cast<float>(SIZE)};

constexpr char SPRITES_FP[] = "assets/sprites.png"; // drawn by me :]
constexpr char PADDLE_FP[] = "assets/paddle.png";
constexpr char BKGD_FP[] = "assets/bkgd.png";

// Global Variables
AppStatus   gAppStatus      = RUNNING;
float       gAngle          = 0.0f, 
            gPreviousTicks  = 0.0f;

Vector2     gBall1Position = ORIGIN,
            gBall1Movement = {0.0f, 0.0f},
            gBall2Position = {ORIGIN.x + 200.0f, ORIGIN.y},//ORIGIN,
            gBall2Movement = {0.0f, 0.0f},
            gBall3Position = {ORIGIN.x- 200.0f, ORIGIN.y}, //ORIGIN,
            gBall3Movement = {0.0f, 0.0f},
            gBallSize = BASE_SIZE,
            gPaddle1Position = {ORIGIN.x - PADDLE_DISTANCE, ORIGIN.y},
            gPaddle1Movement = {0.0f, 0.0f},
            gPaddle2Position = {ORIGIN.x + PADDLE_DISTANCE, ORIGIN.y},
            gPaddle2Movement = {0.0f, 0.0f},
            gPaddleSize = {PADDLE_WIDTH, PADDLE_LENGTH},
            gBkgdPosition = ORIGIN,
            gBkgdSize = {SCREEN_WIDTH, SCREEN_HEIGHT},
            gMousePosition = GetMousePosition();


std::vector<std::vector<float>> gTextureCoords;
std::vector<float> gPaddleCoords = {0.0f, 0.0f, 50, 300};
std::vector<float> gBkgdCoords = {0.0f, 0.0f, 1600, 900};
std::vector<Vector2*> gBallPositions = {
    &gBall1Position,
    &gBall2Position,
    &gBall3Position
};

Texture2D gSpritesTexture;
Texture2D gPaddleTexture;
Texture2D gBkgdTexture;

int numBalls = 3;
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
    gSpritesTexture = LoadTexture(SPRITES_FP);
    gPaddleTexture = LoadTexture(PADDLE_FP);
    gBkgdTexture = LoadTexture(BKGD_FP);

    for (int i = 0; i < 3; i++){
        gTextureCoords.push_back(std::vector<float>{0.0f, 1.0f*SPRITE_SIZE*i,
                                 1.0f*SPRITE_SIZE,
                                 1.0f*SPRITE_SIZE});
    }

    SetTargetFPS(FPS);
}


void processInput(){
    gPaddle1Movement = {0.0f, 0.0f};
    gPaddle2Movement = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W)) gPaddle1Movement.y--;
    else if (IsKeyDown(KEY_S)) gPaddle1Movement.y++;

    if (IsKeyDown(KEY_UP)) gPaddle2Movement.y--;
    else if (IsKeyDown(KEY_DOWN)) gPaddle2Movement.y++;

    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update(){
    // delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gPaddle1Position.y = gPaddle1Position.y + PADDLE_SPEED * gPaddle1Movement.y * deltaTime;
    gPaddle2Position.y = gPaddle2Position.y + PADDLE_SPEED * gPaddle2Movement.y * deltaTime;

    gMousePosition = GetMousePosition();
}

void render(){
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    // render background
    renderObject(&gBkgdTexture, gBkgdCoords, &gBkgdPosition, &gBkgdSize);

    for (int i = 0; i < numBalls; i++){ // render balls
        renderObject(&gSpritesTexture, gTextureCoords[i], gBallPositions[i], &gBallSize);
    }

    // render paddles
    renderObject(&gPaddleTexture, gPaddleCoords, &gPaddle1Position, &gPaddleSize);
    renderObject(&gPaddleTexture, gPaddleCoords, &gPaddle2Position, &gPaddleSize);


    EndDrawing();
}

void shutdown(){

    UnloadTexture(gSpritesTexture);
    UnloadTexture(gPaddleTexture);
    UnloadTexture(gBkgdTexture);

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