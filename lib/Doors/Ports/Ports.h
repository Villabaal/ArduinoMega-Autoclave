#include <Metaprogrammig.h>
#include "../Doors/Controllers/Controllers.h"


namespace Doors{

    template <class Control> struct Port{
    private:
        using Peri = typename Control::Peri;
    public:
        //peticion de movimiento desde la interfase
        static void moveRequest_( bool const &direction ){
            if ( Peri::endline_( direction ) ){ Serial1.write( aceptar ); Control::move_( direction ); return;}
            Serial1.write( denegar ); Serial1.write( denegar );
        };
        //peticion de pero desde la interfase
        static void stopRequest_(){
            Serial1.write( aceptar );  Control::stop_();        
        }  
    };

}