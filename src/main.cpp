#include "Communications.h"


//puerta con controlador Digital
using door = Doors::Engine<>;
//generdor con nivel encoder (mas chico de lo debido)
using GeneratorController = Generators::Controller< Generators::LevelSensUnFit >;
using generator = Generators::Engine< GeneratorController >;
//proceso, puerta y/o generador o nada
using process = Processes::Engine< generator,door >;
//Puerto general, para la comunicacion con la interfase
using Port = Comm::Port< process >;

void setup() { 
  //2 minutos de perro guardian del generadoy y 20 segundos de sobrellenado
  Port::setup( 120,20 );
}

// checar el puerto serie por requests y checar por errorres
void loop() {
  Port::loop();
}
