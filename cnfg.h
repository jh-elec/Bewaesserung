/*
 *	cnfg.h
 *         ██╗   ██╗  ██╗      ███████╗██╗     ███████╗ ██████╗
 *         ██║   ██║  ██║      ██╔════╝██║     ██╔════╝██╔════╝
 *         ██║   ███████║█████╗█████╗  ██║     █████╗  ██║
 *    ██   ██║   ██╔══██║╚════╝██╔══╝  ██║     ██╔══╝  ██║
 *    ╚█████╔╝██╗██║  ██║██╗   ███████╗███████╗███████╗╚██████╗██╗
 *     ╚════╝ ╚═╝╚═╝  ╚═╝╚═╝   ╚══════╝╚══════╝╚══════╝ ╚═════╝╚═╝
 *   	Software freely available. Software for fun ;)!
 */

#ifndef __CNFG_H__
#define __CNFG_H__


//#define SERIAL_OUTPUT


typedef struct
{
  uint16_t  uiADOffset;
  uint16_t  uiInEarth;
  uint16_t  uiInWater;
}Sensor_TypeDef;


Sensor_TypeDef Sensor;
Sensor_TypeDef SensorCalibration;


typedef struct
{
  int16_t iPercentFrom;
  int16_t iPercentTo;
  bool  bState;
}Action_TypeDef;


/*
 * Hier kann festgelegt werden ab wann bewaessert werden soll!
 */
Action_TypeDef Action[] =
{
  /*
   * Schwellwerte zum Schalten
   * z.B { von , bis , an[true] oder aus[false] }
   */
  { 0,    20,   true},
  { 20,   40,   true},
  { 40,   60,   true},
  { 60,   100,  false},
};


#endif /* __CNFG_H__ */