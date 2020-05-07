#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//voter data variables

String states[] = {"AK", "AL", "AR", "AZ", "CA", "CO", "CT", "DC", "DE", "FL", "GA", "HI", "IA", "ID", "IL", "IN", "KS", "KY", "LA", "MA", "MD", "ME", "MI", "MN", "MO", "MS", "MT", "NC", "ND", "NE", "NH", "NJ", "NM", "NV", "NY", "OH", "OK", "OR", "PA", "RI", "SC", "SD", "TN", "TX", "UT", "VA", "VT", "WA", "WI", "WV", "WY"};
String races[] = {"White", "Black", "Hispanic", "Asian", "Native American"};
String candidates[] = {"Republican", "Democrat", "Other"};

//Timer calculated as follows:
//White Timer = wait time data average from 2012 data
//All minorities: (wait time + if(no early voting penalty of 5 min))*(percentage of voters of affected race in state)


float white_timers[] = {11.3, 3.7, 10.8, 12.9, 5.8, 6.3, 8.5, 4.9, 33.9, 45, 17.8, 7, 7.1, 11.7, 14.3, 7.5, 11.5, 8, 20.2, 3.7, 29.2, 8.8, 21.9, 6.2, 7.7, 12.7, 16.5, 5.8, 8.5, 10.7, 4.7, 4.2, 9.5, 13.5, 7.5, 11, 15.9, 0, 9.1, 11.7, 25.2, 4.3, 13.7, 12.4, 10.3, 2, 23.6, 0, 9.8, 8.2, 4.5};
float black_timers[] = {20.6521, 3.8073, 11.2428, 14.8608, 6.119, 6.5457, 14.8365, 12.0285, 49.2567, 51.93, 23.3358, 7.112, 12.1726, 13.338, 21.0756, 12.9125, 17.4075, 14.04, 26.6842, 8.8044, 37.7848, 9.416, 30.5584, 11.9168, 17.5133, 19.7178, 21.586, 6.061, 9.2565, 15.9041, 10.9416, 4.2756, 10.8585, 16.362, 7.725, 17.952, 22.4048, 0, 15.6087, 17.6018, 38.2936, 9.4767, 15.9742, 13.8632, 10.4236, 7.084, 33.9768, 0, 10.1822, 14.0316, 9.5855};
float hispanic_timers[] = {26.9765, 5.9422, 16.7076, 22.2267, 7.946, 10.5966, 22.5045, 16.0578, 46.2735, 69.21, 27.1628, 8.526, 22.022, 18.8604, 34.5856, 23.2375, 29.0235, 23.998, 32.017, 16.8258, 44.0044, 15.092, 47.075, 20.1488, 19.8882, 31.7538, 40.0545, 10.382, 12.648, 29.8771, 15.0156, 5.7708, 14.7345, 22.005, 13.83, 28.624, 34.6104, 0, 24.8724, 28.7407, 49.4072, 16.9539, 23.8243, 17.5956, 18.3649, 13.496, 46.2462, 0, 18.816, 23.9184, 17.48};
float asian_timers[] = {16.5119, 3.9442, 11.1456, 13.1064, 6.6352, 6.4953, 14.1075, 10.296, 35.256, 46.26, 18.4942, 9.611, 12.2573, 12.3318, 19.7246, 12.825, 16.9785, 13.182, 20.5636, 8.7957, 31.0688, 9.3808, 27.7339, 11.7488, 12.8143, 18.054, 21.6505, 5.9392, 9.2055, 16.1239, 10.6506, 4.2546, 10.3265, 13.8915, 7.6275, 16.352, 21.3389, 0, 14.5935, 17.3012, 30.653, 9.4116, 13.9466, 12.9952, 10.5472, 7.126, 30.4304, 0, 9.8784, 13.5564, 9.576};
float na_timers[] = {16.3815, 4.2254, 11.2212, 12.9774, 5.8232, 6.3378, 13.527, 9.9198, 33.9678, 45.09, 17.8356, 7.007, 12.2331, 11.7117, 19.3193, 12.525, 16.599, 13.026, 20.301, 8.7522, 29.2584, 8.8088, 27.0345, 11.312, 12.7508, 17.7531, 22.7685, 5.8406, 8.5765, 15.7157, 9.7097, 4.5696, 9.519, 13.6485, 7.905, 16.032, 22.4257, 0, 14.1141, 16.7501, 30.2604, 10.0998, 13.7274, 12.4372, 10.403, 7.021, 28.6572, 0, 9.8098, 13.3056, 9.6995};


