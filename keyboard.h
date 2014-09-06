// =====================================================
// Returns the key corrosponding to the analog value.
// 0: Right, 1: Left, 2: Down, 3: Up, 4: Enter, 5: Menu
// =====================================================
Button getKey(int delayTime)
{
	int result = -1;
	int value = 0;
	int tolerance = 4; // Allowable diference between 1st and 2nd reading of analog value.
	int sampleSize = 10;
        int aRead=analogRead(A0);
//Serial.print("analogRead="); Serial.println(aRead);
	if (aRead >= 1000) // discard
	{
//Serial.println("no key");
	  return noKey;
	}

	for (int i = 0; i < sampleSize; i++)
	{
		value += analogRead(A0);
		delay(5); //2
	}

	value = (value / sampleSize);
//Serial.print("value=");Serial.println(value);
	delay(delayTime); // Short delay (50) for setting values, longer (200) for menu navigation.

	//Serial.println(value);

	int valMin = value - tolerance;
	int valMax = value + tolerance;

	if ((value > valMin) && (value < valMax))
	{
		if ((value > 280) && (value < 290))
		{
			return btnLeft; 
		}
		else if ((value > 775) && (value < 790))
		{
			return btnDown; 
		}
		else if ((value > 450) && (value < 470))//was 455-570
		{
			return btnRight; 
		}
		else if ((value > 600) && (value < 620))//was 610-620
		{
			return btnUp; 
		}
		else if ((value > 80 && (value < 100)))
		{
			return btnEnter; 
		}
		else if ((value > 170) && (value < 190))
		{
			return btnMenu; 
		}
		return noKey;
	} 
      else{
        Serial.println("readKey outside tolerance");
      }
}

// ****************************************
// *** SetTime ****************************
// ****************************************
int SetTime(LiquidCrystal_I2C &lcd, char* menuTxt, DateTime& time, boolean isAlarmTime)
{
	Button result = noKey;
	delay(100);
	int cursorPos = 0;
	lcd.clear();
	//lcd.setCursor(0, 0);
	lcd.print(menuTxt);

	if (isAlarmTime)
		printAlarmTime(lcd, 0, 1, time);
	else
		printTime(lcd, 0, 1, time);

	lcd.setCursor(cursorPos, 1);
	lcd.cursor();

	//delay(500);
	//int analogValue = analogRead(A0);
	//  Serial.print("Analog value:");
	//  Serial.println(analogValue);
	int lastKey = -1;

	int hour = time.hour();
	int minute = time.minute();
	int second = time.second();

	Button key = noKey;
	do  {
		key = getKey(50);

		if (key != lastKey)
		{
			//Serial.println(key);
			int checkValue = isAlarmTime ? 4 : 7;
			switch (key)
			{
			case btnRight: // Right
					if (cursorPos < checkValue)
						cursorPos++;
					else
						cursorPos = 0;
					if ((cursorPos == 2) || (cursorPos == 5)) // 1st and 2nd colon.
						cursorPos++;
					break;
			case btnLeft: // Left 
					if (cursorPos > 0)
						cursorPos--;
					else
						cursorPos = checkValue;
					if ((cursorPos == 2) || (cursorPos == 5)) // 1st and 2nd colon.
						cursorPos--;
					break;
			case btnDown: // Down
					if (cursorPos == 0) // 1st digit hour
                                        {
						if (hour - 10 > 0)
							hour -= 10;
					//            else if (hour > 0)
					//            {
					//              hour -= 1;
					//              cursorPos++;
					//            }
                                        }
					if (cursorPos == 1) // 2nd digit hour
                                        {
						if (hour > 0)
							hour -= 1;
						else
							hour = 23;
                                        }
					if (cursorPos == 3) // 1st digit minute
                                        {
						if (minute - 10 > 0)
							minute -= 10;
					//            else if (minute > 0)
					//            {
					//              minute -= 1; 
					//              cursorPos++;
					//            }
                                        }
					if (cursorPos == 4) // 2nd digit minute
                                        {
						if (minute > 0)
							minute -= 1;
						else
							minute = 59;

					if (cursorPos == 6) // 1st digit second
						if (second - 10 > 0)
							second -= 10;
					//            else if (second > 0)
					//            {
					//              second -= 1;
					//              cursorPos++;
					//            }
                                        }
					if (cursorPos == 7) // 2nd digit second
                                        {
						if (second > 0)
							second -= 1;               
						else
							second = 59;
                                        }
					break;
			case btnUp: // Up 
				{
					if (cursorPos == 0) // 1st digit hour
						if (hour + 10 < 24)
							hour += 10;

					if (cursorPos == 1) // 2nd digit hour
						if (hour < 23)
							hour += 1;
						else
							hour = 0;

					if (cursorPos == 3) // 1st digit minute
						if (minute + 10 < 60)
							minute += 10;

					if (cursorPos == 4) // 2nd digit minute
						if (minute < 59)
							minute += 1;
						else
							minute = 0;

					if (cursorPos == 6) // 1st digit second
						if (second + 10 < 60)
							second += 10;

					if (cursorPos == 7) // 2nd digit second
						if (second < 59)
							second += 1;
						else 
							second = 0;
					break;
				}        
			case btnEnter: // Enter 
				{
					DateTime tempTime(0, 0, 0, hour, minute, second);
					time = tempTime;
					//Serial.println("Exit Set Time/Alarm");
					delay(100);
					result = btnEnter; // Enter
				}
			}

			//Serial.print(", cursorPos: ");
			//Serial.println(cursorPos);

			//DateTime tempTime(year,month,day,hour,minute,second);
			DateTime tempTime(0, 0, 0, hour, minute, second);
			if (isAlarmTime)
				printAlarmTime(lcd, 0, 1, tempTime);
			else
				printTime(lcd, 0, 1, tempTime);

			lcd.setCursor(cursorPos,1);
			lcd.cursor();
		}
		lastKey = key;
	}
	while ((key != btnEnter) && (key != btnMenu));

	//Serial.println("Exit SetTime");
	return result;
}

