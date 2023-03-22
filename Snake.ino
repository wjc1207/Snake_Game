//Snake game
//Author: wjc74751
//UCL = https://github.com/wjc74751/Snake_Game

// * * * -->move direction
// 2 1 0

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OFFSETX 40
#define XDIM 0
#define YDIM 1
#define RANDOMSEED (0)
#define JOYSTICKX (1)
#define JOYSTICKY (2)

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //For arduino nano, SDA: A4, SCL: A5

uint8_t Position[100][2];
uint8_t nextPosition[2];
int velocity[2];
uint8_t Length;
uint8_t food[2];
uint8_t score = 0;
bool isFoodExist = false;

void setup() {
  Serial.begin(9600);

  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000);         // wait for initializing
  oled.clearDisplay(); // clear display

  Position[0][0] = 6;  //snake original positions
  Position[0][1] = 3;
  Position[1][0] = 5;
  Position[1][1] = 3;
  Position[2][0] = 4;
  Position[2][1] = 3;
  Position[3][0] = 3;
  Position[3][1] = 3;

  velocity[0] = 1; //snake original velocity
  velocity[1] = 0;

  Length = 4;
}
void drawBorder()
{
  oled.drawLine(40, 0, 40, 63, WHITE);
}
void drawPixel4X(uint8_t X, uint8_t Y, uint8_t offsetX) //transform to 32*16
{
  if ((X < 32) and (Y < 16))
    for (uint8_t i = 0; i <= 3; i = i + 1)
    {
      for (uint8_t j = 0; j <= 3; j = j + 1)
      {
        oled.drawPixel(4 * X + i + offsetX, 4 * Y + j, WHITE);
      }
    }
}
void drawCircle4X(uint8_t X, uint8_t Y, uint8_t offsetX) //transform to 32*16
{
  uint8_t bitMap[4][4] = {{0, 1, 1, 0}, {1, 1, 1, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}};
  if ((X < 32) and (Y < 16))
    for (uint8_t i = 0; i <= 3; i = i + 1)
    {
      for (uint8_t j = 0; j <= 3; j = j + 1)
      {
        if (bitMap[i][j] == 1)
          oled.drawPixel(4 * X + i + offsetX, 4 * Y + j, WHITE);
      }
    }
}

void drawPixel8X(uint8_t X, uint8_t Y) //transform to 16*8
{
  if ((X < 16) and (Y < 8))
    for (uint8_t i = 8 * X; i <= 8 * X + 7; i = i + 1)
    {
      for (uint8_t j = 8 * Y; j <= 8 * Y + 7; j = j + 1)
      {
        oled.drawPixel(i, j, WHITE);
      }
    }
}
void generateFood()
{
  while (isFoodExist == false) //generate food
  {
    bool overlap = false;
    randomSeed(analogRead(RANDOMSEED));
    food[0] = random(0, (32 - OFFSETX / 4));
    food[1] = random(0, 16);
    for (int i = Length - 1; i >= 0; i = i - 1)
    {
      if (Position[i][0] == food[0] and Position[i][1] == food[1])
      {
        overlap = true;
        break;
      }
    }
    if (overlap == false)
    {
      isFoodExist = true;
      break;
    }
  }
  drawCircle4X(food[0], food[1], OFFSETX);
}

void(* resetFunc) (void) = 0; //reset the board
void showScore()
{
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println(F("Score: ")); //show score
  oled.println(score);
}
void getNextPosition()
{
  if ((Position[0][XDIM] + velocity[XDIM]) == (32 - OFFSETX / 4))
  {
    nextPosition[XDIM] = 0;
    nextPosition[YDIM] = Position[0][1];
  }
  else if ((Position[0][XDIM] + velocity[XDIM]) == -1)
  {
    nextPosition[XDIM] = (31 - OFFSETX / 4);
    nextPosition[YDIM] = Position[0][1];
  }
  else if ((Position[0][YDIM] + velocity[YDIM]) == -1)
  {
    nextPosition[XDIM] = Position[0][0];
    nextPosition[YDIM] = 15;
  }
  else if ((Position[0][YDIM] + velocity[YDIM]) == 16)
  {
    nextPosition[XDIM] = Position[0][0];
    nextPosition[YDIM] = 0;
  }
  else
  {
    nextPosition[0] = Position[0][0] + velocity[0]; //x' = x + delta(x)
    nextPosition[1] = Position[0][1] + velocity[1]; //y' = y + delta(y)
  }
}
void eatFood()
{
  if ((nextPosition[0] == food[0]) and (nextPosition[1] == food[1]))  //eat
  {
    Length = Length + 1;
    score = score + 1;
    isFoodExist = false;
  }
}
void snakeMove()
{
  for (int i = Length - 1; i >= 0; i = i - 1) //move
  {
    if (i == 0)
    {
      Position[0][0] = nextPosition[0];
      Position[0][1] = nextPosition[1];
    }
    else
    {
      if (((Position[0][0] + velocity[0]) == Position[i][0]) and ((Position[0][1] + velocity[1]) == Position[i][1])) //collision detection
      {
        oled.setCursor(OFFSETX + 19, 29);
        oled.setTextSize(1);
        oled.println(F("Game Over!")); //5x7
        oled.display();
        delay(100);
        resetFunc();
      }
      Position[i][0] = Position[i - 1][0];
      Position[i][1] = Position[i - 1][1];
    }
    drawPixel4X(Position[i][0], Position[i][1], OFFSETX);
  }
}
void control()
{
  if (analogRead(JOYSTICKX) > 800 and velocity[1] == 0)
  {
    velocity[0] = 0;
    velocity[1] = -1;
  }
  else if (analogRead(JOYSTICKY) < 200 and velocity[0] == 0)
  {
    velocity[0] = -1;
    velocity[1] = 0;
  }
  else if (analogRead(JOYSTICKX) < 200 and velocity[1] == 0)
  {
    velocity[0] = 0;
    velocity[1] = 1;
  }
  else if (analogRead(JOYSTICKY) > 800 and velocity[0] == 0)
  {
    velocity[0] = 1;
    velocity[1] = 0;
  }
}

void loop() {
  oled.clearDisplay(); // clear display

  showScore(); //show score

  drawBorder();

  generateFood(); //generate food

  getNextPosition();

  eatFood(); //eat food

  snakeMove();  //move

  control(); //player control

  oled.display();               // show on OLED
  delay(200);
}
