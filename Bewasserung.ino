#include <EEPROM.h>
#include "cnfg.h"


void setup() 
{
  Serial.begin( 9600 );
  analogReference( DEFAULT );
  pinMode( LED_BUILTIN, OUTPUT );
  
  //CalibrateSensor( &SensorCalibration );

  delay( 500 );
  LoadSensorCalibrationValues( &Sensor );
}


void loop() 
{
  uint16_t  Moist = GetMoistRAW();
  int16_t   MoistInPercent = map( Moist, Sensor.uiInWater, Sensor.uiInEarth, 100, 0 );
  uint8_t   i = 0;

  if ( MoistInPercent < 0 ) 
  {
    MoistInPercent = 0;
  }else if ( MoistInPercent >= 93 ) // Toleranzen kompensieren..
  {
    MoistInPercent = 100;
  }

  for ( i = 0 ; i < ( sizeof( Action ) / sizeof( Action[0] ) ) ; i++ )
  {
    if ( MoistInPercent >= Action[i].iPercentFrom && MoistInPercent <= Action[i].iPercentTo ) 
    {
      SetRelais( Action[i].bState );
    }
  }

  #ifdef SERIAL_OUTPUT
    Serial.println( "********************************" );
    Serial.print( "A/D Wert:= " );
    Serial.print( Moist , DEC );
    Serial.println( " Digits" );
    
    Serial.print( "Feuchte:= " );
    Serial.print( MoistInPercent , DEC );
    Serial.println( "%" );

    Serial.println( "********************************" );
  #endif
  
  delay(2000);
}          


uint16_t GetMoistRAW()
{ 
  return analogRead(A0);
}

void SetRelais( bool State ) 
{
  if ( State )
  {
    digitalWrite( LED_BUILTIN, LOW );     
  }
  else
  {
    digitalWrite( LED_BUILTIN, HIGH );
  }
}

void SignalNextStep( void )
{
  uint8_t i = 0;

  for( i = 0 ; i < 10 ; i++ )
  {
    digitalWrite( LED_BUILTIN, LOW );                                 
    delay(25);                      
    digitalWrite( LED_BUILTIN, HIGH );                               
    delay(500);
  }
}


uint8_t CalibrateSensor( Sensor_TypeDef *SensorCal )
{
  Sensor_TypeDef Cal;
  uint32_t uiAVG = 0;
  uint8_t i = 0;

  delay(250);

  SignalNextStep;
  
  #ifdef SERIAL_OUTPUT
    Serial.println( "Calibration started.." );
    Serial.print( "[1] : " );
    Serial.println( "Please Disconnect the Sensor from Board.." );
  #endif
  
  delay(5000);
  
  for( i = 0 ; i < 20 ; i++ )
  {
    uiAVG += analogRead( A0 );
  
  #ifdef SERIAL_OUTPUT
    Serial.print( "\t[" );
    Serial.print( 20-i,DEC );
    Serial.print( "]: " );
    Serial.print( uiAVG, DEC );
    Serial.print( "\r\n" );
  #endif
  
    delay(500);
  }
  
  Cal.uiADOffset = ( uiAVG / i );
  uiAVG = 0;

  #ifdef SERIAL_OUTPUT
    Serial.print( " A/D Offset: " );
    Serial.print( "\t" );
    Serial.println( Cal.uiADOffset, DEC );
  #endif
  
  SignalNextStep();

  #ifdef SERIAL_OUTPUT
    Serial.print( "[2] : " );
    Serial.println( "Put Moist Sensor into Air.." );
  #endif
  
  delay(5000);
  
  /*
   * Low Value := put Sensor into Earth..
   * You have 10 secounds.. let´s go
   * ~865
   */
  digitalWrite( LED_BUILTIN, HIGH ); 
  
  for( i = 0 ; i < 20 ; i++ )
  {
    uiAVG += ( analogRead( A0 ) - Cal.uiADOffset );
  
  #ifdef SERIAL_OUTPUT
    Serial.print( "\t[" );
    Serial.print( 20-i,DEC );
    Serial.print( "]: " );
    Serial.println( uiAVG, DEC );
  #endif 
  
    delay( 500 );
  }
  
  Cal.uiInEarth = ( uiAVG / i );
  uiAVG = 0;
  
  #ifdef SERIAL_OUTPUT
    Serial.print( "\tSensor in Air: " );
    Serial.println( Cal.uiInEarth, DEC );
  #endif
  
  SignalNextStep();
 
  #ifdef SERIAL_OUTPUT
    Serial.print( "[3] : " );
    Serial.println( "Put Moist Sensor into Water.." );
  #endif
  
  delay(5000);
  
  /*
   * High Value := put Sensor into Water..
   * You have 10 secounds.. let´s go
   * ~450
   */
  digitalWrite( LED_BUILTIN, LOW ); 
  for( i = 0 ; i < 20 ; i++ )
  {
    uiAVG += ( analogRead( A0 ) - Cal.uiADOffset );
  
  #ifdef SERIAL_OUTPUT
    Serial.print( "\t[" );
    Serial.print( 20-i,DEC );
    Serial.print( "]: " );
    Serial.println( uiAVG, DEC );
  #endif
  
    delay(500);
  }
  
  Cal.uiInWater = ( uiAVG / i );
  uiAVG = 0;
  
  #ifdef SERIAL_OUTPUT
    Serial.print( "\t Sensor in Water: " );
    Serial.println( Cal.uiInWater, DEC );
  #endif
  
  /*
   * Calibration End
   */
  SignalNextStep();
  
  #ifdef SERIAL_OUTPUT
    Serial.println( "Calibration end.." );
  #endif

  /*
   * Write Calibration Values 
   */

  uint16_t EEPAddress = 0;
  EEPROM.begin(512);
  EEPROM.put( EEPAddress, Cal.uiADOffset );
  EEPAddress += sizeof( uint16_t );
  EEPROM.put( EEPAddress, Cal.uiInEarth );  
  EEPAddress += sizeof( uint16_t );
  EEPROM.put( EEPAddress, Cal.uiInWater );
  EEPROM.commit();
  EEPROM.end();
  
  SensorCal->uiInEarth = Cal.uiInEarth;
  SensorCal->uiInWater = Cal.uiInWater;

  return 0;
}

void LoadSensorCalibrationValues( Sensor_TypeDef *Sensor )
{
  uint16_t EEPAddress = 0;
  
  EEPROM.begin(512);
  EEPROM.get( EEPAddress,Sensor->uiADOffset );
  EEPAddress += sizeof( uint16_t );
  EEPROM.get( EEPAddress,Sensor->uiInEarth );
  EEPAddress += sizeof( uint16_t );
  EEPROM.get( EEPAddress,Sensor->uiInWater );
  EEPROM.end();

  #ifdef SERIAL_OUTPUT
  Serial.print("\tAD_Offset: " );
  Serial.println( Sensor->uiADOffset, DEC );

  Serial.print("\tSensor_Low: " );
  Serial.println( Sensor->uiInEarth, DEC );

  Serial.print("\tSensor_High: " );
  Serial.println( Sensor->uiInWater, DEC );
  #endif
}
       