// ****************************************
// *** SetDate ****************************
// ****************************************
int SetDate(LiquidCrystal_I2C &lcd, char* menuTxt, DateTime& time)
{
	Button result = noKey;
	delay(100);
	int cursorPos = 4;
	lcd.clear();
	//lcd.setCursor(0, 0);
	lcd.print(menuTxt);

	printDate(lcd, 0, 1, time);
	lcd.setCursor(cursorPos, 1);
	lcd.cursor();

	//delay(500);
	//int analogValue = analogRead(A0);
	//  Serial.print("Analog value:");
	//  Serial.println(analogValue);
	int lastKey = -1;

	int year = time.year();
	int month = time.month();
	int day = time.day();
	int hour = time.hour();
	int minute = time.minute();
	int second = time.second();

	int key = -1;
	do
	{
		key = getKey(50);

		if (key != lastKey)
		{
			//Serial.println(key);

			switch (key)
			{
			case 0: // Right
				{
					if (cursorPos < 13)
						cursorPos++;
					else
						cursorPos = 4;

					if (cursorPos == 6) // 1st colon.
						cursorPos+=2;
					if (cursorPos == 9) // 2nd colon.
						cursorPos+=4;
					break;
				}
			case 1: // Left 
				{
					if (cursorPos > 4)
						cursorPos--;
					else
						cursorPos = 13;

					if (cursorPos == 7) // 1st colon.
						cursorPos-=2;
					if (cursorPos == 9) // 2nd colon.
						cursorPos-=4;
					break;
				}
			case 2: // Down
				{
					// 1st digit Day 
					if (cursorPos == 4) 
					{  
						if (day - 10 > 0)
							day -= 10;
					}
					// 2nd digit Day
					if (cursorPos == 5) 
					{
						if (day > 1)
							day--;
					}

					// 1st digit Month
					if (cursorPos == 7) 
					{
						if (month-10 > 12)
							month-=10;
					}
					// 2nd digit Month
					if (cursorPos == 8) 
					{
						if (month > 1)
							month--;
					}

					// 1st digit Year
					if (cursorPos == 12) 
					{
						if (year-10 > 2000)
							year-=10;
					}
					// 2nd digit Year
					if (cursorPos == 13) 
					{
						if (year > 2000)
							year--;
					}
					break;
				}
			case 3: // Up
				{
					// 1st digit Day 
					if (cursorPos == 4) 
					{  
						if (day + 10 < 32)
							day += 10;
					}
					// 2nd digit Day
					if (cursorPos == 5) 
					{
						if (day < 31)
							day++;
					}

					// 1st digit Month
					if (cursorPos == 7)
					{
						if (month+10 < 10)
							month+=10;
					}
					// 2nd digit Month
					if (cursorPos == 8) 
					{
						if (month < 12)
							month++;
					}

					// 1st digit Year
					if (cursorPos == 12) 
					{
						if (year+10 < 2100)
							year+=10;
					}
					// 2nd digit Year
					if (cursorPos == 13) 
					{
						if (year < 2100)
							year++;
					}
					break;
				}
			case 4: // Enter 
				{
					DateTime tempTime(year, month, day, hour, minute, second);
					time = tempTime;
					//Serial.println("Enter pressed. Exiting SetDate");
					result = btnEnter; // Enter
					break;
				}
			}

			DateTime tempTime(year, month, day, hour, minute, second);
			printDate(lcd, 0, 1, tempTime);

			lcd.setCursor(cursorPos, 1);
			lcd.cursor();

			lastKey = key;
		}
	} 
	while ((key != 4) && (key != 5));

	//  Serial.print("Exit SetDate:");
	//  Serial.print(" lastKey");
	//  Serial.print (lastKey);
	//  Serial.print(", result");
	//  Serial.println(result);

	delay(100);
	return result;
}

