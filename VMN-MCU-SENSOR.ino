#include <Task.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "./modules/VmnSensors/water.h"
#include "./modules/VmnSensors/ec.h"
#include "./modules/VmnSensors/LoadCell.h"


int station = 0;
#include "./modules/wifi.h"

TaskManager taskManager;

void setup(){
    lcd.init();
    lcd.backlight();
    Serial.begin(115200);
    taskManager.StartTask(WaterSensor::instance());
    taskManager.StartTask(ECSensor::instance());
    taskManager.StartTask(LoadCell::instance());
    taskManager.StartTask(VmnClient::instance());
}
void loop(){
    taskManager.Loop();
}
