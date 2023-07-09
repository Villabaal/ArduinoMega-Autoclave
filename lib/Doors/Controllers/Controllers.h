#include <Metaprogrammig.h>
#include <Globales.h>
#include <Timers.h>

//usa el timer 1
namespace Doors{
    static constexpr uint8_t SampleInterval {250};
    using timer = Timer<1>;

    //tipos definidos
    struct Analog  { };
    struct Digital { };
    using definedController = MP::Typelist<Analog,Digital>;
    //plantillas de perifericos
    template<typename... Ts>
    struct Peripherals{ };
    template< >
    struct Peripherals<Digital>{ 
        //inicia los perifericos
        static  void init_(){
            //Actuadores                                              //Sensores
            pinMode(motor,OUTPUT); digitalWrite(motor,LOW);   pinMode(puertaBlanca,INPUT_PULLUP);
            pinMode(giro,OUTPUT);  digitalWrite(giro,LOW);    pinMode(puertaGris,INPUT_PULLUP);  
        }       
        //mueve el motor, abre o cierra
        static  void motor_(bool &&on, bool const &  direction){ digitalWrite(motor,on);    digitalWrite(giro, on && direction );  }
        //devuelve la direccion en la que nos movemos
        static  bool direction_(){ return digitalRead(giro); }
        //devuelve el estado del endline switch (1 == puertaGris, 0 = puerta Blanca)
        static  bool endline_(bool const & direction){ return digitalRead(puertaBlanca-direction); }    

    };    

    //plantillas de controladores
    template<typename... Ts>
    struct Controller{ };
    //controlador on-off, con proteccion de endline switch
    template< >
    struct Controller<Digital> { 
        using Peri = Peripherals<Digital>;
        //inicia los componentes del controlador    
        static void init_(){ Peri::init_(); timer::isr = isr_; timer::setInterval(SampleInterval); }        
        //mueve la puerta true = abrir, false = cerrar
        static void move_( bool const &direction ){ Peri::motor_( true , direction );  timer::enableTimer(); }
        //detiene la puerta
        static void stop_(){ timer::stopTimer(); Peri::motor_( false , false); }
    private:
        //funcion para vigilar los swicth endline
        static void isr_(){ if (  !Peri::endline_( Peri::direction_() )  )  stop_(); }
    };
}