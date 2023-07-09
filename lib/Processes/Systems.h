#include "../Processes/Ports/Ports.h"

namespace Processes{
    using Components = MP::TempList< Doors::Engine , Generators::Engine >;

    template<class...CMPS>
    struct inputCheck{
    private:        
        //demasiados componentes
        static_assert( !(sizeof...(CMPS) > Components::size()),"too many components" );
        using results = typename Components::template are_components<CMPS...>;
        static_assert( results::all_are_components,"Not all types are Components specializations" );
        // se introdujo varias veces el mismo componente
        static_assert( !results::any_repeated,"A Component were introduced multiple Times");
        template< uint8_t N > using cmps = typename results::cmps::template select<N>::template select<0>;      
    public:  
        using Door = cmps<0>; using Generator = cmps<1>;
    };
    
    template<class CMPS> struct Request{
    private:
       using Endpoint = Port<CMPS>;
    public:
       static void request( uint8_t const& req ){
            if ( req < 3 ) {
                 Endpoint::sterilizationRequest( req ); return;
            }           
            if ( req == 3 )    { Endpoint::stateRequest(); return; }
            if ( req == 9 ) { Endpoint::jumpRequest(); return; }
            Endpoint::actuatorsRequest( req );
       }
    };

    template<class CMPS> struct Members{
    private:
        using Control = Controller<CMPS>;
    public:
        static constexpr auto &init =   Control::init;
        static constexpr auto &state =  Control::state_;
        static constexpr auto &abort =  Control::abort;
        static constexpr auto &Errors = Control::Errors;
    };
    
    template<typename... CMPS>
    struct Engine : Request< inputCheck<CMPS...> >, Members< inputCheck<CMPS...> > { 
        using cmps = inputCheck<CMPS...>;
    };


}