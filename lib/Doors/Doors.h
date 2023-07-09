#include <Metaprogrammig.h>
#include "../Doors/Ports/Ports.h"


namespace Doors{
    using Components = MP::TempList<Controller,Port>;

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
        //el sensor de nivel queda perfecto
        using ControlType = typename std::conditional< results::CMPSdefs[0], typename specializations<0>::template select<0>,Digital >::type;
        //Tipo no declarado
        static_assert( definedController::contains< ControlType >() , "Type of controller unknown");
    };
    
    template<class Inputs>
    struct Request{
    private:
        using Control = Controller<typename Inputs::ControlType>;
        using EndPoint = Port< Control >;        
    public:
        //Peticion a la puerta
        static void request( const uint8_t& req ){ 
            if ( req==detenerPuerta ) EndPoint::stopRequest_();
            else EndPoint::moveRequest_( req==abrirPuerta );
        };
    };

    template<class Inputs>
    struct Members{
    private:
        using Control = Controller<typename Inputs::ControlType>;
        using Peri = Control::Peri;
    public:
        static constexpr auto &init =  Control::init_;
        static constexpr auto &endline = Peri::endline_;
    };
    
    //Tipo de controlador por defecto Digital
    template<typename... CMPS>
    struct Engine : Request< inputCheck<CMPS...> >, Members< inputCheck<CMPS...> > { };

}
