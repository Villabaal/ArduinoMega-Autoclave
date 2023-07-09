#include "../Generators/Ports/Ports.h"


namespace Generators{
    using Components = MP::TempList<Peripherals,Controller>;

    template<class...CMPS>
    struct inputCheck{
        //demasiados componentes
        static_assert( !(sizeof...(CMPS) > Components::size()),"too many components" );
        using results = typename Components::template are_components<CMPS...>;
        static_assert( results::all_are_components,"Not all types are Components specializations" );
        // se introdujo varias veces el mismo componente
        static_assert( !results::any_repeated,"A Component were introduced multiple Times");
        template< char N > using specializations = typename results::typeList::template select<N>;
        static_assert( !results::any_overSpecialized, "components were specialized multiple Times" );
        //tipos por defecto
        //Tipo de controlador por defecto encoder de 3 estados
        using PeripheralsType = typename std::conditional< results::CMPSdefs[0], typename specializations<0>::template select<0>,Encoder3Pos >::type;
        //el sensor de nivel queda perfecto
        using ControlType = typename std::conditional< results::CMPSdefs[1], typename specializations<1>::template select<0>,LevelSensFit >::type;
        //Tipo no declaradp
        static_assert( definedPeripherals::contains< PeripheralsType >() , "Type of Peripherals unknown");        
        static_assert( definedController::contains< ControlType >() , "Type of controller unknown");
    };

    template<class Inputs>
    struct Request{};
    //vapor generado desde el autoclave, con agua, resistencias...
    template< internalSteam Inputs > 
    struct Request<Inputs>{ 
    private:
        using Control = typename Controller<typename Inputs::ControlType>::template assamble_< Inputs >;
        using EndPoint = Port< Inputs >;
    public:
        //Peticion al generador
        static void request(const uint8_t& req){
            EndPoint::requests[req]();
        };   
    };
    template< externalSteam Inputs > 
    struct Request<Inputs>{ static_assert( internalSteam<Inputs>, "not implemented"); };

    template<class Inputs>
    struct Members{};
    //vapor generado desde el autoclave, con agua, resistencias...
    template< internalSteam Inputs >
    struct Members<Inputs>{
    private:
        using Control = typename Controller<typename Inputs::ControlType>::template assamble_< Inputs >;
        using Rutine = Control::Rutine;
        using Peri = typename Control::Peri;
    public:        
        static constexpr auto  &level_encoder = Peri::level_encoder_;
        //inicializacion del control
        static constexpr auto  &init     =       Control::template init_<>;
        //estado del generador
        static constexpr bool  &state    =       Control::state_;
        static constexpr auto  &Errors   =       Rutine::Errors_;
        //apagar/encender generador 
        static constexpr auto  &turnOff  =       Control::turn_off_;
        static constexpr auto  &turnOn   =       Control::turn_on_;   
    };
    template< externalSteam Inputs > 
    struct Members<Inputs>{ static_assert( internalSteam<Inputs>, "not implemented"); };

    //este sera el que se instanciara
    //plantillas de generador de vapor para los tipos definidos
    template<typename... CMPS>
    struct Engine: Request< inputCheck<CMPS...> >,Members< inputCheck<CMPS...> >{ 
        using cmps = inputCheck<CMPS...>;
    };
}
