#include "Metaprogrammig.h"
#include <Arduino.h>
class Relay{
    uint8_t const &relayPin;
public:
    constexpr Relay( uint8_t const &pin ): relayPin( pin )
        { pinMode( relayPin ,OUTPUT ) ; digitalWrite( relayPin, false ); }
    explicit operator bool() const { return digitalRead( relayPin ); }
    void operator() ( bool const &on ){ digitalWrite( relayPin,on ); }
    void operator() (){ digitalWrite( relayPin, !(*this) ); }
};        
//los relays estan fisicamente uno detras de otro (ENA=44,SEC=45...)
//asi que fasilita la funcion de operador [] ( this[ENA].doSomething() )
template<uint8_t N> class RelayVector{
    //clase privada
    Relay (*const Storage)[N];  uint8_t const &begin;
public:
    //construimos todos los relay en expresion constante!!
    constexpr RelayVector( auto const (&valvs)[N] ): 
        Storage( CE::arrayGenerator<Relay>( valvs ) ), begin( valvs[0] ){  }
    Relay& operator[] (const uint8_t &pos) { return (*Storage)[ pos - begin ] ; }
    void clear_all(){ for ( auto &&relay : (*Storage) ) relay(false);  }
};   