// ****************************************
// *** SetIntegerValue ********************
// ****************************************
int SetIntegerValue(LiquidCrystal_I2C &lcd, int col, int row, char* menuTxt, char* msg, int& value)
{
	int result = -1;
	delay(100);
	//int cursorPos = 11;
	lcd.clear();
	//lcd.setCursor(0, 0);
	lcd.print(menuTxt);

	//int analogValue = analogRead(A0);
	int lastKey = -1;
	int tempValue = value;

	lcd.setCursor(0, 1);
	if (tempValue < 10)
		lcd.print("0");
	lcd.print(tempValue);
	lcd.print(" ");
	lcd.print(msg);
	lcd.setCursor(1, 1);
	lcd.cursor();

	int key = -1;
	do{
		key = getKey(50);

		if (key != lastKey)
		{
			switch (key)
			{
			case 2: // Down
				{
					if (tempValue > 0)
						tempValue--;
					break;
				}
			case 3: // Up
				{
					if (tempValue < 60)
						tempValue++;
					else
						tempValue = 0;
					break;
				}

			case 4: // Enter 
				{
					value = tempValue;
					//Serial.println("Exit Set Light On");
					delay(100);
					result = 4; // Enter
				}
			}

			lcd.setCursor(0,1);
			if (tempValue < 10)
				lcd.print("0");
			lcd.print(tempValue);
			lcd.print(" ");
			lcd.print(msg);
			lcd.setCursor(1, 1);
			lcd.cursor();

			lastKey = key;
		}
	}
	while ((key != 4) && (key != 5));
	//Serial.println("Exit SetIntegerValue");
	return result;
}

void ShowCurrentState(LiquidCrystal_I2C &lcd, int index, bool state, int frequency)
{
	lcd.setCursor(0, 1);
	switch (index)
	{
	case 0:
	case 2:
		lcd.print(state ? "Enabled " : "Disabled");
		break;
	case 1:
		lcd.print(state ? "Radio   " : "Buzzer  ");
		break;
	case 3:
		lcd.print(state ? "On      " : "Off     ");
		break;
	case 4:
		lcd.print(frequency);
		lcd.print(" MHz");
		break;
	}
}

