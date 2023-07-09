#include <Arduino.h>
#pragma once
#ifndef _GLOB_H_
#define _GLOB_H_
//constantes globales
//resoluciones de los sensores analogicos
static constexpr float betaPres {0.1447721179624665};           //resolucion del sensor de presion
static constexpr float betaTemp {0.31777033730158727};          //resolucion del sensor de temperaturauint16_t

//caracteristicas de la placa arduino mega y version 1 de tarjeta controladora
//perifericos especificos
//constantes globales
//indicador
static constexpr uint8_t  led {14};
//Banderas de Error
static constexpr uint8_t  E0  {0};                   //Error de valvula de vapor
static constexpr uint8_t  E1  {1};                   //Error de flotador
static constexpr uint8_t  E2  {2};                   //Error de nivel
static constexpr uint8_t  E3  {3};                   //Error en camara Exterior
static constexpr uint8_t  E4  {4};                   //Error en camara Interior
static constexpr uint8_t  E5  {5};                   //Error de despresurizacion
static constexpr uint8_t  E6  {6};                   //Error de Rellenado
static constexpr uint8_t  E7  {7};                   //error de endline swich en puerta

//constantes del generador
static constexpr float  max_external_press {50};
//codigo del nivel
static constexpr uint8_t full       {2};
static constexpr uint8_t medium     {3};
static constexpr uint8_t empty      {1};
static constexpr uint8_t broken     {0};

//Sensores
static constexpr uint8_t T_IN         {A0};
static constexpr uint8_t P_IN         {A1}; 
static constexpr uint8_t P_EX         {A2};
static constexpr uint8_t puertaGris   {A4};
static constexpr uint8_t puertaBlanca {A5};
static constexpr uint8_t NA           {A6};
static constexpr uint8_t NB           {A7};
//Actuadores
static constexpr uint8_t buzzer  {43};
//relevadores del proceso
static constexpr uint8_t Valvs[] { 44,45,46,47,48,49 };
static constexpr const uint8_t& ENA     {Valvs[0]};
static constexpr const uint8_t& SEC     {Valvs[1]};
static constexpr const uint8_t& ESL     {Valvs[2]};
static constexpr const uint8_t& ESR     {Valvs[3]};
static constexpr const uint8_t& EM      {Valvs[4]};
static constexpr const uint8_t& EST     {Valvs[5]};
//generador
static constexpr uint8_t CNT     {50};
static constexpr uint8_t BMB     {51};
//puerta
static constexpr uint8_t motor   {52};
static constexpr uint8_t giro    {53};

//etiquetas de comunicacion serial
    //respuestas a peticiones de la interfase
static constexpr uint8_t aceptar {'k'};
static constexpr uint8_t denegar {'n'};
static constexpr uint8_t error   {'e'};
//codigos de peticiones y denegaciones
//peticiones de puerta    
static constexpr uint8_t abrirPuerta   {0};
static constexpr uint8_t cerrarPuerta  {1};
static constexpr uint8_t detenerPuerta {2};
//peticiones de medidas
static constexpr uint8_t MedidaP_In        {10};
static constexpr uint8_t MedidaP_Ex        {11};
static constexpr uint8_t MedidaTemp        {12};
//peticiones de alarma
static constexpr uint8_t buzzerIO           {20};
//peticiones de generador
static constexpr uint8_t generadorIO        {30};
static constexpr uint8_t MedidaNivel        {31};
static constexpr uint8_t ResistenciasManual {32};
static constexpr uint8_t BombaManual        {33};
//peticiones del proceso 
static constexpr uint8_t Process121           {40};
static constexpr uint8_t Process134           {41};
static constexpr uint8_t ProcessCorto         {42};
static constexpr uint8_t Resumen              {43};
static constexpr uint8_t AireManual           {44};
static constexpr uint8_t SuccionManual        {45};
static constexpr uint8_t EscapeLentoManual    {46};
static constexpr uint8_t EscapeRapidoManual   {47};
static constexpr uint8_t CalentManual         {48};
static constexpr uint8_t SaltarFase           {49};
//denegacion de la peticion de generador
static constexpr uint8_t tanqueLleno          {2};
static constexpr uint8_t tanqueVacio          {1};
static constexpr uint8_t errorSensorNivel     {0};
//denegacion del proceso 
static constexpr uint8_t puertaAbierta         {0};
static constexpr uint8_t presionInsuficiente   {1};
//etapas del proceso
static constexpr uint8_t Prewarm              { 0 };
static constexpr uint8_t Warm                 { 1 };
static constexpr uint8_t Sterelization        { 2 };
static constexpr uint8_t Expulsion            { 3 };
static constexpr uint8_t Refill               { 4 };
static constexpr uint8_t Drying               { 5 };
static constexpr uint8_t Finish               { 6 };
static constexpr uint8_t Free                 { 7 };



// funcion general para timers de ciclo infinito ( **timer interrupts... )
static void timerHandler( auto &counter, uint16_t const &endCount , void (*onCount)(), void (*onEnd)() ){
    counter++; onCount();
    if (counter >= endCount){ counter = 0; onEnd(); }
}
// funcion general para timers de ciclo infinito ( **timer interrupts... )
static void timerHandler( auto &counter, uint16_t const &endCount, void (*onEnd)() ){
    timerHandler( counter, endCount, [](){}, onEnd );
}    



#endif