int state_val = 0;
int race_val = 0;
int cand_val = 0;
int new_race_val = 0;
int new_cand_val = 0;

//filter pot input for reliability
float EMA_a = 0.6;      //initialization of EMA alpha
int filtered_scroll = 0;

int print_count = 0;
int my_state = 0;
String my_race = " ";
String my_cand = " ";

int buttonPin = 2;
int lastButtonState = 0;
int lastButtonState2 = 0;

int init_print = 0;
int next_print = 0;
int third_print = 0;

unsigned long newTimer1 = 0;
unsigned long newTimer2 = 0;
unsigned long printTimer = 0;

bool state_select = false;
bool race_select = false;
bool cand_select = false;
int question_state = 0;
int pressed = 0;
bool button_state_change = false;


//game variables

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

bool upSpeed = false;
int leveledUp = 0;
int current_level = 1;

bool time2start = 1;

unsigned long newTimer3 = 0;
int filtered_pot = 0;

unsigned long playTimer = 0;
bool print_wait = 0;
bool cand_pick = 0;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  pinMode(buttonPin, INPUT_PULLUP);
  filtered_scroll = analogRead(A0);  //set EMA S for t=1
  filtered_pot = analogRead(A0);

  display.clearDisplay();


  obs1_x = constrain(obs1_x, 0, SCREEN_WIDTH);
  obs2_x = constrain(obs2_x, 0, SCREEN_WIDTH);


}

void loop() {
  button_check();

  if (init_print == 0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println("Select");
    display.println("your");
    display.println("choices");
    display.display();
    delay(2000);
    question_state = 1;
    init_print = 1;
  }
  if (question_state == 1) {
    delay(200);
    state_question();
  }
  else if (question_state == 2) {
    race_question();
  }
  else if (question_state == 3) {
    delay(200);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println("Your wait time is: ");
    wait_timer();
  }
}

void state_question() {
  state_val = analogRead(A0);

  if (pressed >= 1 && button_state_change) {
    my_state = filtered_scroll;
    button_state_change = false;
    pressed = 0;
    state_select = true;
  }
  else {
    if (!state_select) {
      state_val = map(state_val, 0, 1023, 0, 51);
      filtered_scroll = (EMA_a * state_val) + ((1 - EMA_a) * filtered_scroll);
      display.clearDisplay();
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 0);
      display.print("State: ");
      display.print(states[filtered_scroll]);
      display.println("");
      display.display();
      delay(100);
    }


    if (state_select) {
        question_state = 2;
      }

    }
}

int button_check() {
  int current_button_state = digitalRead(buttonPin);
  if (current_button_state != lastButtonState) {
    if (current_button_state == LOW) {
      delay(10);
      button_state_change = true;
      pressed ++;
    }
    lastButtonState = current_button_state;
  }


  return pressed;

}

String race_question() {
  race_val = analogRead(A0);

  if (pressed > 1 && button_state_change) {
    my_race = races[new_race_val];
    button_state_change = false;
    pressed = 0;
    race_select = true;
  }


  else {
    if (!race_select) {
      new_race_val = map(race_val, 0, 1023, 0, 4);
      display.clearDisplay();
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 0);
      display.print("Race: ");
      display.println(races[new_race_val]);
      display.println("");
      display.display();
      delay(100);
    }
    if (race_select) {
      
        question_state = 3;
        
    }
  }
  return my_race;
}

