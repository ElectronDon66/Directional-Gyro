
//This is working 2/16/26
//BNO085 has to be strapped for address 0x4A 
//
// Sketch for a Directional Gyro project 
// The TFT_eSPI driver needs to be configured correctly to work with the GC9A01 TFT display 
//The processer is the Teensy 4.0
// The IMU is an Adafruit BNO085 or GY-BNO085 
//The compiler will throw a few warnings but that is OK they are not critical errors 






#include <Wire.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Adafruit_BNO08x.h>

// =====================
// Hardware Objects
// =====================
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
Adafruit_BNO08x bno = Adafruit_BNO08x();

// =====================
// Encoder Pins
// =====================
#define ENC_A 2
#define ENC_B 3

volatile long encoderCount = 0;
volatile uint8_t lastEncoded = 0;

// =====================
// Display
// =====================
#define SCREEN_SIZE 240
#define CENTER 120

// =====================
// Heading
// =====================
float currentHeading = 0;
float lastHeading = -999;

// =====================
// Quadrature ISR
// =====================
void updateEncoder()
{
  uint8_t MSB = digitalRead(ENC_A);
  uint8_t LSB = digitalRead(ENC_B);

  uint8_t encoded = (MSB << 1) | LSB;
  uint8_t sum  = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    encoderCount++;

  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
    encoderCount--;

  lastEncoded = encoded;
}

// =====================
// Quaternion → Yaw
// =====================
float quaternionToYaw(float w, float x, float y, float z)
{
  float siny_cosp = 2.0 * (w * z + x * y);
  float cosy_cosp = 1.0 - 2.0 * (y * y + z * z);
  float yaw = atan2(siny_cosp, cosy_cosp);
  return yaw * 180.0 / PI;
}

// =====================
// Draw Compass Rose
// =====================
void drawCompassRose(float heading)
{
  sprite.fillSprite(TFT_BLACK);

  // -------- Compass Card Ticks --------
  for (int deg = 0; deg < 360; deg += 5)
  {
    float angle = radians(deg + heading);

    int rOuter = 110;
    int rInner = 105;

    if (deg % 30 == 0) {
      rInner = 88;    // Long tick (30°)
    }
    else if (deg % 10 == 0) {
      rInner = 98;    // Medium tick (10°)
    }

    int x0 = CENTER + sin(angle) * rInner;
    int y0 = CENTER - cos(angle) * rInner;
    int x1 = CENTER + sin(angle) * rOuter;
    int y1 = CENTER - cos(angle) * rOuter;

    sprite.drawLine(x0, y0, x1, y1, TFT_WHITE);
  }

  // -------- 30° Numbers (Except N/E/S/W) --------
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);

  for (int deg = 0; deg < 360; deg += 30)
  {
    // Skip cardinal directions
    if (deg == 0 || deg == 90 || deg == 180 || deg == 270)
      continue;

    float angle = radians(deg + heading);
    int x = CENTER + sin(angle) * 72;
    int y = CENTER - cos(angle) * 72;

    int displayNum = deg / 10;   // 30→3, 60→6, etc.
    char buf[3];
    sprintf(buf, "%d", displayNum);

    sprite.drawString(buf, x, y, 2);
  }

  // -------- N E S W Letters --------
  struct {
    char label;
    int deg;
  } cardinal[] = {
    {'N', 0},
    {'E', 90},
    {'S', 180},
    {'W', 270}
  };

  for (int i = 0; i < 4; i++)
  {
    float angle = radians(heading + cardinal[i].deg);

    //float angle = radians(cardinal[i].deg - heading);
    int x = CENTER + sin(angle) * 70;
    int y = CENTER - cos(angle) * 70;

    char txt[2] = {cardinal[i].label, '\0'};
    sprite.drawString(txt, x, y, 4);
  }

  // =================================================
  // Aircraft Symbol (Centered, Larger, Longer Nose)
  // =================================================

  int cx = CENTER;
  int cy = CENTER;

  // Nose (extended upward)
  sprite.fillTriangle(
    cx,      cy - 58,   // tip (extended)
    cx - 10, cy - 22,
    cx + 10, cy - 22,
    TFT_YELLOW
  );

  // Fuselage
  sprite.fillRect(cx - 4, cy - 22, 8, 55, TFT_YELLOW);

  // Wings
  sprite.fillRect(cx - 45, cy - 5, 90, 10, TFT_YELLOW);

  // Tailplane
  sprite.fillRect(cx - 12, cy + 26, 24, 6, TFT_YELLOW);
}


/*
void drawCompassRose(float heading)
{
  sprite.fillSprite(TFT_BLACK);

  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);

  // Draw 30° marks and labels
  for (int deg = 0; deg < 360; deg += 30)
  {
    float angle = radians(heading - deg);

    int x = CENTER + sin(angle) * 100;
    int y = CENTER - cos(angle) * 100;

    char buf[6];
    sprintf(buf, "%d", deg);
    sprite.drawString(buf, x, y, 2);
  }

  // Aircraft fixed index marker (top)
  sprite.fillTriangle(
    CENTER - 10, 15,
    CENTER + 10, 15,
    CENTER, 35,
    TFT_YELLOW
  );
}
*/
// =====================
// Setup
// =====================
void setup()
{
// Reset the BNO


   //Wire.setClock(40000);
  Serial.begin(115200);
  Wire.begin();

  // Encoder setup
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  lastEncoded = (digitalRead(ENC_A) << 1) | digitalRead(ENC_B);

  attachInterrupt(digitalPinToInterrupt(ENC_A), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), updateEncoder, CHANGE);

  // Display init
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(SCREEN_SIZE, SCREEN_SIZE);
  sprite.setSwapBytes(true);

  // BNO085 init
  if (!bno.begin_I2C())
  {
    Serial.println("BNO085 not detected");
   while (1);
  }

  // Game Rotation Vector (NO magnetometer)
  bno.enableReport(SH2_GAME_ROTATION_VECTOR, 5000);
}

// =====================
// Main Loop
// =====================
void loop()
{
  sh2_SensorValue_t sensorValue;

  if (bno.getSensorEvent(&sensorValue))
  {
    if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR)
    {
      float q_w = sensorValue.un.gameRotationVector.real;
      float q_x = sensorValue.un.gameRotationVector.i;
      float q_y = sensorValue.un.gameRotationVector.j;
      float q_z = sensorValue.un.gameRotationVector.k;

      float yaw = quaternionToYaw(q_w, q_x, q_y, q_z);

      // Convert encoder counts to degrees
      float headingOffset = encoderCount * 0.5;  // adjust scaling here

      currentHeading = yaw + headingOffset;

      // Normalize
      while (currentHeading < 0) currentHeading += 360;
      while (currentHeading >= 360) currentHeading -= 360;

      // Only redraw if changed enough
      if (abs(currentHeading - lastHeading) > 0.2)
      {
        drawCompassRose(currentHeading);
        sprite.pushSprite(0, 0);
        lastHeading = currentHeading;
      }
    }
  }
}
