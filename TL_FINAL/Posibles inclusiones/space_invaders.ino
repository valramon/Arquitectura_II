#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// Matrice Dimensions
const uint8_t MAT_WIDTH = 32;
const uint8_t MAT_HEIGHT = 16;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

const int MAX_LEVEL = 4;
const int MAX_INVADERS = 16;

const int CANNON_BALL_DELAY = 50;
const int INVADER_DELAY = 5000;

const int strengths[MAX_LEVEL][MAX_INVADERS] = {{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
                                                {1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2},
                                                {1,2,3,4,5,1,2,3,4,5,1,2,3,4,5,1},
                                                {5,4,5,4,5,4,5,4,2,3,2,3,2,3,2,3}
                                                };

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      strength = 0;
      time = millis();
    }
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      time = millis();
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
      time = millis();
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
      time = millis();
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // setters
    void set_pos(int x_arg, int y_arg){
      x = x_arg;
      y = y_arg;
    }
    void set_strength(int s){
      strength = s;
      time = millis();
    }
    
    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      if(strength > 0 && (millis() - time) > INVADER_DELAY){
        erase();
        y++;
        if(y >= (MAT_HEIGHT - 4)){
          reached_bottom = true;
        }
        else{
          draw();
        }
        time = millis();
      }
    }

    bool has_touched_player(int x_arg, int y_arg){
      if (get_strength() > 0) {
        bool _cannon = (x <= x_arg && x >= (x + 3)) && (y_arg == (y + 3));
        bool _overlap = false;
        for(int i = x; i <= (x + 3); i++){
          for(int j = (x_arg - 1); j <= (x_arg + 1); j++){
            _overlap = (i == j);
          }
        }

        bool _body = _overlap && ((y_arg + 1) == (y + 3));

        if (_cannon && _body) {
          return true;
        }
      }
      return false;
    }

    bool has_reached_bottom(){
      return reached_bottom;
    }

    void clear_reached_bottom(){
      reached_bottom = false;
    }
    
    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      switch(get_strength()){
        case 1:
          draw_with_rgb(RED, BLUE);
          break;
        case 2:
          draw_with_rgb(ORANGE, BLUE);
          break;
        case 3:
          draw_with_rgb(YELLOW, BLUE);
          break;
        case 4:
          draw_with_rgb(GREEN, BLUE);
          break;
        case 5:
          draw_with_rgb(BLUE, BLUE);
          break;
        case 6:
          draw_with_rgb(PURPLE, BLUE);
          break;
        case 7:
          draw_with_rgb(WHITE, BLUE);
          break;
        default:
          break;
      }
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      strength--;
      if(strength <= 0){
        erase();
      }
      else{
        draw();
      }
    }

    bool has_been_hit(int x_arg, int y_arg) {
      if (get_strength() > 0) {
        bool _x = x_arg == (x + 1) || x_arg == (x + 2);
        bool _y = y_arg == (y + 1);
        
        if (_x && _y) {
          return true;
        }
      }
      return false;
    }

  private:
    int x;
    int y;
    int strength;
    unsigned long time;
    bool reached_bottom = false;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      matrix.drawPixel(x + 1, y, body_color.to_333());
      matrix.drawPixel(x + 2, y, body_color.to_333());
      
      matrix.drawPixel(x + 0, y + 1, body_color.to_333());
      matrix.drawPixel(x + 1, y + 1, eye_color.to_333());
      matrix.drawPixel(x + 2, y + 1, eye_color.to_333());
      matrix.drawPixel(x + 3, y + 1, body_color.to_333());
      
      matrix.drawPixel(x + 0, y + 2, body_color.to_333());
      matrix.drawPixel(x + 1, y + 2, body_color.to_333());
      matrix.drawPixel(x + 2, y + 2, body_color.to_333());
      matrix.drawPixel(x + 3, y + 2, body_color.to_333());
  
      matrix.drawPixel(x + 0, y + 3, body_color.to_333());
      matrix.drawPixel(x + 3, y + 3, body_color.to_333());
    }
};

class Cannonball {
  public:
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
      time = millis();
    }
    
    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
      time = millis();
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    void fire(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      fired = true;
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
      if(fired && (millis() - time) > CANNON_BALL_DELAY){
        erase();
        y--;
        if(y < 0){
          reset();
        }
        else{
          draw();
        }
        time = millis();
      }
    }
    
    // resets private data members to initial values
    void hit() {
      erase();
      reset();
    }
    
    // draws the Cannonball, if it is fired
    void draw() {
      matrix.drawPixel(x, y, ORANGE.to_333());
      matrix.drawPixel(x, y - 1, ORANGE.to_333());
    }
    
    // draws black where the Cannonball used to be
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x, y - 1, BLACK.to_333());
    }

  private:
    int x;
    int y;
    bool fired;
    unsigned long time;
};

