#include <Task.h>
#include <HX711_ADC.h>
#define TBT 25
//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell(18, 19);
extern TaskManager taskManager;
class LoadCellSensor : public Task
{
  public:
    static LoadCellSensor *s_instance;
    LoadCellSensor() : Task(MsToTaskTime(100))
    {
        pinMode(25,INPUT);
        volume = 0;
        Serial.println("Wait...");
        LoadCell.begin();
        long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
        LoadCell.start(stabilisingtime);
        LoadCell.setCalFactor(696.0/2); // user set calibration factor (float)
        Serial.println("Startup + tare is complete");
    }
    static LoadCellSensor *instance()
    {
        if (!s_instance)
            s_instance = new LoadCellSensor;
        return s_instance;
    }

    float getVal()
    {
        return volume;
    }

    void tareZero(){
        Serial.println("Tare Zero");
        LoadCell.tareNoDelay();
    }

  private:
    float volume;
    unsigned long ct = 0;
    unsigned long bt = 0;
    virtual bool OnStart() { return true; }
    virtual void OnUpdate(uint32_t delta_time)
    {
        LoadCell.update();
        ct += delta_time;
        if(ct >= 1000){
            volume = getLoadCellSensor();
            ct = 0;
        }
    }


    float getLoadCellSensor()
    {
        float val = LoadCell.getData();
        return (val < 0)? val*(-1):val;
    }
};
LoadCellSensor *LoadCellSensor::s_instance = 0;