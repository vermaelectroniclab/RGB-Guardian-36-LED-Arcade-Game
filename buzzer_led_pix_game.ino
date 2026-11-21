#include <Adafruit_NeoPixel.h>

// =================================
// RGB GUARDIAN - 36 LED ARCADE
// ORIGINAL BUZZER SOUND SYSTEM
// =================================

#define LED_PIN 6
#define NUM_LEDS 36

#define RED_BUTTON 2
#define GREEN_BUTTON 3
#define BLUE_BUTTON 4
#define BUZZER_PIN 5

#define MAX_ENEMIES 5
#define MAX_BULLETS 3

#define BRIGHTNESS 55
#define WIN_SCORE 500

#define ENEMY_LENGTH 4
#define BULLET_LENGTH 4

Adafruit_NeoPixel strip(
  NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800
);

// COLORS
#define RED 0
#define GREEN 1
#define BLUE 2

uint32_t colors[3];

const byte buttons[3] = {
  RED_BUTTON, GREEN_BUTTON, BLUE_BUTTON
};

// FIRST RED, GREEN, BLUE, THEN RANDOM
byte nextEnemyColor = 0;

// =================================
// ORIGINAL BUZZER SOUND SYSTEM
// =================================

struct SoundStep {
  byte state;
  unsigned int duration;
};

// 1. LASER SHOOT
const SoundStep laserSound[] = {
  {1, 35}, {0, 20},
  {1, 25}, {0, 15},
  {1, 45}, {0, 20},
  {1, 25}, {0, 20}
};

// 2. ENEMY DESTROY
const SoundStep hitSound[] = {
  {1, 100}, {0, 35},
  {1, 45}, {0, 25},
  {1, 120}, {0, 20}
};

// 3. WRONG COLOUR
const SoundStep wrongSound[] = {
  {1, 180}, {0, 70},
  {1, 180}, {0, 70},
  {1, 250}, {0, 30}
};

// 4. LIFE LOST
const SoundStep lifeSound[] = {
  {1, 250}, {0, 100},
  {1, 150}, {0, 100},
  {1, 350}, {0, 30}
};

// 5. GAME OVER
const SoundStep gameOverSound[] = {
  {1, 100}, {0, 80},
  {1, 150}, {0, 100},
  {1, 250}, {0, 120},
  {1, 400}, {0, 50}
};

// 6. LEVEL UP
const SoundStep levelSound[] = {
  {1, 50}, {0, 30},
  {1, 50}, {0, 30},
  {1, 50}, {0, 30},
  {1, 100}, {0, 25},
  {1, 180}, {0, 30}
};

// 7. VICTORY
const SoundStep winSound[] = {
  {1, 100}, {0, 50},
  {1, 100}, {0, 50},
  {1, 100}, {0, 50},
  {1, 180}, {0, 70},
  {1, 180}, {0, 70},
  {1, 300}, {0, 80},
  {1, 500}, {0, 50}
};

const SoundStep* currentSound = NULL;

byte soundIndex = 0;
byte soundLength = 0;

bool soundActive = false;
unsigned long soundNext = 0;

// =================================
// PLAY SOUND
// =================================

void playSound(byte type) {

  noTone(BUZZER_PIN);

  switch (type) {

    case 1:
      currentSound = laserSound;
      soundLength = sizeof(laserSound) / sizeof(SoundStep);
      break;

    case 2:
      currentSound = hitSound;
      soundLength = sizeof(hitSound) / sizeof(SoundStep);
      break;

    case 3:
      currentSound = wrongSound;
      soundLength = sizeof(wrongSound) / sizeof(SoundStep);
      break;

    case 4:
      currentSound = lifeSound;
      soundLength = sizeof(lifeSound) / sizeof(SoundStep);
      break;

    case 5:
      currentSound = gameOverSound;
      soundLength = sizeof(gameOverSound) / sizeof(SoundStep);
      break;

    case 6:
      currentSound = levelSound;
      soundLength = sizeof(levelSound) / sizeof(SoundStep);
      break;

    case 7:
      currentSound = winSound;
      soundLength = sizeof(winSound) / sizeof(SoundStep);
      break;

    default:
      return;
  }

  soundIndex = 0;
  soundNext = 0;
  soundActive = true;
}

