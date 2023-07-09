#include <Metaprogrammig.h>
#include <Globales.h>
#include <Timers.h>


#pragma once
#ifndef _BGenerators_H_
#define _BGenerators_H_

//usa el timer 2
namespace Generators{
    using timer = Timer<2>;
    static constexpr uint16_t SampleInterval {1000};    

    struct ExternalSource {};    
    //tipos definidos
    struct Encoder3Pos  { };
    using definedPeripherals = MP::Typelist<Encoder3Pos,ExternalSource>;

    //tipos definidos
    struct LevelSensFit   {};
    struct LevelSensUnFit {};
    using definedController = MP::Typelist<LevelSensFit,LevelSensUnFit,ExternalSource>; 

    //vapor autogenerado
    template< class Inputs > concept internalSteam = 
        (!std::is_same_v< class Inputs::ControlType, ExternalSource >)&&(!std::is_same_v<class Inputs::PeripheralsType, ExternalSource >);
    //no autogenerado (solo controla la presion de la camara exterior)
    template< class Inputs > concept externalSteam = !internalSteam< Inputs >;

        //inputs los manda la clase Engine, son los tipos introducidos por el usuario y salen filtrados
    //por ambiguedades y static_asserts...
    template <class Inputs> struct Assembly{};

    //plantillas de controladores
    template<class CNTRLType> struct Controller{
        //ensambla el controlador
        template< class Inputs >
        using assamble_ = Assembly< Inputs >;          
    };

    //plantillas de perifericos
    template<class PERIType> struct Peripherals{ };


}
#endif
