#include <Adafruit_LEDBackpack.h>

#define SECONDS_BUTTON 5	// 10秒追加ボタン
#define MINUTES_BUTTON 4	// 1分追加ボタン
#define RESET_BUTTON 3 		// リセットボタン
#define START_STOP_BUTTON 2 // スタート/ストップボタン
#define RED_LED 6 			// 赤色LED
#define BLUE_LED 7 			// 青色LED
#define GREEN_LED 8 		// 緑色LED
#define BUZZER 9 			// ブザー

// 7セグメントディスプレイ
Adafruit_7segment matrix = Adafruit_7segment();

int State = 0; 		  	// 7セグメントディスプレイの状態(時間)
int Adjust = 0;			// 調整用
bool DrawDots = false;  // 小数点表示
bool Seconds = false; 	// 10秒追加ボタンの状態
bool Minutes = false; 	// 1分追加ボタンの状態
bool Reset = false; 	// リセットボタンの状態
bool StartStop = false; // スタート/ストップボタンの状態

void setup()
{
  // 入力(秒, 分, リセット, スタート/ストップ)
  pinMode(SECONDS_BUTTON, INPUT_PULLUP);
  pinMode(MINUTES_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(START_STOP_BUTTON, INPUT_PULLUP);

  // 出力(LED, ブザー, 7セグメントディスプレイ)
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  matrix.begin(0x70);
  matrix.clear();
  matrix.writeDisplay();
  randomSeed(analogRead(0));
}

void loop()
{
  int BuzzerState = random(0, 5); // 0から1(曲の数)までの乱数を生成
  
  // 10秒追加
  if (digitalRead(SECONDS_BUTTON) == LOW) {
    tone(BUZZER, 2000);
    delay(30);
    noTone(BUZZER);
    if (State <= 3589) State += 10; 
    else State = 3599;
    while (digitalRead(SECONDS_BUTTON) == LOW);
  }
  
  // 1分追加
  if (digitalRead(MINUTES_BUTTON) == LOW) {
    tone(BUZZER, 2000);
    delay(30);
    noTone(BUZZER);
    if (State <= 3539) State += 60; 
    else State = 3599;
    while (digitalRead(MINUTES_BUTTON) == LOW);
  }
  
  // リセット
  if (digitalRead(RESET_BUTTON) == LOW) {
    tone(BUZZER, 2000);
    delay(30);
    noTone(BUZZER);
  	State = 0;
    Adjust = 0;
    analogWrite(RED_LED, 0);
    analogWrite(BLUE_LED, 0);
    analogWrite(GREEN_LED, 0);
    StartStop = false;
    while (digitalRead(RESET_BUTTON) == LOW);
  }
  
  // スタート/ストップ
  if (digitalRead(START_STOP_BUTTON) == LOW) {
    tone(BUZZER, 2000);
    delay(30);
    noTone(BUZZER);
    
    if (StartStop) { // ストップ
      StartStop = false;
      Adjust = 0;
    } else if (State) { // スタート
      StartStop = true;
    }
    
    while (digitalRead(START_STOP_BUTTON) == LOW);
  }

  // 起動状態
  if (StartStop && State) {
   	Adjust = (Adjust + 1) % 10; // 調整用
    
    if (!Adjust) {
      State--; // カウントダウン
      
  	  if (State == 0){ // 終了状態
        matrix.writeDigitNum(0, (State / 60) / 10, DrawDots);
  		matrix.writeDigitNum(1, (State / 60) % 10, DrawDots);
  	    matrix.drawColon(!DrawDots);
 		matrix.writeDigitNum(3, (State % 60) / 10, DrawDots);
 	    matrix.writeDigitNum(4, (State % 60) % 10, DrawDots); 
 		matrix.writeDisplay();
      	analogWrite(RED_LED, 0);
      	analogWrite(BLUE_LED, 0);
      	analogWrite(GREEN_LED, 0);
        
        // ランダムに曲を選択
        if (BuzzerState == 0) Twinkle_Star();
        else if (BuzzerState == 1) Working_Railway();
        else if (BuzzerState == 2) London_Bridge();
        else if (BuzzerState == 3) Westmister_Quarters();
        else if (BuzzerState == 4) Mary_had_a_little_lamb();
        
      	StartStop = false;
  	  } else if (State <= 10) { // 残り10秒
      	analogWrite(RED_LED, 255);
        analogWrite(BLUE_LED, 0);
     	analogWrite(GREEN_LED, 0);
  	  } else if (State <= 30) { // 残り30秒
      	analogWrite(RED_LED, 255);
      	analogWrite(BLUE_LED, 0);
      	analogWrite(GREEN_LED, 255);
  	  } else { 				  // 通常状態
      	analogWrite(RED_LED, 0);
      	analogWrite(BLUE_LED, 255);
      	analogWrite(GREEN_LED, 0);      
  	  }
    }
  }

  // 7セグメントディスプレイの状態を更新(動作中に下のドットを点滅)
  matrix.writeDigitNum(0, (State / 60) / 10, (State + 1) % 5 ? DrawDots : !DrawDots);
  matrix.writeDigitNum(1, (State / 60) % 10, (State + 2) % 5 ? DrawDots : !DrawDots);
  matrix.drawColon(!DrawDots);
  matrix.writeDigitNum(3, (State % 60) / 10, (State + 3) % 5 ? DrawDots : !DrawDots);
  matrix.writeDigitNum(4, (State % 60) % 10, (State + 4) % 5 ? DrawDots : !DrawDots); 
  matrix.writeDisplay();
  
  delay(100); // 1秒待機
}

// ドレミファソラシド
const int Scale[] = {523.251, 587.330, 659.255, 698.456, 783.991, 880, 987.767, 1046.502};

// 音を出す関数
void playTone(String note, int duration, int pause)
{
  int index;

  if (note == "ド") index = 0;
  else if (note == "レ") index = 1;
  else if (note == "ミ") index = 2;
  else if (note == "ファ") index = 3;
  else if (note == "ソ") index = 4;
  else if (note == "ラ") index = 5;
  else if (note == "シ") index = 6;
  else if (note == "高ド") index = 7;

  int frequency = Scale[index];
  tone(BUZZER, frequency);
  delay(duration);
  noTone(BUZZER);
  delay(pause);
}

// きらきら星
void Twinkle_Star() {
  playTone("ド", 500, 100);
  playTone("ド", 500, 100);
  playTone("ソ", 500, 100);
  playTone("ソ", 500, 100);
  playTone("ラ", 500, 100);
  playTone("ラ", 500, 100);
  playTone("ソ", 500, 600);

  playTone("ファ", 500, 100);
  playTone("ファ", 500, 100);
  playTone("ミ", 500, 100);
  playTone("ミ", 500, 100);
  playTone("レ", 500, 100);
  playTone("レ", 500, 100);
  playTone("ド", 500, 0);
}

// 線路は続くよどこまでも
void Working_Railway() {
  playTone("ファ", 750, 100);
  playTone("ド", 200, 100);
  playTone("ファ", 400, 100);
  playTone("ド", 150, 100);
  playTone("ファ", 450, 100);
  playTone("ソ", 150, 100);
  playTone("ラ", 900, 200);
  playTone("ファ", 900, 200);
  
  playTone("シ", 750, 100);
  playTone("シ", 250, 100);
  playTone("ファ", 500, 100);
  playTone("ソ", 500, 100);
  playTone("ラ", 1250, 0);
}

// ロンドン橋
void London_Bridge() {
  playTone("ソ", 700, 100);
  playTone("ラ", 200, 100);
  playTone("ソ", 500, 100);
  playTone("ファ", 500, 100);
  playTone("ミ", 500, 100);
  playTone("ファ", 500, 100);
  playTone("ソ", 500, 600);
  playTone("レ", 500, 100);
  playTone("ミ", 500, 100);
  playTone("ファ", 500, 500);
  playTone("ミ", 500, 100);
  playTone("ファ", 500, 100);
  playTone("ソ", 500, 800);
  playTone("ソ", 700, 100);
  playTone("ラ", 200, 100);
  playTone("ソ", 500, 100);
  playTone("ファ", 500, 100);
  playTone("ミ", 500, 100);
  playTone("ファ", 500, 100);
  playTone("ソ", 500, 300);
  playTone("レ", 700, 100);
  playTone("ソ", 450, 100);
  playTone("ミ", 200, 100);
  playTone("ド", 500, 0);
}

// ウェストミンスターの鐘
void Westmister_Quarters() {
  playTone("ファ", 500, 100);
  playTone("ラ", 500, 100);
  playTone("ソ", 500, 100);
  playTone("ド", 800, 500);
  playTone("ファ", 500, 100);
  playTone("ソ", 500, 100);
  playTone("ラ", 500, 100);
  playTone("ファ", 800, 500);
  playTone("ラ", 500, 100);
  playTone("ファ", 500, 100);
  playTone("ソ", 500, 100);
  playTone("ド", 800, 500);
  playTone("ド", 500, 100);
  playTone("ソ", 500, 100);
  playTone("ラ", 500, 100);
  playTone("ファ", 800, 500);
}

//メリーさんの羊
void Mary_had_a_little_lamb() {
  playTone("ミ", 500, 100);
  playTone("レ", 200, 100);
  playTone("ド", 300, 100);
  playTone("レ", 300, 100);
  playTone("ミ", 300, 100);
  playTone("ミ", 300, 100);
  playTone("ミ", 300, 300);
  playTone("レ", 300, 100);
  playTone("レ", 300, 100);
  playTone("レ", 300, 300);
  playTone("ミ", 300, 100);
  playTone("ソ", 300, 100);
  playTone("ソ", 300, 500);
  playTone("ミ", 500, 100);
  playTone("レ", 200, 100);
  playTone("ド", 300, 100);
  playTone("レ", 300, 100);
  playTone("ミ", 300, 100);
  playTone("ミ", 300, 100);
  playTone("ミ", 300, 100);
  playTone("ミ", 300, 100);
  playTone("レ", 300, 100);
  playTone("レ", 300, 100);
  playTone("ミ", 500, 100);
  playTone("レ", 200, 100);
  playTone("ド", 300, 100);
}