// Items[0] = "ALARM";			// Enable/Disable alarm.
// Items[1] = "WAKE UP LIGHT";	// Enable/Disable wake up light.
// Items[2] = "ALARM IS";		// Select alarm type; Buzzer or Radio
// Items[3] = "BACK LIGHT";		// On/Off.
// Items[4] = "RADIO FREQ";  // FM min -> FM max. 
Button QuickSetUp(LiquidCrystal_I2C &lcd, 
	bool &alarmState, 
	bool &wakeUpLightState, 
	bool &alarmType, 
	bool &backLightState,
#ifdef USE_FM_RADIO
	Si4735 &radio,
#endif
	int &radioFrequency
        )
{
	int menuIndex = 0; 

	Serial.println("Enter QS");
	Button result = noKey;
	Button lastKey = noKey;
#ifdef USE_FM_RADIO
	radio.tuneFrequency(radioFrequency);
#endif
	delay(100);

	lcd.clear();
	lcd.print(quickItems[menuIndex]);

	bool value;
	int frequency;

	switch (menuIndex)
	{
	case 0:
		value = alarmState;
		break;
	case 1:
		value = alarmType;
		break;
	case 2:
		value = wakeUpLightState;
		break;
	case 3:
		value = backLightState;
		break;
	case 4:
		frequency = radioFrequency;
		break;
	}

	ShowCurrentState(lcd, menuIndex, value, frequency);

	Button key = noKey;

	do {
		key = getKey(50); // TODO: Experiment with larger value (if display flikkers or stability issues.)

		if (key != lastKey)
		{
			switch (key)
			{
			case btnLeft : // Decrease or toggle value.
				{
					Serial.println("Left");
					value = !value;
					if (menuIndex == 4)
					{
						frequency -= 10;
#ifdef USE_FM_RADIO
						radio.tuneFrequency(radioFrequency);
#endif
					}
					ShowCurrentState(lcd, menuIndex, value, frequency);

					Serial.println(value);
					break;
				}
			case btnRight : // Increase or toggle value.
				{
					Serial.println("Left");
					value = !value;
					if (menuIndex == 4)
					{
						frequency += 10;
#ifdef USE_FM_RADIO
						radio.tuneFrequency(radioFrequency);
#endif
					}
					ShowCurrentState(lcd, menuIndex, value, frequency);

					Serial.println(value);
					break;
				}

			case btnUp : // Previous menu item.
				{
					Serial.println("Up");
					if (menuIndex < quickMenuCount - 1)
						menuIndex++;
					else
						menuIndex = 0;

					lcd.clear();
					lcd.print(quickItems[menuIndex]);
					lcd.setCursor(0, 1);

					switch (menuIndex)
					{
					case 0:{
						value = alarmState;
						break;
						   }
					case 1:{
						value = alarmType;
						break;
						   }
					case 2:{
						value = wakeUpLightState;
						break;
						   }
					case 3:{
						value = backLightState;
						break;
						   }
					case 4:{
						frequency = radioFrequency;
						break;
						   }
					}
					ShowCurrentState(lcd, menuIndex, value, frequency);
					Serial.println(menuIndex);
					break;
				}
			case btnDown : // Next menu item.
				{
					Serial.println("Down");
					if (menuIndex > 0)
						menuIndex--;
					else
						menuIndex = quickMenuCount - 1;

					lcd.clear();
					lcd.print(quickItems[menuIndex]);
					lcd.setCursor(0, 1);

					switch (menuIndex)
					{
					case 0:
						value = alarmState;
						break;
					case 1:
						value = alarmType;
						break;
					case 2:
						value = wakeUpLightState;
						break;
					case 3:
						value = backLightState;
						break;
					case 4:{
						frequency = radioFrequency;
						break;
						   }
					}
					ShowCurrentState(lcd, menuIndex, value, frequency);
					Serial.println(menuIndex);
					break;
				}

			case btnEnter : // Set value and exit menu.
				{
					switch (menuIndex)
					{
					case 0:
						alarmState = value;
						break;
					case 1:
						alarmType = value;
						break;
					case 2:
						wakeUpLightState = value;
						break;
					case 3:
						backLightState = value;
						break;
					case 4:{
						radioFrequency = frequency;
						break;
						   }
					}

					result = btnEnter; 
					break;
				}
			case btnMenu : // Cancel???.
				{
					// TODO: Cancel
					//delay(100);
					result = btnMenu; 
					break;
				}
			}

			// Show selected menu.
			//lcd.clear(); // TODO: Check if this flickers the display.
			//lcd.print(quickItems[menuIndex]); 

			lastKey = key;
		}
	}
	while ((key != btnEnter) && (key != btnMenu));

	lcd.clear();
	Serial.println("Exit QS");
	
	return result;
}

