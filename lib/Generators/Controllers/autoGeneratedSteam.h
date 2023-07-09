#include "../Generators/Controllers/baseController.h"

//usa el timer 2
namespace Generators{

    //plantillas de perifericos
    template<> struct Peripherals<Encoder3Pos>{ 
        //iniciar los perifericos
        static void     init_(){
            pinMode(NB,INPUT_PULLUP);  /* nivel alto */                      pinMode(NA,INPUT_PULLUP);  /* nivel bajo */
            pinMode(BMB,OUTPUT);       /* Valvula de agua o bomba de agua */ pinMode(CNT,OUTPUT);       /* contactor de resistencia */
            digitalWrite(BMB,LOW);                                           digitalWrite(CNT,LOW);
        }
        //sensores
        //presion de camara externa
        static float    external_pressure_(){ return betaPres*analogRead(P_EX); }
        //estado del encoder de nivel
        static uint8_t  level_encoder_(){ return  ( ((!digitalRead(NA))<<1)|(!digitalRead(NB))  ); }
        //actuadores
        //order: true=resistencias, false= bomba de agua, on = estado deseado
        static void     actuators_(bool const &order,bool const &on)     { digitalWrite(BMB-order,on); }      //True CNT, false BMB
        //order: true=resistencias, false= bomba de agua
        static bool     actuators_(bool const &order)            { return digitalRead(BMB-order); }    //True CNT, false BMB
    };


    //el sensor de nivel queda bien o no al tanque
    template<class CNTRLtype> concept fit      = std::is_same<CNTRLtype,LevelSensFit>::value;
    template<class CNTRLtype> concept notfit   = std::is_same<CNTRLtype,LevelSensUnFit>::value;
    template<class T> using enable_fit    = typename std::enable_if_t< fit< T > > ;
    template<class T> using enable_notfit = typename std::enable_if_t< notfit< T > > ;
    
    template<class... Ts> struct Isr{};
    template<class CNTRLType> struct Isr<Encoder3Pos,CNTRLType>{
    private:
        using Peri = Peripherals<Encoder3Pos>;
    public:
        inline static uint8_t Errors_ {0};
        inline static uint8_t Counters[ 2*notfit<CNTRLType> + 2 ] {};               //array de 2 o 4 espacios unsigned char
        static void isr_(){
            bool threshold = Peri::external_pressure_() <= max_external_press;      //limite superior de presion en camara exterior
            uint8_t water_level = Peri::level_encoder_();                           //codificacion del nivel
            if (water_level == broken ){ bitSet(Errors_,E2); return; }  //si el nivel es erroneo salimos
            Peri::actuators_(true, threshold && (bitRead(water_level,1)) );         //Controla las resistencias  
            if constexpr ( notfit<CNTRLType> ){                                     //Se instancia el conteo de sobrellenado?
                if ( (water_level == full) && Peri::actuators_(false) ){            //la bomba esta activa (nivel alto)
                    timerHandler( Counters[2],Counters[3], [](){                    //cuenta de sobrenivel
                        Peri::actuators_( false,false ) ;                           //Apaga la bomba
                    });  return;                                                    //salimos
                }               
            }              
            Peri::actuators_( false, bitRead(water_level,0) );                      //Controla la bomba
            if ( water_level == empty ){                                            //perro guardian del flotador    
                timerHandler( Counters[0],Counters[1],[](){                         //falla en el flotador
                    bitSet(Errors_,E1);                                             //Error: tiempo de llenado superado         
                }); return;
            }
            Counters[0] = 0;
        }
    };
    
    template<class PERIType, class CNTRLType> struct Init{
    private:
        using Peri = Peripherals<PERIType>;
    public:
        using Rutine = Isr<PERIType,CNTRLType>;
        //inicializa el generador con el periodo(segs.) perro guardian de flotador  
        template < typename T = CNTRLType, typename = enable_fit<T> >
        static void init_(uint8_t &&WT) {
            Rutine::Counters[1] =  WT ; 
            Peri::init_();  timer::isr = Rutine::isr_;
            timer::setInterval(SampleInterval);
        }
        //inicializa el generador con el periodo(segs.) perro guardian de flotador
        //y con el periodo(segs.) de sobrellevado (sensor de nivel mas chico de lo debido)        
        template < typename T = CNTRLType, typename = enable_notfit<T> >
        static void init_(uint8_t &&WT,uint8_t &&OFT){
            Rutine::Counters[1] =  WT ; 
            Rutine::Counters[3] = OFT ; 
            Peri::init_();  timer::isr = Rutine::isr_;
            timer::setInterval(SampleInterval);
        }
    };

    template < internalSteam Inputs > struct Assembly<Inputs>:
    Init< Inputs::PeripheralsType , Inputs::ControlType >{
        using Peri = Peripherals< Inputs::PeripheralsType >;
        //Encendido o apagado
        static inline bool state_ = false; 
        static void turn_on_(){ timer::enableTimer(); state_ = true; }
        static void turn_off_(){        
            Peri::actuators_(false,false); Peri::actuators_(true,false); 
            timer::stopTimer(); state_ = false;
        }        
    };    

}