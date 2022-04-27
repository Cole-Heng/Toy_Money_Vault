
#include <LiquidCrystal.h>
#include <Servo.h>

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
const int BTTN0 = 2;
const int BTTN1 = 10;
const int BTTN2 = 11;
const int BTTN3 = 12;
const int TILT = A0;
const int BUZZER = 13;
const int SERVO_PIN = 3;

const int SET_PSWD = 0;
const int LOCKED = 1;
const int UNLOCKED = 2;

int sys_state = SET_PSWD;
int password[] = {-1, -1, -1, -1};
int pass_len = 0;
int guess[] = {-1, -1, -1, -1};
int guess_len = 0;
int servo_pos = 0;

bool pass_updated = false;
bool alarm_tripped = false;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo lock;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("begin");
  delay(1000);
  lcd.clear();
  pinMode(13, OUTPUT);
  pinMode(BTTN0, INPUT);
  pinMode(BTTN1, INPUT);
  pinMode(BTTN2, INPUT);
  pinMode(BTTN3, INPUT);
  pinMode(TILT, INPUT);
  Serial.begin(9600);

  lock.attach(SERVO_PIN);
  lock.write(servo_pos);
}

void loop() {
  
  int state_0 = read_button_state(BTTN0);
  int state_1 = read_button_state(BTTN1);
  int state_2 = read_button_state(BTTN2);
  int state_3 = read_button_state(BTTN3);
  //digitalWrite(13, LOW);
  switch (sys_state) {
    case SET_PSWD:
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
      Serial.println(pass_len);
      if (state_0 + state_1 + state_2 + state_3 == 1 && pass_updated == false) {
        if (state_0 == HIGH) {
          password[pass_len] = 0;
          pass_len++;
        } else if (state_1 == HIGH) {
          password[pass_len] = 1;
          pass_len++;
        } else if (state_2 == HIGH) {
          password[pass_len] = 2;
          pass_len++;
        } else if (state_3 == HIGH) {
          password[pass_len] = 3;
          pass_len++;
        }
        pass_updated = true;
      } else if (state_0 + state_1 + state_2 + state_3 == 0) {
        pass_updated = false;
      }
      
      lcd.setCursor(0, 1);
      for (int i = 0; i < pass_len; i++) {
        if (password[i] != -1) {
          lcd.print(password[i]);
        }
        Serial.print(password[i]);
      }
      if (pass_len == 4) {
        lcd.setCursor(0, 0);
        lcd.print("Your Password Is");
        delay(1000);
        lock_safe();
        sys_state = LOCKED;
        lcd.clear();
      }
      break;
      
    case LOCKED:
      lcd.setCursor(0, 0);
      lcd.print("LOCK enter pswd:");
      
      if (read_button_state(TILT) == HIGH) {
        alarm_tripped = true;
      }
      if (alarm_tripped == true) {
        tone(BUZZER, 750, 50);
      }
      
      if (state_0 + state_1 + state_2 + state_3 == 1 && pass_updated == false) {
        if (state_0 == HIGH) {
          guess[guess_len] = 0;
          guess_len++;
        } else if (state_1 == HIGH) {
          guess[guess_len] = 1;
          guess_len++;
        } else if (state_2 == HIGH) {
          guess[guess_len] = 2;
          guess_len++;
        } else if (state_3 == HIGH) {
          guess[guess_len] = 3;
          guess_len++;
        }
        pass_updated = true;
      } else if (state_0 + state_1 + state_2 + state_3 == 0) {
        pass_updated = false;
      }
      
      lcd.setCursor(0, 1);
      for (int i = 0; i < guess_len; i++) {
        if (guess[i] != -1) {
          lcd.print(guess[i]);
        }
        Serial.print(guess[i]);
      }
      if (guess_len == 4) {
        lcd.setCursor(0, 0);
        if (pass_match() == true){
          lcd.clear();
          lcd.print("PASS correct");
          tone(BUZZER, 1000, 500);
          sys_state = UNLOCKED;
          alarm_tripped = false;
          unlock_safe();
        } else {
          lcd.print("PASS incorrect  ");
          tone(BUZZER, 500, 500);
        }
        guess[0] = -1;
        guess[1] = -1;
        guess[2] = -1;
        guess[3] = -1;
        guess_len = 0;
        delay(1000);
        lcd.clear();
      }
      break;
    case UNLOCKED:
      lcd.setCursor(0,0);
      lcd.print("UNLOCK press any");
      lcd.setCursor(0,1);
      lcd.print("to lock");
      if (state_0 + state_1 + state_2 + state_3 >= 1) {
        sys_state = LOCKED;
        lock_safe();
        lcd.clear();
      }
  }
  
}

int read_button_state(int bttn) {
  int state = digitalRead(bttn);

  if (state == HIGH) {
    delay(50);
    state = digitalRead(bttn);
    if (state == HIGH) {
      //digitalWrite(13, HIGH);
      return 1;
    }
  }
  return 0;
}

bool pass_match() {
  for (int i = 0; i < 4; i++) {
    if (password[i] != guess[i]) {
      return false;
    }
  }
  return true;
}

void lock_safe() {
  while(servo_pos <= 180) { 
    lock.write(servo_pos);
    delay(5);
    servo_pos++;
  }
}

void unlock_safe() {
  while(servo_pos > 0) { 
    lock.write(servo_pos);
    delay(5);
    servo_pos--;
  }
}