// =================================
// UPDATE SOUND
// =================================

void updateSound() {

  if (!soundActive) return;

  unsigned long now = millis();

  if (now < soundNext) return;

  if (soundIndex >= soundLength) {

    digitalWrite(BUZZER_PIN, LOW);

    soundActive = false;
    return;
  }

  digitalWrite(
    BUZZER_PIN,
    currentSound[soundIndex].state ? HIGH : LOW
  );

  soundNext = now + currentSound[soundIndex].duration;

  soundIndex++;
}

// =================================
// BUTTON DEBOUNCE
// =================================

bool lastReading[3] = {HIGH, HIGH, HIGH};
bool buttonState[3] = {HIGH, HIGH, HIGH};

unsigned long debounceTimer[3] = {0, 0, 0};

bool readButton(byte index) {

  bool reading = digitalRead(buttons[index]);

  if (reading != lastReading[index]) {

    lastReading[index] = reading;
    debounceTimer[index] = millis();
  }

  if (millis() - debounceTimer[index] > 25) {

    if (reading != buttonState[index]) {

      buttonState[index] = reading;

      if (buttonState[index] == LOW) {
        return true;
      }
    }
  }

  return false;
}

// =================================
// ENEMY
// =================================

struct Enemy {
  int pos;
  byte color;
  bool active;
};

Enemy enemies[MAX_ENEMIES];

// =================================
// BULLET
// =================================

struct Bullet {
  int pos;
  byte color;
  bool active;
};

Bullet bullets[MAX_BULLETS];

// =================================
// GAME VARIABLES
// =================================

int score = 0;
int lives = 3;
int level = 1;
int combo = 0;

int enemySpeed = 150;
int bulletSpeed = 35;
int spawnSpeed = 850;

unsigned long lastEnemyMove = 0;
unsigned long lastBulletMove = 0;
unsigned long lastSpawn = 0;
unsigned long lastShot = 0;

const int FIRE_POSITION = 4;

// =================================
// GAME STATES
// =================================

enum GameState {
  START_SCREEN,
  PLAYING,
  GAME_OVER,
  WIN
};

GameState gameState = START_SCREEN;

// =================================
// RESET GAME
// =================================

void resetGame() {

  score = 0;
  lives = 3;
  level = 1;
  combo = 0;

  nextEnemyColor = 0;

  enemySpeed = 150;
  bulletSpeed = 35;
  spawnSpeed = 850;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].active = false;
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].active = false;
  }

  lastEnemyMove = millis();
  lastBulletMove = millis();
  lastSpawn = millis();
  lastShot = 0;

  soundActive = false;
  digitalWrite(BUZZER_PIN, LOW);

  strip.clear();
  strip.show();

  gameState = PLAYING;
}

// =================================
// START ANIMATION
// =================================

void startAnimation() {

  strip.clear();

  for (int i = NUM_LEDS - 1; i >= 0; i--) {

    strip.setPixelColor(i, colors[BLUE]);
    strip.show();

    digitalWrite(BUZZER_PIN, HIGH);
    delay(8);
    digitalWrite(BUZZER_PIN, LOW);
  }

  digitalWrite(BUZZER_PIN, LOW);

  strip.clear();
  strip.show();
}

// =================================
// START SCREEN
// GAME WILL WAIT FOR BUTTON
// =================================

void showStartScreen() {

  static unsigned long flashTimer = 0;
  static bool flashState = false;

  if (millis() - flashTimer >= 350) {

    flashTimer = millis();
    flashState = !flashState;

    if (flashState) {
      strip.fill(strip.Color(0, 0, 255));
    } else {
      strip.clear();
    }

    strip.show();
  }

  // ANY BUTTON = START GAME

  for (int i = 0; i < 3; i++) {

    if (readButton(i)) {

      flashTimer = 0;
      flashState = false;

      resetGame();
      return;
    }
  }
}

// =================================
// SPAWN ENEMY
// =================================

