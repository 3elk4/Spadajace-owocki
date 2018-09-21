#include <ISADefinitions.h>
#include <ISAOLED.h>
#include <ISALiquidCrystal.h>
#include <stdlib.h>
#include <time.h>

#include "grafiki.h"
#include "struktury.h"

ISAOLED oled;
ISALiquidCrystal lcd;

Player player1;
Fruit fruit[16];

bool refresh = true;

int timer = 0;

void setup() {
  oled.begin();
  lcd.begin();
  Serial.begin(9600);
  
  pinMode(24, OUTPUT);

  //srand(time(0));
  randomSeed(analogRead(A7) + analogRead(A6));
  
  //gracz
   player1.posX = 61;
   player1.points = 0;
   player1.speed = 0.8;
   
  // owocek
  for(int i = 0; i < 16; ++i) {
    fruit[i].posY = -6;
    fruit[i].speed = 0.3;
    fruit[i].isFalling = false;
    fruit[i].typeFruit = random(0, 3);
  }

  timer = millis();
}

void loop() {
  if(player1.points < 0) {
    lcd.clear();
    lcd.print("KONIEC GRY!");
    for(int i = 0; i < 128; ++i) {
      for(int j = 0; j < 64; ++j) {
        oled.setPixel(i, j, 1);
      }
    }
    oled.renderAll();
    return;
  }
  
  if (refresh) {
    clearWindow();
    refresh = false;

   for(int i = 0; i < 16; ++i) {
      if(fruit[i].isFalling) {
        drawFruit(i * 8, fruit[i].posY, fruit[i].typeFruit);    
      }
   }
   drawBasket((int)player1.posX);
   
   oled.renderAll();
   lcd.clear();
   lcd.print("Punkty: ");
   lcd.print(player1.points);
  }

  if ((millis() - timer) >= (4000 - (50 * player1.points))) {
    int newFruits = random(0, 1 + player1.points / 10);
    if (newFruits > 10) {
      newFruits = 8;
    }
    for(int i = newFruits; i >= 0; --i) {
      int choosenFruit = random(0, 16);
      if (fruit[choosenFruit].isFalling) {
        continue;
      }
      fruit[choosenFruit].isFalling = true;
      fruit[choosenFruit].posY = -6;
      fruit[choosenFruit].speed = 0.05 * random(1, 3) + 0.1 * (player1.points / 5);
      fruit[choosenFruit].typeFruit = random(0, 3);
    }
    timer =  millis();
  }

  for(int i = 0; i < 16; ++i) {
      if(fruit[i].isFalling){
        refresh = true;
        fruit[i].posY += fruit[i].speed; 
        if(fruit[i].posY >= 64) {
           fruit[i].isFalling = false;   
        }
        if(fruit[i].posY >= 58 && fruit[i].posY <= 62){
          int basketCenter = player1.posX + 3;
          if(basketCenter > (i * 8) && basketCenter < ((i + 1) * 8)) {
             //zniszcz owocka
             //dodaj punkt
             fruit[i].isFalling = false;
             if(fruit[i].typeFruit == 0) {
                player1.points += 1;
             }
             else if(fruit[i].typeFruit == 1) {
                player1.points += 2;
             }
             else if(fruit[i].typeFruit == 2) {
                player1.points += 3;
             }
             fruitPlusSignal();
          }
          else if(basketCenter < (i * 8) || basketCenter > ((i + 1) * 8)) {
             fruit[i].isFalling = false;
             if(fruit[i].typeFruit == 0) {
                player1.points -= 1;
             }
             else if(fruit[i].typeFruit == 1) {
                player1.points -= 2;
             }
             else if(fruit[i].typeFruit == 2) {
                player1.points -= 3;
             }
          }
        } 
        
      }
   }  

  int joystick = analogRead(JOY1X);
  if(joystick > 600) {
    //lewo
    player1.posX -= player1.speed;
    refresh = true;
  }
  else if(joystick < 400) {
    //prawo
    player1.posX += player1.speed;
    refresh = true;
  }

  if(player1.posX < 0) {
    player1.posX = 0;
  }
  else if(player1.posX > 122) {
    player1.posX = 122;
  }
  
  if (player1.points > 15) {
    delay(2);
  } else {
    delay(10);  
  }
}

void clearWindow() {
  for(int i = 0; i < 128; ++i) {
    for(int j = 0; j < 64; ++j) {
      oled.setPixel(i, j, 0);
    }
  }
}

void drawBasket(int x) {
  int y = 58;
  for(int i = 0; i < 30; ++i) {
    oled.setPixel(i%6 + x, y, gfxBasket[i]);
    if(i%6 == 5) {
      ++y;
    }
  }
}

void drawFruit(int x, int y, int fruit){
  for(int i = 0; i < 36; ++i) {
    oled.setPixel(i%6 + x, y, gfxFruit[fruit][i]);
    if(i%6 == 5) {
      ++y;
    }
  }
}

void fruitPlusSignal() {
  for(int i = 0; i < 20; ++i) {
    digitalWrite(24, HIGH);
    
    if (i >= 0 && i < 10)
      delay(2);
    else if (i >= 10 && i < 20) 
      delay(7);
      
    digitalWrite(24, LOW); 
  }
}

