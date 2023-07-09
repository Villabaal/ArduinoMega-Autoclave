//mediciones analogicas
#include "Globales.h"
#include <Timers.h>

template<class T> concept arithmetic = std::is_arithmetic_v< std::remove_reference_t< T > >;

//crea una referencia a un arreglo y agrega algunas operaciones vectoriales
template <arithmetic T,uint8_t N> class vector{
private:
  T (&storage)[N]; //Referencia a la memoria del array
public:
    constexpr vector(  T(&v)[N]) : storage( v ) { }
     T& operator[](uint8_t const &location){  return storage[ location ]; };
    // //producto punto
    template<uint8_t I = N-1> float operator*( arithmetic auto const (&u)[N] ){
      if constexpr (I == 0) return storage[I]*u[I]; else return storage[I]*u[I] + this->operator*<I-1>(u) ;
    }
    // asingar valores de otro array de mismo tamaño
    template<uint8_t I = N-1> void operator=( arithmetic auto const (&u)[N] ){
      if constexpr (I > 0){ storage[I] = u[I];  this->operator=<I-1>(u); } else storage[I] = u[I]; 
    }
};

//usa el timer 3
namespace Meassures{
  using timer = Timer<3>;
  static constexpr uint8_t SampleInterval {100};
  class AnalogMeassures{
    //Parametros de temperatura y presion(ecuacion caracteristica)
    //constantes del filtro digital
    static constexpr float a[] {4.005914995308668e-05,0.000155265542181926,3.761960249867857e-05};
    static constexpr float b[] {-2.874377619492435,2.7565219420852456,-0.8819113782981763};
    static inline float x[4] {}, y[4] {};
    static inline vector dx { ( float(&)[3] )( x[1] ) }; //son los ultimos 3 elementos de x
    static inline vector dy { ( float(&)[3] )( y[1] ) }; //son los ultimos 3 elementos de y
    //sistema de convolucion discreta, filtro digital de 3 orden butterworth
    //constantes a y b calculadas para un pt100 a una frecuencia de muestreo de 10hz. 
    static void isr_(){
      x[0] = analogRead(T_IN)*betaTemp;          //la entrada es la medida en bruto del pt100
      y[0] = dx*a - dy*b;                        //la salida del filtro solo depende de los valores pasados
      dx = ( float(&)[3] )( x[0] ) ;             //mantenemos en memoria 3 resagos de la entrada
      dy = ( float(&)[3] )( y[0] ) ;             // y tambien de la salida
    };
    static constexpr float( *meassures[3] )() { 
      [](){ return betaPres*analogRead(P_IN); },  //internal_pressure_
      [](){ return betaPres*analogRead(P_EX); },  //external_pressure_
      [](){ return y[0]; }                        //internal_temp_
    };
  public:
    //inicia el filtro
    static void init() { timer::isr = isr_; timer::setInterval(SampleInterval); timer::enableTimer(); };
    static constexpr auto &internal_pressure_  = meassures[0];
    static constexpr auto &external_pressure_  = meassures[1];
    static constexpr auto &internal_temp_      = meassures[2];
    static void request(uint8_t const &req){
      Serial1.write(aceptar); Serial1.print( meassures[req]() );
    }
  };
}