void spawnEnemy() {

  for (int i = 0; i < MAX_ENEMIES; i++) {

    if (!enemies[i].active) {

      enemies[i].active = true;
      enemies[i].pos = NUM_LEDS - 1;

      // RED -> GREEN -> BLUE -> RANDOM

      if (nextEnemyColor < 3) {

        enemies[i].color = nextEnemyColor;
        nextEnemyColor++;

      } else {

        enemies[i].color = random(0, 3);
      }

      return;
    }
  }
}

// =================================
// SHOOT BULLET
// =================================

void shootBullet(byte color) {

  if (millis() - lastShot < 120) return;

  for (int i = 0; i < MAX_BULLETS; i++) {

    if (!bullets[i].active) {

      bullets[i].active = true;
      bullets[i].pos = FIRE_POSITION;
      bullets[i].color = color;

      lastShot = millis();

      playSound(1);

      return;
    }
  }
}

// =================================
// LEVEL SYSTEM
// =================================

void updateLevel() {

  int newLevel = 1 + score / 100;

  if (newLevel > 5) newLevel = 5;

  if (newLevel != level) {

    level = newLevel;

    enemySpeed = 150 - (level - 1) * 23;
    spawnSpeed = 850 - (level - 1) * 130;

    if (enemySpeed < 45) enemySpeed = 45;
    if (spawnSpeed < 250) spawnSpeed = 250;

    playSound(6);
  }
}

// =================================
// LIFE LOST
// =================================

void loseLife() {

  lives--;
  combo = 0;

  if (lives <= 0) {

    gameState = GAME_OVER;
    playSound(5);

  } else {

    playSound(4);
  }
}

// =================================
// ENEMY MOVEMENT
// =================================

void moveEnemies() {

  for (int i = 0; i < MAX_ENEMIES; i++) {

    if (enemies[i].active) {

      enemies[i].pos--;

      if (enemies[i].pos <= FIRE_POSITION) {

        enemies[i].active = false;

        loseLife();

        if (gameState == GAME_OVER) return;
      }
    }
  }
}

// =================================
// BULLET MOVEMENT
// =================================

void moveBullets() {

  for (int i = 0; i < MAX_BULLETS; i++) {

    if (bullets[i].active) {

      bullets[i].pos++;

      if (bullets[i].pos >= NUM_LEDS) {
        bullets[i].active = false;
      }
    }
  }
}

// =================================
// COLLISION DETECTION
// =================================

void checkCollisions() {

  for (int b = 0; b < MAX_BULLETS; b++) {

    if (!bullets[b].active) continue;

    int nearestEnemy = -1;
    int nearestDistance = 999;

    for (int e = 0; e < MAX_ENEMIES; e++) {

      if (!enemies[e].active) continue;

      int distance = abs(
        bullets[b].pos - enemies[e].pos
      );

      if (distance <= 1 && distance < nearestDistance) {

        nearestDistance = distance;
        nearestEnemy = e;
      }
    }

    if (nearestEnemy >= 0) {

      if (bullets[b].color == enemies[nearestEnemy].color) {

        enemies[nearestEnemy].active = false;

        combo++;

        int multiplier = 1 + combo / 5;

        score += 10 * multiplier;

        bullets[b].active = false;

        // WIN AT 500 SCORE

        if (score >= WIN_SCORE) {

          score = WIN_SCORE;

          gameState = WIN;

          playSound(7);

          return;
        }

        playSound(2);

        updateLevel();

      } else {

        bullets[b].active = false;

        playSound(3);

        loseLife();
      }

      if (gameState != PLAYING) return;
    }
  }
}

// =================================
// BUTTON CONTROL
// =================================

void checkButtons() {

  for (int i = 0; i < 3; i++) {

    if (readButton(i)) {
      shootBullet(i);
    }
  }
}

// =================================
// DRAW GAME
// =================================

