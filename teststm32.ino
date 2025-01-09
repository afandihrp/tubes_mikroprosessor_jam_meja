#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "RTClib.h"
#include "EEPROM.h"
#include <STM32TimerInterrupt.h>

// #include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include "cus_char.h"
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
#define EEPROM_SIZE 64
// Adafruit_BMP280 bmp;
Adafruit_AHTX0 aht;

RTC_DS1307 rtc;

STM32Timer Timerstm1(TIM1);

uint8_t alarm_hour, alarm_minutes;

const char dayOfTheWeek[][12]={
  "Minggu",
  "Senin",
  "Selasa",
  "Rabu",
  "Kamis",
  "Jum'at",
  "Sabtu"
};

void createchar()
{
  lcd.createChar(1,bar1);
  lcd.createChar(2,bar2);
  lcd.createChar(3,bar3);
  lcd.createChar(4,bar4);
  lcd.createChar(5,bar5);
  lcd.createChar(6,bar6);
  lcd.createChar(7,bar7);
  lcd.createChar(8,bar8);
}

bool is_the_buzzer_on = false;

void buzzer(){
  if(is_the_buzzer_on)
  {
    for(uint8_t x = 0;x < 5;x++)
    {
      digitalWrite(PB0, HIGH);
      delay(60);
      digitalWrite(PB0, LOW);
      delay(60);
    }
  }
}

void setup()
{
  
  pinMode(PA1, INPUT_PULLUP);
  pinMode(PA2, INPUT_PULLUP);
  pinMode(PA3, INPUT_PULLUP);
  pinMode(PA4, INPUT_ANALOG);
  pinMode(PB0, OUTPUT);


  Serial.begin(115200);
  // EEPROM.begin(EEPROM_SIZE);
  Timerstm1.attachInterruptInterval(1000*1000, buzzer);
 

  alarm_hour = EEPROM.read(1);
  alarm_minutes = EEPROM.read(2);

  Wire.begin();
  // bmp.begin();
  
  
  Serial.print("Initializing lcd");
  lcd.init();
  createchar();
  lcd.backlight();
  
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }


}

