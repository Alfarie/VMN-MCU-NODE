#include <Task.h>
extern TaskManager taskManager;
#define EC_SENSOR A0
class ECSensor : public Task
{
public:
  static ECSensor *s_instance;
  ECSensor() : Task(MsToTaskTime(100)){
    ec = 0;
  };
  static ECSensor *instance()
  {
    if (!s_instance)
      s_instance = new ECSensor;
    return s_instance;
  }

  float GetRaw()
  {
    return aVal;
  }
  float GetEC(){
    return ec;
  }

private:
  float aVal,ec;
  int analogReadVal[30];
  int readIndex = 0;
  virtual bool OnStart()
  {
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    int val = analogRead(EC_SENSOR);
    analogReadVal[readIndex] = val;
    readIndex++;

    if (readIndex >= 20)
    {
      aVal = 0;
      qsort(analogReadVal, readIndex, sizeof(analogReadVal[0]), sort_desc);
      int num = 0;
      for (int i = 2; i < readIndex - 2; i++)
      {
        aVal += analogReadVal[i];
        num++;
      }
      aVal = aVal / num;
      aVal = aVal * 5000 / 1024;
      readIndex = 0;

      float TempCoefficient = 1.0 + 0.0185 * (WaterSensor::instance()->GetWater() - 25.0);
      float CoefficientVolatge = (float)aVal / TempCoefficient;

      if (CoefficientVolatge <= 448)
        ec = 6.84 * CoefficientVolatge - 64.32; //1ms/cm<EC<=3ms/cm
      else if (CoefficientVolatge <= 1457)
        ec = 6.98 * CoefficientVolatge - 127; //3ms/cm<EC<=10ms/cm
      else
        ec = 5.3 * CoefficientVolatge + 2278;
      ec = ec/1000;
      
      if(ec < 0) ec = 0;

    }
  }

  static int sort_desc(const void *cmp1, const void *cmp2)
  {
    // Need to cast the void * to int *
    int a = *((int *)cmp1);
    int b = *((int *)cmp2);
    // The comparison
    return a < b ? -1 : (a > b ? 1 : 0);
    // A simpler, probably faster way:
    //return b - a;
  }
};
ECSensor *ECSensor::s_instance = 0;