class Player {
  public:
    Player() {
      x = 1;
      y = MAT_HEIGHT-2;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }
    
    // Modifies: lives
    void die() {
      lives--;
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(AQUA);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    int x;
    int y;
    int lives = 0;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      lives = 3;
    }
    
    // draws the player
    void draw_with_rgb(Color color) {
      matrix.drawPixel(x, y+1, color.to_333());
      matrix.drawPixel(x-1, y+1, color.to_333());
      matrix.drawPixel(x+1, y+1, color.to_333());
      // player's cannon
      matrix.drawPixel(x, y, color.to_333());
    }
};

class Game {
  public:
    Game() {
      level = 1;
      time = 0;
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setup(int level_arg=1) {
      level = level_arg;
      if (level == 1) {
        num_invaders = 8;        
      }
      else {
        num_invaders = 16;
      }

      reset_invaders();
      print_level(level);
      delay(2000);
      print_lives(player.get_lives());
      delay(2000);
      matrix.fillScreen(BLACK.to_333());
    }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {
      if(is_game_over){
        game_over();
        delay(2000);
        return;
      }
      
      // Update the position of the player based on the value of the potentiometer
      player.erase();
      player.set_x(((MAT_WIDTH) * potentiometer_value) / 1024);
      player.draw();

      // Update the position of the cannonball, including detecting if a new cannonball is being fired
      if (button_pressed && !ball.has_been_fired()) {
        ball.fire(player.get_x(), 14);
      }
      ball.move();

      // Update the position of each invader
      for (int i = 0; i < num_invaders; ++i) {
        enemies[i].draw();
        // Move enemy
        if( (i/8 == 0) || enemies_killed(8) ){
          enemies[i].move();
        }        

        // Detect if the cannonball is colliding with any of the invaders;
        // if so, both the cannonball and the invader disappear
        if (ball.has_been_fired()) {
          if(enemies[i].has_been_hit(ball.get_x(), ball.get_y())){
            // redraw the enemy
            enemies[i].hit();
            // erase the ball
            ball.hit();

            // Detect if all the invaders are killed; if so, start the next level
            if(level_cleared()){
              Serial.println("Level Cleared!");
              next_level();
            }
          }
        }

        // Detect if the player is colliding with any of the invaders; if so, the player loses a life and the level restarts
        if(enemies[i].has_touched_player(player.get_x(), player.get_y()) || enemies[i].has_reached_bottom()){
          player.die();
          if (player.get_lives() < 1){
            is_game_over = true;
            game_over();
          }
          else{
            reset_level();
          }
          
        }
        
      }
    }

  private:
    int level;
    unsigned long time;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES];
    int num_invaders;
    bool is_game_over = false;

    // Check if first N enemies killed
    bool enemies_killed(int first_n = 16){
      for (int n = 0; n < first_n; n++) {
        if (enemies[n].get_strength() != 0) {
          return false;
        }
      }
      return true;
    }

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      return enemies_killed(NUM_ENEMIES);
    }

    // Next level
    void next_level(){
      level++;
      setup(level);
    }

    // set up a level
    void reset_level() {
      setup(level);
    }

    // reset the enemies
    void reset_invaders(){
      int strength;
      for (int i = 0; i < num_invaders; ++i) {          
          if(level >= 1 && level <= 4){
            strength = strengths[level-1][i];
          }
          else{
            strength = random(1, 8);
          }
          
          enemies[i].set_strength(strength);
          enemies[i].set_pos( ((i % 8) * 4), (num_invaders/16) * (1-(i/8)) * 5 );
          enemies[i].clear_reached_bottom();
       }
    }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.setup(1);
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int total = 0;
  int num_readings = 10;
  for (int i = 0; i < num_readings; i++) {
    total += analogRead(POTENTIOMETER_PIN_NUMBER);
  }  
  int potentiometer_value = total / num_readings;
  
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.update(potentiometer_value, button_pressed);
}

// displays Level
void print_level(int level) {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(1,0);
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print('L');
  matrix.print('E');
  matrix.print('V');
  matrix.print('E');
  matrix.print('L');
  matrix.print(' ');
  matrix.print(level);
}

// displays number of lives
void print_lives(int lives) {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(1,0);
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print('L');
  matrix.print('I');
  matrix.print('V');
  matrix.print('E');
  matrix.print('S');
  matrix.print(' ');
  matrix.print(lives);
  
}

// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(1,0);
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print('G');
  matrix.print('A');
  matrix.print('M');
  matrix.print('E');
  matrix.print(' ');
  matrix.print('O');
  matrix.print('V');
  matrix.print('E');
  matrix.print('R');
}