void custom_big_numbers(uint8_t col, uint8_t row, uint8_t number)
{
  switch(number)
  {
    case 0:
      lcd.setCursor(col, row); 
      lcd.write(2);  
      lcd.write(8); 
      lcd.write(1);
      lcd.setCursor(col, row+1); 
      lcd.write(2);  
      lcd.write(6);  
      lcd.write(1);
    break;
    case 1:
      lcd.setCursor(col,row);
      lcd.write(32);
      lcd.write(32);
      lcd.write(1);
      lcd.setCursor(col,row+1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(1);
    break;
    case 2:
      lcd.setCursor(col,row);
      lcd.write(5);
      lcd.write(3);
      lcd.write(1);
      lcd.setCursor(col, row+1);
      lcd.write(2);
      lcd.write(6);
      lcd.write(6);
    break;
    case 3:
      lcd.setCursor(col,row);
      lcd.write(5);
      lcd.write(3);
      lcd.write(1);
      lcd.setCursor(col, row+1);
      lcd.write(7);
      lcd.write(6);
      lcd.write(1);
    break;
    case 4:
      lcd.setCursor(col,row);
      lcd.write(2);
      lcd.write(6);
      lcd.write(1);
      lcd.setCursor(col, row+1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(1);
    break;
    case 5:
      lcd.setCursor(col,row);
      lcd.write(2);
      lcd.write(3);
      lcd.write(4);
      lcd.setCursor(col, row+1);
      lcd.write(7);
      lcd.write(6);
      lcd.write(1);
    break;
    case 6:
      lcd.setCursor(col,row);
      lcd.write(2);
      lcd.write(3);
      lcd.write(4);
      lcd.setCursor(col, row+1);
      lcd.write(2);
      lcd.write(6);
      lcd.write(1);
    break;
    case 7:
      lcd.setCursor(col,row);
      lcd.write(2);
      lcd.write(8);
      lcd.write(1);
      lcd.setCursor(col, row+1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(1);
    break;
    case 8:
      lcd.setCursor(col, row); 
      lcd.write(2);  
      lcd.write(3); 
      lcd.write(1);
      lcd.setCursor(col, row+1); 
      lcd.write(2);  
      lcd.write(6);  
      lcd.write(1);
    break;
    case 9:
      lcd.setCursor(col, row); 
      lcd.write(2);  
      lcd.write(3); 
      lcd.write(1);
      lcd.setCursor(col, row+1); 
      lcd.write(7);  
      lcd.write(6);  
      lcd.write(1);
    break;
  }
}

void print_big_clock_numbers(uint8_t clock_x, uint8_t clock_y, uint8_t hour, uint8_t minute)
{
  uint8_t hour1 = hour/10;
  uint8_t hour2 = hour%10;

  uint8_t minute1 = minute/10;
  uint8_t minute2 = minute%10;

  custom_big_numbers(clock_x,clock_y,hour1);
  custom_big_numbers(clock_x+3,clock_y,hour2);
  lcd.setCursor(clock_x+6, clock_y);
  lcd.print(":");
  lcd.setCursor(clock_x+6, clock_y+1);
  lcd.print(":");
  custom_big_numbers(clock_x+7,clock_y,minute1);
  custom_big_numbers(clock_x+10,clock_y,minute2);
}

void get_offline_Time_dates(int8_t *hour, int8_t *minutes, int8_t *seconds, int16_t *year, int8_t *month, int8_t *dates, String *day)
{
  DateTime now=rtc.now();
  *hour = now.hour();
  *minutes = now.minute();
  *seconds = now.second();
  *year = now.year();
  *month = now.month();
  *dates = now.day();
  *day = dayOfTheWeek[now.dayOfTheWeek()];
}

uint32_t timer1, timer2;
bool menu = false;

char list_menu[][12] = {
  "Atur Jam",
  "Atur Alarm",
  "Keluar"
};

int8_t cursor = 0, menu_selected = 0;
void loop()
{

  // Serial.println(String(alarm_hour)+"-"+String(alarm_minutes));
  int8_t hour,minutes,seconds,dates,month;
  int16_t year;
  String day;

  get_offline_Time_dates(&hour,&minutes,&seconds,&year,&month,&dates,&day);

  if(hour == alarm_hour && minutes == alarm_minutes && seconds <= 60)
  {
    is_the_buzzer_on = true;
  }
  else
  {
    is_the_buzzer_on = false;
  }
  


  if(!digitalRead(PA1) && !menu)
  {
    while(digitalRead(PA1))
    {
      delay(10);
    }
    
    delay(300);
    cursor = 0;
    menu=true;
    lcd.clear();
  }
  

  if(millis()-timer1 > 1000 && !menu)
  {
    lcd.setCursor(0,2);
    lcd.print(String(day)+","+String(dates)+"-"+String(month)+"-"+String(year));
    timer1 = millis();
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    // Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    // Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

    print_big_clock_numbers(0,0,hour,minutes);
    lcd.setCursor(13,0);
    uint8_t seconds1 = seconds/10, seconds2 = seconds%10;
    lcd.print(".");
    lcd.print(String(seconds1)+String(seconds2));    

    lcd.setCursor(0,3);
    lcd.print(temp.temperature);
    lcd.write(B11011111);   // print degree symbol
    lcd.print("C");
    lcd.print(" ");
    lcd.print(humidity.relative_humidity);
    lcd.print("%");
  }

  if(menu)
  {    
    bool confirm = false;
    if(menu_selected == 0) 
    {
      if(!digitalRead(PA1)) 
      {
        lcd.clear();
        confirm = true;
        delay(300);
      }
      else if(!digitalRead(PA2)) 
      {
        cursor--;
        delay(100);
      }
      else if(!digitalRead(PA3)) 
      {
        cursor++;
        delay(100);
      }
      
      if(millis()-timer1>500)
      {
        if(cursor < 0)
        {
          cursor = 2;
        }
        if(cursor > 2)
        {
          cursor = 0;
        }          
        lcd.setCursor(0,0);
        lcd.print("========Menu========");
        uint8_t length1 = sizeof(list_menu)/sizeof(list_menu[0]);
        for(uint8_t x=0; x<length1; x++)
        {
          uint8_t str_length = strlen(list_menu[x]);
          uint8_t calculate = (20-str_length)/2;
          lcd.setCursor(calculate, x+1);
          lcd.print(list_menu[x]);
          if(x == cursor)
          {
            lcd.setCursor(calculate-2, x+1);
            lcd.print(">");
            lcd.setCursor(calculate+str_length+1, x+1);
            lcd.print("<");
          }
          else
          {
            lcd.setCursor(calculate-2, x+1);
            lcd.print(" ");
            lcd.setCursor(calculate+str_length+1, x+1);
            lcd.print(" ");
          }
          if(cursor == 0 && confirm) 
          {
            menu_selected = 1;
            lcd.clear();
          }
          else if(cursor == 1 && confirm) 
          {
            menu_selected = 2;
            lcd.clear();
          }
          else if(cursor == 2 && confirm) 
          {
            menu = false;
            lcd.clear();
          }
        }
      }
    }
    else if(menu_selected == 1)
    {
      int8_t hourset,minutesset,secondsset,datesset,monthset,step=0;
      int16_t yearset;
      String dayset;

      get_offline_Time_dates(&hourset,&minutesset,&secondsset,&yearset,&monthset,&datesset,&dayset);

      bool run_once = true,run_once1= true, change_1=true;
      uint32_t timer_2nd_1;
      while(1)
      {
        if(run_once)
        {
          run_once = false;
          // DateTime temp=rtc.now();
          // hourset = temp.hour();
          // minutesset = temp.minute();
          get_offline_Time_dates(&hourset,&minutesset,&secondsset,&yearset,&monthset,&datesset,&dayset);
        }

        if(!digitalRead(PA1)) 
        {        
          while(digitalRead(PA1))
          {
            delay(50);
          } 
          delay(300);
          Serial.println("1 is pressed");         
          step++;
        }
        if(!digitalRead(PA2) && step == 0) 
        {
          hourset++;
          delay(100);
        }
        else if(!digitalRead(PA2) && step == 1) 
        {
          minutesset++;
          delay(100);
        }
        else if(!digitalRead(PA2) && step == 2) 
        {
          datesset++;
          delay(100);
        }
        else if(!digitalRead(PA2) && step == 3) 
        {
          monthset++;
          delay(100);
        }
        else if(!digitalRead(PA2) && step == 4) 
        {
          yearset++;
          delay(100);
        }



        if(!digitalRead(PA3) && step == 0) 
        {
          hourset--;
          delay(100);
        }
        else if(!digitalRead(PA3) && step == 1) 
        {
          minutesset--;
          delay(100);
        }
        else if(!digitalRead(PA3) && step == 2) 
        {
          datesset--;
          delay(100);
        }
        else if(!digitalRead(PA3) && step == 3) 
        {
          monthset--;
          delay(100);
        }
        else if(!digitalRead(PA3) && step == 4) 
        {
          yearset--;
          delay(100);
        }

        if(step >=5)
        { 
          menu_selected = 0;          
          rtc.adjust(DateTime(yearset,monthset,datesset,hourset,minutesset,secondsset));
          lcd.clear();
          delay(300);
          return;
        }
        // This line sets the RTC with an explicit date & time , for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

        if(hourset>23) hourset = 0;
        else if(hourset<0) hourset = 23;

        if(minutesset>59) minutesset = 0;
        else if(minutesset<0) minutesset = 59;

        if(datesset>31) datesset = 0;
        else if(datesset<0) datesset = 31;

        if(monthset>12) monthset = 0;
        else if(monthset<0) monthset = 12;

        Serial.println("step: "+String(step));
        if(step<=1)
        {
          if(millis()-timer_2nd_1>400)
          {
            timer_2nd_1 = millis();
            if(change_1)
            {
              print_big_clock_numbers(2,1,hourset,minutesset);
              change_1 = false;
            }
            else if(!change_1 && step == 0)
            {
              lcd.setCursor(2,1);
              lcd.print("      ");
              lcd.setCursor(2,2);
              lcd.print("      ");
              change_1 = true;
            }
            else if(!change_1 && step == 1)
            {
              lcd.setCursor(9,1);
              lcd.print("      ");
              lcd.setCursor(9,2);
              lcd.print("      ");
              change_1 = true;
            }
            
          }
          
          
          
        }        
        else if(step>1)
        {
          if(run_once1)
          {
            lcd.clear();
            run_once1 = false;
          }
          lcd.setCursor(1,0);
          lcd.print("Tanggal: "+String(datesset));
          lcd.setCursor(1,1);
          lcd.print("Bulan: "+String(monthset));
          lcd.setCursor(1,2);
          lcd.print("Tahun: "+String(yearset));
          
          for(uint8_t x=0;x<3;x++)
          {
            if(step-2 == x)
            {
              lcd.setCursor(0,x);
              lcd.print(">");
            }
            else
            {
              lcd.setCursor(0,x);
              lcd.print(" ");
            }            
          }
        }
      }  
    }
    else if(menu_selected == 2)
    {
      int8_t step = 0;
      bool change_1 = true;
      while(1)
      {
        uint32_t timer_3nd_1;
        if(!digitalRead(PA1))
        {
          while(digitalRead(PA1))
          {
            delay(50);
          }
          step++;
          delay(300);
        }

        if(!digitalRead(PA2) && step == 0)
        {
          alarm_hour++;
          delay(100);
        }
        else if(!digitalRead(PA2) && step == 1)
        {
          alarm_minutes++;
          delay(100);
        }

        if(!digitalRead(PA3) && step == 0)
        {
          alarm_hour--;
          delay(100);
        }
        else if(!digitalRead(PA3) && step == 1)
        {
          alarm_minutes--;
          delay(100);
        }

        if(step >= 2)
        {
          menu_selected = 0;  
          lcd.clear();
          EEPROM.write(1, alarm_hour);
          EEPROM.write(2, alarm_minutes);
          delay(300);
          return;

        }

        if(alarm_hour>23) alarm_hour = 0;
        else if(alarm_hour<0) alarm_hour = 23;

        if(alarm_minutes>59) alarm_minutes = 0;
        else if(alarm_minutes<0) alarm_minutes = 59;

        if(millis()-timer_3nd_1>400)
        {
          timer_3nd_1 = millis();
          if(change_1)
          {
            print_big_clock_numbers(2,1,alarm_hour,alarm_minutes);
            change_1 = false;
          }
          else if(!change_1 && step == 0)
          {
            lcd.setCursor(2,1);
            lcd.print("      ");
            lcd.setCursor(2,2);
            lcd.print("      ");
            change_1 = true;
          }
          else if(!change_1 && step == 1)
          {
            lcd.setCursor(9,1);
            lcd.print("      ");
            lcd.setCursor(9,2);
            lcd.print("      ");
            change_1 = true;
          }
        }
      }      
    }    
  }
}
