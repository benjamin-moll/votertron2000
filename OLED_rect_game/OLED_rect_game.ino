#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int myRectY = 30;
int myRectX = 0;
int myRectW = 10;
int myRectH = 10;



int obs1_x = SCREEN_WIDTH - 10;
int obs1_y = 0;
int obs1_height = 0;
int obs1_width = 10;

int obs2_x = SCREEN_WIDTH - 10;
int obs2_y = SCREEN_HEIGHT - 2;
int obs2_height = 0;
int obs2_width = 10;

int obs1_real_y = 0;
int obs2_real_y = 0;

int obs_speed = 2;

int draw_count1 = 0;
int draw_count2 = 0;

float EMA_a = 0.6;
int filtered_pot = 0;

bool upSpeed = false;
int leveledUp = 0;
int current_level = 1;

void setup() {

  delay(100);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  filtered_pot = analogRead(A0);


  display.clearDisplay();


  obs1_x = constrain(obs1_x, 0, SCREEN_WIDTH);
  obs2_x = constrain(obs2_x, 0, SCREEN_WIDTH);


}

void loop() {

  if (leveledUp == 1) {
    if (upSpeed) {
      obs_speed += 1;
      upSpeed = false;
    }
    leveledUp = 0;
  }


  if (!collisionCheck()) {
    moveMyRect();
    drawObstacleTop(obs1_x);
    drawObstacleBottom(obs2_x);
    moveObstacle(obs_speed);
  }
  else {
    gameOver();
  }

  //  Serial.print("my rect y:");
  //  Serial.println(myRectY+myRectH);
  //  Serial.print("bottom rect y:");
  //  Serial.println(obs2_real_y);
  //  Serial.print("top rect y:");
  //  Serial.print(obs1_real_y);
  //  Serial.println("");


}


void moveMyRect() {
  display.clearDisplay();
  int pot_pos = analogRead(A0);
  pot_pos = map(pot_pos, 0, 1023, 0, (SCREEN_HEIGHT - 10));
  filtered_pot = (EMA_a * pot_pos) + ((1 - EMA_a) * filtered_pot);
  myRectY = filtered_pot;
  display.fillRect(myRectX, myRectY, myRectW, myRectH, SSD1306_WHITE);
  display.display();
}

int height_select() {
  obs1_height = random(3, 20);
  obs2_height = random(10, 20);

  return obs1_height, obs2_height;
}

int drawObstacleTop(int x1) {
  randomSeed(analogRead(0));

  if (draw_count1 == 0) {
    height_select();
    draw_count1 = 1;
    delay(10);
  }
  obs1_real_y = obs1_y + obs1_height;
  display.fillRect(x1, obs1_y, obs1_width, obs1_height, SSD1306_WHITE);
  display.display();
  return obs1_real_y;
}

int drawObstacleBottom(int x2) {
  randomSeed(analogRead(0));

  if (draw_count2 == 0) {
    height_select();
    draw_count2 = 1;
    delay(10);
  }
  obs2_real_y = obs2_y - obs2_height;
  display.fillRect(obs2_x, (SCREEN_HEIGHT - 20), obs2_width, obs2_height, SSD1306_WHITE);
  display.display();

  return obs2_real_y;
}

int moveObstacle(int x_speed) {


  //border check
  if (obs1_x > 0 && !collisionCheck()) {
    obs1_x -= x_speed;
    display.display();
  }
  else if (obs1_x <= 0) {
    draw_count1 = 0;
    obs1_x = SCREEN_WIDTH - 10;
    levelUp();
    drawObstacleTop(obs1_x);
  }

  if (obs2_x > 0 && !collisionCheck()) {
    obs2_x -= x_speed;
    display.display();
  }
  else if (obs2_x <= 0) {
    draw_count2 = 0;
    obs2_x = SCREEN_WIDTH - 10;
    drawObstacleBottom(obs2_x);
  }


}

bool collisionCheck() {
  if ((myRectY <= obs1_real_y && obs1_x <= 10) || ((myRectY + myRectH) >= obs2_real_y && obs2_x <= 10)) {
    return 1;
  }
  else {
    return 0;

  }
}

void levelUp() {
  current_level++;
  leveledUp = 1;
  upSpeed = true;

}

void gameOver() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("YOU LOSE");
  display.display();
}
