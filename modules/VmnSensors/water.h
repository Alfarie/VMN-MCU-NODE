#include <Task.h>

#define StartConvert 0
#define ReadTemperature 1

extern TaskManager taskManager;

// OneWire ds(A2);
// DallasTemperature wtSensor(&ds);

class WaterSensor : public Task
{
public:
  static WaterSensor *s_instance;
  WaterSensor() : Task(MsToTaskTime(2000)){
    // wtSensor.begin();
  };
  static WaterSensor *instance()
  {
    if (!s_instance)
      s_instance = new WaterSensor;
    return s_instance;
  }

  float GetWater(){
    return _water;
  }

private:
  float _water;
  uint32_t _timer;
  byte data[12];
  byte addr[8];
  float TemperatureSum;
  bool isRequest = true;
  virtual bool OnStart()
  {
    _water = 25;
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    // float wt = wtSensor.getTempCByIndex(0);
    // _water = (wt <= 40 && wt >= 0 )?wt:25;
    // wtSensor.requestTemperatures();
    _water = 25;
  }
};
WaterSensor *WaterSensor::s_instance = 0;