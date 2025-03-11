#include <FastLED.h>
#include <LedControl.h>

#define LED_PIN     13     // Data Pin (usually referred to as DI or Data In)
#define CLK_PIN     12     // Clock Pin (usually referred to as SCK or Clock)
#define CS_PIN      11     // Chip Select Pin (used for selecting the matrix)

#define WIDTH       32
#define HEIGHT      8
#define NUM_LEDS    (WIDTH * HEIGHT)
#define BRIGHTNESS  100
#define SCROLL_SPEED 100  // Scrolling speed in milliseconds

CRGB leds[NUM_LEDS];

int remapIndex(int x, int y) {
    if (x % 2 == 0) {
        return x * HEIGHT + y;
    } else {
        return x * HEIGHT + (HEIGHT - 1 - y);
    }
}
const uint64_t font5x7[][1] = {
    {0x0000000000000000}, // Space
    {0x0000000000000017}, // !
    {0x0000000000007E09}, // R
    {0x0000000000003E41}, // O
    {0x0000000000003E49}, // G
    {0x000000000000407E}, // U
    {0x0000000000007E09}, // E
    {0x0000000000003E51}, // 2
    {0x0000000000007E49}, // 9
    {0x0000000000003E41}, // 8
    {0x0000000000003E40}  // 7
};

void drawCandyCanePattern() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if ((x + y) % 2 == 0) {
                leds[remapIndex(x, y)] = CRGB::Red;  // Red stripe
            } else {
                leds[remapIndex(x, y)] = CRGB::White; // White stripe
            }
        }
    }
}

void drawText(int offset) {
    const char text[] = "ROGUE 2987";
    for (int i = 0; text[i] != '\0'; i++) {
        int x = (i * 6) - offset; // Each character is about 6 pixels wide
        if (x >= -6 && x < WIDTH) {
            for (int col = 0; col < 5; col++) {
                for (int row = 0; row < 7; row++) {
                    if (font5x7[text[i] - 32][col] & (1 << row)) {
                        leds[remapIndex(x + col, row)] = CRGB::White;
                    }
                }
            }
        }
    }
}

void setup() {
  pinMode(12, INPUT);  // or pinMode(12, INPUT_PULLUP);
    FastLED.addLeds<WS2812B, DIN_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    // Initialize the LEDs immediately by drawing the candy cane pattern first.
    drawCandyCanePattern();
    FastLED.show(); // Display the initial pattern
}

void loop() {

      int inputState = digitalRead(DIN_PIN);


    static int offset = 0;

    drawCandyCanePattern();
    drawText(offset);

    FastLED.show();  // Display updated LEDs

    offset++;
    if (offset > (WIDTH + 6 * strlen("ROGUE 2987"))) {
        offset = 0; // Reset when the text finishes scrolling
    }

    delay(SCROLL_SPEED);
}
