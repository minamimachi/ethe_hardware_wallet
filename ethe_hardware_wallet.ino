#include <EEPROM.h>
/**
* FA***:アドレス  ,FL***:サイズ
* |アドレス|プライベートキー|パスワード|チェック|
* |FAAddress
*          |FAPrivate
*                           |FAPass
*                                      |FAInitChek
*                                               |FAEND
*/
#define FLAddress 34   //アドレス
#define FLPrivate 51   //秘密鍵
#define FLPass 6       //パスワード
#define FLInitChek 1   //初期化の確認

#define FAAddress 0
#define FAPrivate (FAAddress+FLAddress)
#define FAPass (FAPrivate+FLPrivate)
#define FAInitChek (FAPass+FLPass)
#define FAEND (FAInitChek+FLInitChek)


int Seq = 0;
String Adrs;
String Prky;
String Pswd;


void setup() {
Serial.begin(38400); /*dpsは文字化けの原因 随時変更*/
  while (!Serial) {
  }
initialize(); /*初回起動時実行 //initialize();*/
  initialcheck();
  initialMes();
}


void loop() {
  switch (Seq) {
    case 0:
      while (Serial.available() == 0) {
      }
      switch (Serial.read()) {
        case '1':
          Serial.println(F("アドレスを表示します"));
          Serial.println(F("アドレスは"));
          Serial.println(EEPROMread(FAAddress, FLAddress));
          initialMes2();
          break;
        case '2':
          Seq = 1;
          break;
        case '3':
          Seq = 2;
          break;
      }
      break;
    case 1:
      Serial.println(F("秘密鍵を表示します"));
      if (isPaswdCorrect(EEPROMread(FAPass, FLPass)) == true) {
        Serial.println(F("秘密鍵は"));
        Serial.println(EEPROMread(FAPrivate, FLPrivate));
      }
      Seq = 0;
      initialMes2();
      break;
    case 2:
      Serial.println(F("各種設定"));
      if (isPaswdCorrect(EEPROMread(FAPass, FLPass)) == true) {
        Serial.println(F("1.アドレスと秘密鍵を設定します\n2.パスワードを設定します\n3.初期化します\n4.戻る"));
        while (Serial.available() == 0) {
        }
        switch (Serial.read()) {
          case '1':
            char inAdrs[34];
            char inPrky[51];
            Adrs = "";
            Prky = "";
            Serial.println(F("アドレスと秘密鍵を設定します"));
            Serial.println(F("新しいアドレスを入力してください"));
            while (Serial.available() == 0) {
            }
            delay(500);
            for (int i = 0; i <= 33; i++) {
              inAdrs[i] = Serial.read();
            }
            for (int i = 0; i <= 33; i++) {
              Adrs.concat(inAdrs[i]);
            }
            Serial.println(F("新しい秘密鍵を入力してください"));
            while (Serial.available() == 0) {
            }
            delay(500);
            for (int i = 0; i <= 50; i++) {
              inPrky[i] = Serial.read();
            }
            for (int i = 0; i <= 50; i++) {
              Prky.concat(inPrky[i]);
            }
            EEPROMwrite(FAAddress, Adrs);
            EEPROMwrite(FAPrivate, Prky);
            Serial.println(F("アドレスと秘密鍵の設定が完了しました"));
            Seq = 0;
            initialMes2();
            break;
          case '2':
            Serial.println(F("パスワードを設定します"));
            Serial.println(F("新しいパスワードを設定してください\n文字数 6 文字以上\n半角英数字 0-9 A-Z a-z が使えます"));
            char inPswd[] = Serial.read();
            Pswd = "";
            while (Serial.available() == 0) {
            }
            delay(500);
            for (int i = 0; i <= sizeof(inPswd); i++) {
              inPswd[i] = Serial.read();
            }
            for (int i = 0; i <= 5; i++) {
              Pswd.concat(inPswd[i]);
            }
            EEPROMwrite(FAPass, Pswd);
            Serial.println(F("パスワードの設定が完了しました"));
            Seq = 0;
            initialMes2();
            break;
          case '3':
            Serial.println(F("初期化しますか?\n1.いいえ\n2.はい"));
            while (Serial.available() == 0) {
            }
            switch (Serial.read()) {
              case '1':
                Serial.println(F("初期化をキャンセルしました"));
                Seq = 0;
                initialMes2();
                break;
              case '2':
                Serial.println(F("本当に初期化しますか?\n1.いいえ\n2.はい"));
                while (Serial.available() == 0) {
                }
                switch (Serial.read()) {
                  case '1':
                    Serial.println(F("初期化をキャンセルしました"));
                    Seq = 0;
                    initialMes2();
                    break;
                  case '2':
                    Serial.println(F("初期化中です..."));
                    initialize();
                    Serial.println(F("初期化が完了しました"));
                    Seq = 0;
                    initialMes2();
                    break;
                }
                break;
            }
            break;
          case '4':
            Seq = 0;
            initialMes2();
            break;
        }
        break;
      } else {
        initialMes2();
        Seq = 0;
      }
      break;
  }
}

void initialcheck() {
  String Intsum = "1";
  if (EEPROMread(FAInitChek, FLInitChek) != Intsum) {
    initialize();
  }
}


void initialize() {
  String IntAdrs = "初期アドレスXXXXXXXXXXXXXXXXXXXX";
  String IntPrky = "初期秘密鍵XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  String IntPswd = "000000";
  String Intsum = "1";
  EEPROMwrite(FAAddress, IntAdrs);
  EEPROMwrite(FAPrivate, IntPrky);
  EEPROMwrite(FAPass, IntPswd);
  EEPROMwrite(FAInitChek, Intsum);
}


void initialMes() {
  initialMes1();
  initialMes2();
}


void initialMes1() {
  Serial.println(F("こんにちは。Ethereum ハードウェアウォレットです"));
}


void initialMes2() {
  Serial.println(F("《 MENU 》\n1.アドレスを表示します\n2.秘密鍵を表示します\n3.設定"));
}


boolean isPaswdCorrect(String inpswd) {
  boolean correct;
  char key[];
  String Pswdinput;
  Serial.println(F("パスワードを入力してください"));
  while (Serial.available() == 0) {
  }
  delay(500);
  for (int i = 0; i < sizeof(key); i++) {
    key[i] = Serial.read();
  }
  for (int i = 0; i < sizeof(key); i++) {
    Pswdinput.concat(key[i]);
  }
  Serial.println(F("/********************/"));
  Serial.println(Pswdinput);
  Serial.println(inpswd);
  Serial.println(F("/********************/"));
  if (Pswdinput == inpswd) {
    Serial.println(F("パスワードが認証されました"));
    correct = true;
  } else {
    Serial.println(F("パスワードが間違っています"));
    correct = false;
  }
  return correct;
}


String EEPROMread (int adrs, int lng) {
  String readout;
  for (int i = 0; i < lng; i++) {
    readout.concat(char(EEPROM.read(adrs + i)));
  }
  return readout;
}


void EEPROMwrite (int adrs, String writein) {
  int lng = writein.length();
  for (int i = 0; i < lng; i++) {
    EEPROM.write(i + adrs,  writein.charAt(i));
  }
}