// ****************************************
// *** Main Set-up menu *******************
// ****************************************
int ShowSetupMenu(LiquidCrystal_I2C &lcd, RTC_DS1307 &RTC, DateTime &now, DateTime &alarmTime, int &lightFade, int &snoozeTime)
{
	// ==========================
	// Read keypad.
	// ==========================  
	//int analogValue = analogRead(A0);
	Button key = noKey;
	Button lastKey = noKey;

	int menuIndex = 0;

	lcd.clear();
	lcd.noCursor();

	lcd.clear();
	lcd.print(menuItems[menuIndex]);
	lcd.print(" ");
	lcd.write(6); // Enter symbol

	do {

		key = (Button)getKey(100);
		//Serial.println(key);
		if (key != lastKey )
		{
			//Serial.print("Key: "); 
			//Serial.println(key, DEC);
			displayTimeOut = millis();

			if (key == btnUp)
				menuIndex--;
			if (key == btnDown)
				menuIndex++;

			if (menuIndex >= menuItemCount)
				menuIndex = 0;
			if (menuIndex < 0)
				menuIndex = menuItemCount - 1;

			//Serial.println(menuItems[menuIndex]);
			lcd.clear();
			lcd.print(menuItems[menuIndex]);
			lcd.print(" ");
			lcd.write(6); // Enter symbol

			if (key == btnEnter)
			{
				switch (menuIndex)
				{
				case 0: // SET DATE
					{
						int result = SetDate(lcd, menuItems[menuIndex], now); // Execute Set Date

						int year = now.year();
						int month = now.month();
						int day = now.day();

						if (result == 4) // Enter
						{
							// Update Real Time Clock.
							if (!DEBUGMODE)
								RTC.adjust(DateTime(year, month, day, RTC.now().hour(), RTC.now().minute(), RTC.now().second()));       

							tone(buzzerPin, beepFrequency, 100);
							lcd.clear();
							lcd.print(_Ok);

							delay(pauseTime);
							return key;
						}
						else
						{
							lcd.clear();
							lcd.print(_Cancel);
							delay(pauseTime);
							return key;
						}
						break;
					}
				case 1: // SET TIME
					{
						int result = SetTime(lcd, menuItems[menuIndex], now, false); // Execute Set Date
						int hour = now.hour();
						int minute = now.minute();
						int second = now.second();

						if (result == 4) // Enter
						{
							// Update Real Time Clock
							if (!DEBUGMODE)
								RTC.adjust(DateTime(RTC.now().year(), RTC.now().month(), RTC.now().day(), hour, minute, second));       

							tone(buzzerPin, beepFrequency, 100);
							lcd.clear();
							lcd.print(_Ok);
							delay(pauseTime);
							return key;
						}
						else
						{
							lcd.clear();
							lcd.print(_Cancel);
							delay(pauseTime);
							return key;
						}
						break;
					}
				case 2: // SET ALARM
					{
						int result = SetTime(lcd, menuItems[menuIndex], alarmTime, true); // Execute Set Alarm

						int alarmHour = alarmTime.hour();
						int alarmMinute = alarmTime.minute();
						Serial.print(alarmHour);
						Serial.print(":");
						Serial.print(alarmMinute);
						Serial.println();
						if (result == 4) // Enter
						{
							// Update Real Time Clock
							if (!DEBUGMODE)
							{
								EEPROM.write(alarmHourAddr, alarmHour);
								EEPROM.write(alarmMinuteAddr, alarmMinute);
							}
							tone(buzzerPin, beepFrequency, 100);
							lcd.clear();
							lcd.print(_Ok);
							delay(pauseTime);
							return key;
						}
						else
						{
							lcd.clear();
							lcd.print(_Cancel);
							delay(pauseTime);
							return key;
						}
						break;
					}
				case 3: // SET FADE IN TIME
					{
						int result = SetIntegerValue(lcd, 0, 1, menuItems[menuIndex], "minutes", lightFade);
						Serial.print(lightFade);
						if (result == 4) // Enter
						{
							// Update Real Time Clock
							if (!DEBUGMODE)
							{
								EEPROM.write(fadeInTimeAddr, lightFade);
							}
							tone(buzzerPin, beepFrequency, 100);
							lcd.clear();
							lcd.print(_Ok);
							delay(pauseTime);
							return key;
						}
						else
						{
							lcd.clear();
							lcd.print(_Cancel);
							delay(pauseTime);
							return key;
						}
						break;
					}
				case 4: // SET SNOOZE TIME
					{
						int result = SetIntegerValue(lcd, 0, 1, menuItems[menuIndex], "minutes", snoozeTime);
						Serial.print(snoozeTime);
						if (result == 4) // Enter
						{
							// Update Real Time Clock
							if (!DEBUGMODE)
							{
								EEPROM.write(fadeInTimeAddr, snoozeTime);
							}
							tone(buzzerPin, beepFrequency, 100);
							lcd.clear();
							lcd.print(_Ok);
							delay(pauseTime);
							return key;
						}
						else
						{
							lcd.clear();
							lcd.print(_Cancel);
							delay(pauseTime);
							return key;
						}
						break;
					}
				case 5: // Exit menu
					{
						menuIndex = 0;
						break;
					}
				}
			}
		}
	} 
	while ((key != btnMenu) && ((millis() - displayTimeOut) < 5000));

	return key; // Should be 5

}

