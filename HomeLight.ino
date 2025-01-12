#include <ArduinoBLE.h>
#include <IRremote.hpp>

// マクロ定義
#define IR_SEND_PIN 4

// 関数プロトタイプ宣言
void notifyError(int ledPinNum = LED_BUILTIN, int intervalTime = 1000);
void notifySetUpSuccess(int ledPinNum = LED_BUILTIN, int loopNum = 5, int intervalTime = 100);
void centralConnected(int ledPinNum = LED_BUILTIN);
void centralDisconnected(int ledPinNum = LED_BUILTIN);

// BLEサービスとキャラクタリスティックのUUID
BLEService lightControlService("1815");
BLEIntCharacteristic lightState("2A37", BLERead | BLEWrite); // 0x2A37を使用してライトの状態を送受信


void setup() {
  Serial.begin(9600);
  while (!Serial); // シリアル通信が開始されるまで待機

  // BLEの初期化
  if (!BLE.begin()) {
    notifyError();
    while(1);
  }

  // デバイス名とサービス名の設定
  BLE.setLocalName("HomeLight");
  lightControlService.addCharacteristic(lightState);
  BLE.addService(lightControlService);

  // 赤外線送信準備
  IrSender.begin(IR_SEND_PIN, DISABLE_LED_FEEDBACK, 0);

  // セットアップ成功を通知
  notifySetUpSuccess();

  // BLEのアドバタイズ開始
  BLE.advertise();
}


void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    // セントラルデバイスが接続されたらLEDを点灯
    centralConnected();

    while (central.connected()) {
      if (lightState.written()) {
        int value = lightState.value();
        // 受信した値に応じて赤外線信号を送信
        switch (value) {
          case 0:
            IrSender.sendNEC(0x459A, 0x4, 0);
            break;
          case 1:
            IrSender.sendNEC(0x459A, 0x48, 0);
            break;
          case 2:
            IrSender.sendNEC(0x459A, 0x88, 0);
            break;
          case 3:
            IrSender.sendNEC(0x459A, 0x21, 0);
            break;
          case 4:
            IrSender.sendNEC(0x459A, 0x22, 0);
            break;
          case 5:
            IrSender.sendNEC(0x459A, 0x23, 0);
            break;
          case 6:
            IrSender.sendNEC(0x459A, 0xC4, 0);
            break;
          case 7:
            IrSender.sendNEC(0x459A, 0x44, 0);
            break;
          default:
            break;
        }
      }
    }

    centralDisconnected();
  }
}

void notifyError(int ledPinNum, int intervalTime) {
  // ArduinoのLEDを点滅させる
  while (true) {
    digitalWrite(ledPinNum, HIGH);
    delay(intervalTime);
    digitalWrite(ledPinNum, LOW);
    delay(intervalTime);
  }
}

void notifySetUpSuccess(int ledPinNum, int loopNum, int intervalTime) {
  // ArduinoのLEDを5回点滅させる
  for (int i = 0; i < loopNum; i++) {
    digitalWrite(ledPinNum, HIGH);
    delay(intervalTime);
    digitalWrite(ledPinNum, LOW);
    delay(intervalTime);
  }
}

void centralConnected(int ledPinNum) {
  // ArduinoのLEDを点灯させる
  digitalWrite(ledPinNum, HIGH);
}

void centralDisconnected(int ledPinNum) {
  // ArduinoのLEDを消灯させる
  digitalWrite(ledPinNum, LOW);
}