void wait_timer() {
  unsigned long waitTimer1 = millis();
  //if your race is white
  if (my_race == "White") {
    float white_wait_time = white_timers[my_state];
    if (print_wait == 0 && cand_pick == 0) {
      display.println(white_wait_time);
      display.print("minutes");
      display.display();
      delay(2000);
      print_wait = 1;
    }
    else if (print_wait == 1 && cand_pick == 0) {
      Serial.print("current timer in ms: ");
      Serial.println(waitTimer1 - playTimer);
      if (waitTimer1 - playTimer <= (white_wait_time * 60000)) {
        runGame();
      }
      else {
        cand_pick = 1;
      }
    }
    else if (cand_pick == 1) {
      pickCandidate();
    }
  }
  //if your race is black
  else if (my_race == "Black") {
    float black_wait_time = black_timers[my_state];
    if (print_wait == 0 && cand_pick == 0) {
      display.println(black_wait_time);
      display.print("minutes");
      display.display();
      delay(2000);
      print_wait = 1;
    }
    else if (print_wait == 1 && cand_pick == 0) {
      if (waitTimer1 - playTimer <= (black_wait_time * 60000)) {
        runGame();
      }
      else {
        cand_pick = 1;
      }
    }
    else if (cand_pick == 1) {
      pickCandidate();
    }
  }
  //if your race is hispanic
  else if (my_race == "Hispanic") {
    float hispanic_wait_time = hispanic_timers[my_state];
    if (print_wait == 0 && cand_pick == 0) {
      display.println(hispanic_wait_time);
      display.print("minutes");
      display.display();
      delay(3000);
      print_wait = 1;
    }
    if (print_wait == 1 && cand_pick == 0) {
      if (waitTimer1 - playTimer <= (hispanic_wait_time * 60000)) {
        runGame();
      }
      else {
        cand_pick = 1;
      }
    }
    else if (cand_pick == 1) {
      pickCandidate();
    }
  }
  //if your race is asian
  else if (my_race == "Asian") {
    float asian_wait_time = asian_timers[my_state];
    if (print_wait == 0 && cand_pick == 0) {
      display.println(asian_timers[my_state]);
      display.print("minutes");
      display.display();
      delay(3000);
      print_wait = 1;
    }
    else if (print_wait == 1 && cand_pick == 0) {
      if (waitTimer1 - playTimer <= (asian_wait_time * 60000)) {
        runGame();
      }
      else {
        cand_pick = 1;
      }
    }
    else if (cand_pick == 1) {
      pickCandidate();
    }
  }
  //if your race is native american
  else if (my_race == "Native American") {
    float na_wait_time = na_timers[my_state];
    if (print_wait == 0 && cand_pick == 0) {
      display.println(na_timers[my_state]);
      display.print("minutes");
      display.display();
      delay(3000);
      print_wait = 1;
    }
    else if (print_wait == 1 && cand_pick == 0) {
      if (waitTimer1 - playTimer <= (na_wait_time * 60000)) {
        runGame();
      }
      else {
        cand_pick = 1;
      }
    }
    else if (cand_pick == 1) {

      pickCandidate();
    }
  }

}

void pickCandidate() {
  cand_val = analogRead(A0);
  if (pressed >= 1 && button_state_change) {
    my_cand = candidates[new_cand_val];
    button_state_change = false;
    cand_select = true;
  }
  else {
    if (!cand_select) {
      new_cand_val = map(cand_val, 0, 1023, 0, 2);
      display.clearDisplay();
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 0);
      display.println("Vote: ");
      display.println(candidates[new_cand_val]);
      display.println("");
      display.display();
      delay(100);
    }


    if (cand_select) {
      display.clearDisplay();
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(5, 0);
      display.println("You voted:");
      display.print(my_cand);
      display.println("");
      display.display();
    }
  }

}

void runGame() {

  if (time2start == 1) {
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
      time2start = 0;
      gameOver();
    }
  }


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
  obs1_height = random(5, 20);
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
  delay(100);
  unsigned long currentTime3 = millis();
  if (currentTime3 - newTimer3 >= 3000) {
    newTimer3 = currentTime3;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println("Your");
    display.print("Score:");
    display.println(current_level);
    display.display();
    delay(5000);
    current_level = 1;
    obs_speed = 2;
    obs1_x = SCREEN_WIDTH - 10;
    obs2_x = SCREEN_WIDTH - 10;
    time2start = 1;
  }

}
