#include "../Processes/Controllers/Controllers.h"

namespace Processes{


    template <class CMPS>
    struct Port{};
    template <hasDoor CMPS>
    struct Port<CMPS>{
        static void sterilizationRequest( uint8_t const& req ){
            if (( !Door::endline(false) )&&( sensors::external_pressure_() >= PresExMin )){ 
                Serial1.write( aceptar );                             //okay a la peticion de la interfase
                Control::sterilization( params[req][0] , params[req][1], params[req][2] );
                return;
            }
            Serial1.write( denegar );                                                                 //no fue posible llevar a cabo la peticion
            if ((sensors::external_pressure_()<PresExMin)) Serial1.write( presionInsuficiente );      //no hay presion suficiente en camara externa
            else Serial1.write( puertaAbierta );      
        }
        static void stateRequest(){
            static constexpr uint8_t& state = Control::state_; 
            static constexpr auto sendTime = [&state](const uint16_t&& timer){
                const uint8_t&& min {timer/60}, seg {timer%60};
                Serial1.write( aceptar );                                 //okay a la peticion de la interfase
                Serial1.write( state );                                   //envia el estado actual
                Serial1.write(min); Serial1.write(seg);                   //enviamos el timer en formato min : segs 
            };
            if (( state == Sterelization )||( state == Drying )||( state == Finish )){  
                sendTime( Control::countDown()/2 ); return; 
            }
            Serial1.write( aceptar ); Serial1.write( state );             //okay a la peticion de la interfase
        }
        static void jumpRequest(){ Serial1.write( aceptar ); Serial.write(aceptar); Control::jump(); }
        static void actuatorsRequest( uint8_t const& req ){
            Serial1.write(aceptar); 
            if ( req == 8 ){  Control::relays[EM](); Control::relays[EST](); return; }
            Control::relays[ 40+req ]();
        }
    private:
        static constexpr uint16_t params[3][3] {{20,35,121},{10,35,134},{3,3,121}};
        using Control = Controller<CMPS>;
        using sensors = Meassures::AnalogMeassures;
        using Door  = typename CMPS::Door;        
    };

}
