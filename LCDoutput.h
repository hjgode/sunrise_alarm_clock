// Outputs Hour, minute and second.
void printTime(LiquidCrystal_I2C &lcd, int col, int row, DateTime &dateTime)    
{
  lcd.setCursor(col ,row);
  if (dateTime.hour()<10)
    lcd.print("0");
  lcd.print(dateTime.hour(), DEC);
  lcd.print(":");
  if (dateTime.minute()<10)
    lcd.print("0");
  lcd.print(dateTime.minute(), DEC);
  lcd.print(":");
  if (dateTime.second()<10)
    lcd.print("0");
  lcd.print(dateTime.second(), DEC);
}
// Outputs Day of week, day, month and year.
void printDate(LiquidCrystal_I2C &lcd, int col, int row, DateTime &dateTime) 
{
  lcd.setCursor(col, row);

  switch(dateTime.dayOfWeek()){
  case 0: 
    lcd.print("Sun");
    break;
  case 1: 
    lcd.print("Mon");
    break;
  case 2: 
    lcd.print("Tue");
    break;
  case 3: 
    lcd.print("Wed");
    break;
  case 4: 
    lcd.print("Thu");
    break;
  case 5: 
    lcd.print("Fri");
    break;
  case 6: 
    lcd.print("Sat");
    break;
  }
  lcd.print(" ");
  if (dateTime.day()<10)
    lcd.print("0");
  lcd.print(dateTime.day(), DEC);
  lcd.print("-");
  if (dateTime.month()<10)
    lcd.print("0");
  lcd.print(dateTime.month(), DEC);
  lcd.print("-");
  lcd.print(dateTime.year(), DEC);
}

// Only outputs hour and munite
void printAlarmTime(LiquidCrystal_I2C &lcd, int col, int row, DateTime &dateTime)    
{
  lcd.setCursor(col ,row);
  if (dateTime.hour()<10)
    lcd.print("0");
  lcd.print(dateTime.hour(), DEC);
  lcd.print(":");
  if (dateTime.minute()<10)
    lcd.print("0");
  lcd.print(dateTime.minute(), DEC);
}

// Outputs the value of either; Current time, Alarm Time, Wake-up light fade or Snooze time.
void printItemValue(LiquidCrystal_I2C &lcd, int &menuIndex, DateTime &now, DateTime &alarmTime, int &lightFade, int &snoozeTime)
{
  Serial.println((int)now.second());
  //if (now.second() % 2 == 0)
  {
    switch (menuIndex)
    {
    case 0: 
      {
        printDate(lcd, 0, 1, now);
        break;
      }
    case 1: 
      {
        printTime(lcd, 0, 1, now);
        break;
      }
    case 2: 
      {
        printAlarmTime(lcd, 0, 1, alarmTime);
        break;
      }
    case 3: 
      {
        lcd.setCursor(0, 1);
        lcd.print(lightFade);
        lcd.print(" minutes");
        break;
      }
    case 4: 
      {
        lcd.setCursor(0, 1);

        lcd.print(snoozeTime);
        lcd.print(" minutes");
        break;
      }
    }
  }
}


// **********************************
// ****** Display Big Numbers  ******
// **********************************

// helper method.
void writeDigitBits(LiquidCrystal_I2C &lcd, int pos, char* bitmap)
{
  lcd.setCursor(pos, 0);
  lcd.write((int)bitmap[0] - 48); // 1  
  lcd.write((int)bitmap[1] - 48); // 0  
  lcd.write((int)bitmap[2] - 48); // 1
  lcd.setCursor(pos, 1);  
  lcd.write((int)bitmap[3] - 48);  // 1
  lcd.write((int)bitmap[4] - 48);  // 2
  lcd.write((int)bitmap[5] - 48);  // 1
}

// Output Big digit at position pos.
void WriteDigit(LiquidCrystal_I2C &lcd, int pos, int value)
{
  char* bitMaps[10] = {
    "101121", // 0
    "017212", // 1
    "331122", // 2
    "031221", // 3
    "121771", // 4
    "133221", // 5
    "133121", // 6
    "001717", // 7
    "131121", // 8
    "131221"  // 9
  };

  if (value < 10)
  {
    writeDigitBits(lcd, pos, bitMaps[0]); // Zero
    writeDigitBits(lcd, pos + 4, bitMaps[value]);
  }
  else // value >= 10
  {
    int firstDigit = (int)(value / 10);
    writeDigitBits(lcd, pos, bitMaps[firstDigit]);

    int lastDigit = (value - (firstDigit*10) / 1);
    writeDigitBits(lcd, pos + 4, bitMaps[lastDigit]);
  }
}

// Output Big Time
void WriteBigTime(LiquidCrystal_I2C &lcd, DateTime time, bool alarmOn)
{
  WriteDigit(lcd, 0, time.hour());
  WriteDigit(lcd, 9, time.minute());

  lcd.setCursor(8, 0);
  lcd.write(((time.second() % 2) == 0) ? 4 : 7);
  lcd.setCursor(8, 1);
  if (alarmOn)
    lcd.write(((time.second() % 2) == 0) ? 7 : 4);
  else
    lcd.write(((time.second() % 2) == 0) ? 4 : 7);
}

