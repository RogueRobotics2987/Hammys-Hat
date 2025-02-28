#include <Adafruit_NeoPixel.h>

#define LED_PIN 10       // Data pin connected to WS2812 Matrix
#define MATRIX_WIDTH 8  // 8x8 matrix size
#define MATRIX_HEIGHT 8 
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int push = 0;  // Push button pin

// Joystick and game variables
int headX = 4, headY = 4; // Snake head position
int dirX = 1, dirY = 0;   // Snake direction
int tailX[64], tailY[64]; // Snake body segments
int tailLength = 0;        // Snake length
int foodX, foodY;          // Food position
bool gameOver = false;
bool gameStarted = false;  // Flag to check if game has started

int pressCount = 0;  // Count how many times the button has been pressed
bool buttonPressed = false;

// Function to map (x, y) to WS2812 strip index (zigzag pattern)
int getPixelIndex(int x, int y) {
    if (y % 2 == 0) {
        return (y * MATRIX_WIDTH) + x;  // Even rows left to right
    } else {
        return (y * MATRIX_WIDTH) + (MATRIX_WIDTH - 1 - x);  // Odd rows right to left
    }
}

void drawGame() {
    matrix.clear();
    for (int i = 0; i < tailLength; i++) {
        matrix.setPixelColor(getPixelIndex(tailX[i], tailY[i]), matrix.Color(0, 255, 0)); // Green body
    }
    matrix.setPixelColor(getPixelIndex(headX, headY), matrix.Color(0, 255, 0)); // Red head
    matrix.setPixelColor(getPixelIndex(foodX, foodY), matrix.Color(255, 0, 0)); // Yellow food
    matrix.show();
}

void drawBitmap(const uint8_t bmp[8], uint32_t color) {
    matrix.clear();
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (bmp[y] & (1 << (7 - x))) {
                matrix.setPixelColor(getPixelIndex(x, y), color);
            }
        }
    }
    matrix.show();
}

const uint8_t bmp_H[8] = {
  B11000011,
  B11000011,
  B11000011,
  B11111111,
  B11111111,
  B11000011,
  B11000011,
  B11000011
};

void moveSnake() {
    for (int i = tailLength - 1; i >= 1; i--) {
        tailX[i] = tailX[i - 1];
        tailY[i] = tailY[i - 1];
    }
    tailX[0] = headX;
    tailY[0] = headY;
    headX += dirX;
    headY += dirY;
}

bool checkCollision() {
    if (headX < 0 || headX >= MATRIX_WIDTH || headY < 0 || headY >= MATRIX_HEIGHT) {
        return true;
    }
    for (int i = 0; i < tailLength; i++) {
        if (tailX[i] == headX && tailY[i] == headY) {
            return true;
        }
    }
    return false;
}

void spawnFood() {
    foodX = random(0, MATRIX_WIDTH);
    foodY = random(0, MATRIX_HEIGHT);
}

bool checkFood() {
    if (headX == foodX && headY == foodY) {
        tailLength++;
        return true;
    }
    return false;
}

void setup() {
    pinMode(2, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(push, INPUT_PULLUP);
    Serial.begin(9600);
    matrix.begin();
    matrix.show();
    matrix.setBrightness(128);
    spawnFood();
}

void loop() {
    if (digitalRead(push) == LOW && !buttonPressed) {
        buttonPressed = true;
        pressCount = (pressCount + 1) % 4;
        delay(200);
    }
    if (digitalRead(push) == HIGH) {
        buttonPressed = false;
    }

    if (pressCount == 1) {
        drawBitmap(bmp_H, matrix.Color(0, 0, 255));
    } 
    else if (pressCount == 2) {
        drawBitmap(bmp_H, matrix.Color(255, 0, 0));
    }
    else if (pressCount >= 3) {
        gameStarted = true;
        if (digitalRead(2) == LOW && dirY == 0) { dirX = 0; dirY = -1; }
        else if (digitalRead(4) == LOW && dirX == 0) { dirX = 1; dirY = 0; }
        else if (digitalRead(6) == LOW && dirY == 0) { dirX = 0; dirY = 1; }
        else if (digitalRead(8) == LOW && dirX == 0) { dirX = -1; dirY = 0; }
        
        moveSnake();
        if (checkCollision()) {
            Serial.println("Game Over");
            gameOver = true;
        }
        if (checkFood()) {
            spawnFood();
        }
        drawGame();
        delay(500);
        if (gameOver) {
            Serial.println("Restarting game...");
            delay(2000);
            headX = 4; headY = 4;
            dirX = 1; dirY = 0;
            tailLength = 0;
            gameOver = false;
            gameStarted = false;
            spawnFood();
            pressCount = 3;
        }
    }
}
