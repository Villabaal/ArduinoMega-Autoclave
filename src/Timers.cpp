#include "Timers.h"
//estos irs con vectores macro mejor aqui aislado
ISR(TIMER1_OVF_vect){ Routine<1>(); }
ISR(TIMER2_OVF_vect){ Routine<2>(); }
ISR(TIMER3_OVF_vect){ Routine<3>(); }
ISR(TIMER4_OVF_vect){ Routine<4>(); }
ISR(TIMER5_OVF_vect){ Routine<5>(); }