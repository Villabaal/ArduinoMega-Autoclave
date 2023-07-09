
#include "Globales.h"
#include "Timers.h"

//usa timer 4
namespace Alarms{

  using timer = Timer<4>;

  struct Alarm{
  private: 
    static inline uint16_t interval;  //en ms, 16  bits
    static inline uint8_t duty;       // de 1 a 100 %, solo se necesitan 8 bits
    static constexpr uint16_t(*const intervals[2] )()  { 
      []()->uint16_t{  return (100-duty)*(interval/100);  },            //rest cicle
      []()->uint16_t{  return (interval*duty)/100;  }                   //duty cicle
    };
    static void isr_(){ 
      digitalWrite(buzzer,!digitalRead(buzzer)); 
      timer::setInterval( intervals[digitalRead(buzzer)]() );
    }
  public:
    static inline bool state = false;
    //inicia el timer 4 y prepara el buzzer
    static void init() {  pinMode(buzzer,OUTPUT); digitalWrite(buzzer,false);timer::isr = isr_; }
    //enciende el buzzer con cierta frecuencia y ciclo de trabajo para lograr
    //varias indicaciones sonoras (es un PWM de baja frecuencia)
    static void turnOn(uint32_t &&Interval = 3000,uint8_t &&dutyCicle = 8){
      interval = Interval ; duty = dutyCicle ;
      digitalWrite(buzzer,true); 
      timer::setInterval( intervals[true]() );
      timer::enableTimer(); 
      state = true;
    }
    //apaga el buzzer
    static void turnOff(){ digitalWrite(buzzer,false); timer::stopTimer();  state = false; }
    //peticion desde la interfase
    static void request(const uint8_t &){ Serial1.write( aceptar ); if(state) turnOff(); else turnOn(); }
  };
}
