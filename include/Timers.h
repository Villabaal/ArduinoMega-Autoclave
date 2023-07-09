#include <Arduino.h>
#include <concepts>

#pragma once
#ifndef _TIMERS_H
#define _TIMERS_H

#define TIMSK(n) _SFR_MEM8(110 + n)
template< uint8_t N > class Timer{
  static_assert( (N>0)&&(N<6),"only 5 timers on Arduino mega" );
  static inline uint16_t counter {},interval {1000};
  friend void Routine<N>(); 
public:
  static void enableTimer(){ if( isr ) TIMSK(N) |= 0b01; }
  static void stopTimer(){ TIMSK(N) &= 0b00; counter = 0 ;}
  static inline void(*isr)() {nullptr};
  //T en milisegundos, en esta funcion se setea el prescaler y el registro del timer
  //(intervalo 1s por defecto)
  static void setInterval( std::unsigned_integral auto const &T ){ 
    interval = T;
    if constexpr ( N==1 ){ TCCR1A = 0; TCCR1B = 0; TCCR1B |= 0b01; TCNT1 = 49536;  }
    if constexpr ( N==2 ){ TCCR2A = 0; TCCR2B = 0; TCCR2B |= 0b100;   TCNT2 = 6;   }
    if constexpr ( N==3 ){ TCCR3A = 0; TCCR3B = 0; TCCR3B |= 0b01; TCNT3 = 49536;  }
    if constexpr ( N==4 ){ TCCR4A = 0; TCCR4B = 0; TCCR4B |= 0b01; TCNT4 = 49536;  }
    if constexpr ( N==5 ){ TCCR5A = 0; TCCR5B = 0; TCCR5B |= 0b01; TCNT5 = 49536;  }
  }
};
//una funcion amiga, para poder accerder a los miembros privados en el ISR(TIMER(N)_OVF_vect){ Routine<N>() }
template<uint8_t N> static void Routine(){
  Timer<N>::counter++;  
  if ( Timer<N>::counter == Timer<N>::interval ){ 
    Timer<N>::isr(); Timer<N>::counter = 0; 
  }
  if   constexpr    ( N == 1 ){ TCNT1 = 49536; }  
  else if constexpr ( N == 2 ){   TCNT2 = 6;   }  
  else if constexpr ( N == 3 ){ TCNT3 = 49536; }  
  else if constexpr ( N == 4 ){ TCNT4 = 49536; }  
  else if constexpr ( N == 5 ){ TCNT5 = 49536; }  
}

#endif
