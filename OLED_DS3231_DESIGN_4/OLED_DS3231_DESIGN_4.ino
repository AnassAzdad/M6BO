#include <Wire.h>
#include <DS3231.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>

int pause = 1000;

DS3231 clock;
RTCDateTime dt;

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  clock.begin();

  // Check if RTC is running correctly and sync with system time if needed
  RTCDateTime currentTime = clock.getDateTime();
  if (currentTime.year == 0) {
    // If year is zero, RTC has not been set, set it to your desired time
    clock.setDateTime(2025, 1, 10, 14, 18, 25);  // Example: Set to 14:12:00 on Jan 9th, 2025
  }

  // SSD1306 initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.display();  // Display initial Adafruit logo

  // Clear the buffer
  display.clearDisplay();
  display.display();

  // RTC control register reset (this is to reset the status registers if needed)
  Wire.beginTransmission(0x68);
  Wire.write(0xF);   // Address the Status register
  Wire.write(0x00);  // Zero the Status register
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0xE);   // Address the Control register
  Wire.write(0x00);  // Zero the Control register
  Wire.endTransmission();

  Wire.begin();
  clock.begin();
}

String DayOfTheWeek(uint8_t Day) {
  String DayText;
  if (Day == 1) DayText = "Monday";
  if (Day == 2) DayText = "Tuesday";
  if (Day == 3) DayText = "Wednesday";
  if (Day == 4) DayText = "Thursday";
  if (Day == 5) DayText = "Friday";
  if (Day == 6) DayText = "Saturday";
  if (Day == 7) DayText = "Sunday";
  return DayText;
}

String DayMonthYear(uint8_t Day, uint8_t Month, uint16_t Year) {
  String DayMonthYearText;
  if (Month == 1) DayMonthYearText = "JAN ";
  if (Month == 2) DayMonthYearText = "FEB ";
  if (Month == 3) DayMonthYearText = "MAR ";
  if (Month == 4) DayMonthYearText = "APR ";
  if (Month == 5) DayMonthYearText = "MAY ";
  if (Month == 6) DayMonthYearText = "JUN ";
  if (Month == 7) DayMonthYearText = "JUL ";
  if (Month == 8) DayMonthYearText = "AUG ";
  if (Month == 9) DayMonthYearText = "SEP ";
  if (Month == 10) DayMonthYearText = "OCT ";
  if (Month == 11) DayMonthYearText = "NOV ";
  if (Month == 12) DayMonthYearText = "DEC ";

  DayMonthYearText = DayMonthYearText + Day;
  if (Day == 1) DayMonthYearText = DayMonthYearText + "st ";
  if (Day == 2) DayMonthYearText = DayMonthYearText + "nd ";
  if (Day > 2) DayMonthYearText = DayMonthYearText + "th ";

  DayMonthYearText = DayMonthYearText + Year;

  return DayMonthYearText;
}

String AddLeadingZero(uint8_t x) {
  String AddLeadingZeroText;
  if (x < 10) AddLeadingZeroText = "0";
  else AddLeadingZeroText = "";
  AddLeadingZeroText = AddLeadingZeroText + x;
  return AddLeadingZeroText;
}

String CurrentTime(uint8_t h, uint8_t i) {
  String CurrentTimeText = "";
  CurrentTimeText = CurrentTimeText + AddLeadingZero(h) + ":" + AddLeadingZero(i);
  return CurrentTimeText;
}

void loop() {
  // Fetch the current time from the RTC
  dt = clock.getDateTime();

  // Print the RTC time to Serial Monitor for verification
  Serial.println(String(dt.year) + "-" + String(dt.month) + "-" + String(dt.day) + " " +
                 String(dt.hour) + ":" + String(dt.minute) + ":" + String(dt.second));

  // Clear the screen sections to prepare for new data
  display.fillRect(0, 0, 128, 16, WHITE);  // Use WHITE to clear
  display.fillRect(0, 16, 128, 28, BLACK); // Use BLACK to clear
  display.fillRect(0, 28, 128, 46, WHITE); // Use WHITE to clear
  display.fillRect(0, 46, 128, 64, BLACK); // Use BLACK to clear

  // Display Day of the Week
  display.setCursor(15, 1);
  display.setTextSize(2);
  display.setTextColor(BLACK); // Use BLACK for text
  display.println(DayOfTheWeek(dt.dayOfWeek));

  // Display Date
  display.setCursor(1, 18);
  display.setTextSize(1);
  display.setTextColor(WHITE); // Use WHITE for text
  display.println(DayMonthYear(dt.day, dt.month, dt.year));

  // Display Time (Hours:Minutes:Seconds) with +5 hours adjustment
  display.setCursor(15, 30);
  display.setTextSize(2);
  display.setTextColor(BLACK); // Use BLACK for text
  display.print(CurrentTime((dt.hour + ) % 24, dt.minute));  // Display HH:MM with +5 hours

  display.setCursor(87, 30);
  display.setTextSize(2);
  display.setTextColor(BLACK); // Use BLACK for text
  display.println(AddLeadingZero(dt.second));  // Display SS

  // Display Temperature
  display.setCursor(2, 48);
  display.setTextSize(2);
  display.setTextColor(WHITE); // Use WHITE for text
  display.println("Temp");

  clock.forceConversion();  // Ensure the temperature is updated
  display.setCursor(55, 48);
  display.setTextSize(2);
  display.setTextColor(WHITE); // Use WHITE for text
  display.print(clock.readTemperature());
  display.setCursor(116, 46);
  display.setTextSize(1);
  display.print("o");  // Degree symbol

  // Update the display
  display.display();

  // Add a 100-millisecond delay before the next update
  delay(100); // This will pause the program for 100 milliseconds (100ms)
}
