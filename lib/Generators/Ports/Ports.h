#include "../Generators/Controllers/Controllers.h"

namespace Generators{

    template <class Inputs>
    struct Port{};
    template <internalSteam Inputs>
    struct Port<Inputs>{
    private:
        using Control = typename Controller<typename Inputs::ControlType>::template assamble_< Inputs >;
        using Peri = typename Control::Peri;
    public:
        static void actuatorsRequest(bool &&order){             // false: BMB request, true: CNT request 
            uint8_t&& levelDenied = ( 0b01<<(!order) );         //true: levelDenied = empty, false: levelDenied = full
            uint8_t&& water_level = Peri::level_encoder_();
            if( ((water_level != levelDenied)&&(water_level != broken)) || Peri::actuators_(order) ){ 
                Serial1.write(aceptar);
                Peri::actuators_( order, !Peri::actuators_( order ) ); return;
            }
            Serial1.write(denegar);
            if (water_level == broken){ Serial1.write( errorSensorNivel ); return; };
            Serial1.write( tanqueLleno-order );
        }
        static constexpr void ( *const requests[] )() {
            //power request
            [](){ Serial1.write(aceptar); if ( Control::state_ ) Control::turn_off_();  else Control::turn_on_(); },
            //level request
            [](){ Serial1.write(aceptar); Serial1.write( Peri::level_encoder_() ); },
            // CNT request
            [](){ actuatorsRequest( true ); },            
            // BMB request
            [](){ actuatorsRequest( false ); }

        };        
    };

}