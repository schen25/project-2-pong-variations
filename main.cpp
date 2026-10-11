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
              SIZE          = 500 / 2,
              SPEED         = 200;

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN    = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                  BASE_SIZE = {static_cast<float>(SIZE), static_cast<float>(SIZE)};

AppStatus   gAppStatus      = RUNNING;
float       gAngle          = 0.0f, 
            gPreviousTicks  = 0.0f;

Vector2     gMousePosition = GetMousePosition();

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

void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
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

    SetTargetFPS(FPS);
}


void processInput(){
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update(){
    // delta time
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gMousePosition = GetMousePosition();
}

void render(){
    BeginDrawing();
    
    ClearBackground(ColorFromHex(BG_COLOUR));

    EndDrawing();
}

void shutdown(){

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