void drawGame() {

  strip.clear();

  // ENEMY CONTINUOUS TRAIL

  for (int i = 0; i < MAX_ENEMIES; i++) {

    if (enemies[i].active) {

      int p = enemies[i].pos;

      for (int j = 0; j < ENEMY_LENGTH; j++) {

        int led = p + j;

        if (led >= 0 && led < NUM_LEDS) {

          strip.setPixelColor(
            led,
            colors[enemies[i].color]
          );
        }
      }
    }
  }

  // BULLET CONTINUOUS TRAIL

  for (int i = 0; i < MAX_BULLETS; i++) {

    if (bullets[i].active) {

      int p = bullets[i].pos;

      for (int j = 0; j < BULLET_LENGTH; j++) {

        int led = p + j;

        if (led >= 0 && led < NUM_LEDS) {

          strip.setPixelColor(
            led,
            strip.Color(255, 255, 255)
          );
        }
      }
    }
  }

  // SHOOTER ZONE

  strip.setPixelColor(
    0, strip.Color(40, 40, 40)
  );

  // LIFE INDICATOR

  for (int i = 0; i < 3; i++) {

    strip.setPixelColor(
      1 + i,
      i < lives ? strip.Color(0, 255, 0)
                : strip.Color(25, 0, 0)
    );
  }

  strip.show();
}

// =================================
// WIN ANIMATION
// =================================

void winAnimation() {

  static unsigned long flashTimer = 0;
  static bool flashState = false;

  if (millis() - flashTimer >= 300) {

    flashTimer = millis();

    flashState = !flashState;

    if (flashState) {

      strip.fill(strip.Color(0, 255, 0));

    } else {

      strip.clear();
    }

    strip.show();
  }

  // ANY BUTTON = RESTART

  for (int i = 0; i < 3; i++) {

    if (readButton(i)) {

      flashTimer = 0;
      flashState = false;

      resetGame();
      return;
    }
  }
}

// =================================
// GAME OVER ANIMATION
// =================================

void gameOverAnimation() {

  static unsigned long flashTimer = 0;
  static bool flashState = false;

  if (millis() - flashTimer >= 250) {

    flashTimer = millis();

    flashState = !flashState;

    if (flashState) {

      strip.fill(strip.Color(255, 0, 0));

    } else {

      strip.clear();
    }

    strip.show();
  }

  // ANY BUTTON = RESTART

  for (int i = 0; i < 3; i++) {

    if (readButton(i)) {

      flashTimer = 0;
      flashState = false;

      resetGame();
      return;
    }
  }
}

// =================================
// SETUP
// =================================

void setup() {

  strip.begin();

  strip.setBrightness(BRIGHTNESS);

  strip.clear();
  strip.show();

  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  colors[RED] = strip.Color(255, 0, 0);
  colors[GREEN] = strip.Color(0, 255, 0);
  colors[BLUE] = strip.Color(0, 0, 255);

  randomSeed(analogRead(A0));

  startAnimation();

  // IMPORTANT:
  // DO NOT START GAME AUTOMATICALLY

  gameState = START_SCREEN;
}

// =================================
// MAIN LOOP
// =================================

void loop() {

  updateSound();

  // START SCREEN
  // WAIT UNTIL ANY BUTTON IS PRESSED

  if (gameState == START_SCREEN) {

    showStartScreen();
    return;
  }

  // WIN STATE

  if (gameState == WIN) {

    winAnimation();
    return;
  }

  // GAME OVER STATE

  if (gameState == GAME_OVER) {

    gameOverAnimation();
    return;
  }

  if (gameState != PLAYING) return;

  unsigned long now = millis();

  // BUTTONS

  checkButtons();

  if (gameState != PLAYING) return;

  // ENEMY SPAWN

  if (now - lastSpawn >= spawnSpeed) {

    lastSpawn = now;

    spawnEnemy();
  }

  // ENEMY MOVEMENT

  if (now - lastEnemyMove >= enemySpeed) {

    lastEnemyMove = now;

    moveEnemies();
  }

  if (gameState != PLAYING) return;

  // BULLET MOVEMENT

  if (now - lastBulletMove >= bulletSpeed) {

    lastBulletMove = now;

    moveBullets();
  }

  // COLLISION

  checkCollisions();

  if (gameState != PLAYING) return;

  // DISPLAY

  drawGame();
}
