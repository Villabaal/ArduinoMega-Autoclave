
#pragma once
#ifndef _MP_H_
#define _MP_H_
#include <type_traits>
#include <utility>
#include <cstdint>



namespace MP{
    // devuelve el objto A si la condicion es true, B si false
    template<bool cond,auto A,auto B> 
    static consteval auto obj_conditional() noexcept { if constexpr(cond) return A; else return B; };
    template <typename T>
    struct type_id{  using type = T;  };
    //constante    
    template <typename T, T val>
    struct constant { static constexpr T value {val};   };
    // Lo mismo pero para template template parameters
    template < template <typename...> class T, template <typename...> class U >
    struct is_same : constant<bool,false> {};
     template < template <typename...> class T >
    struct is_same <T, T> : constant<bool,true> {};
    //tipo n-esimo de una lista
    template <char N, typename... Ts>
    struct nth_type{ using type = void; };
    template <char N, typename... Ts>
    using nth_type_t = typename nth_type<N,Ts...>::type;
    template <typename T, typename... Ts>
    struct nth_type<0, T, Ts...>: type_id< T > { };
    template <char N, typename T, typename... Ts>
    struct nth_type<N, T, Ts...> : type_id< nth_type_t< N-1, Ts...> > {};
    // posicion de un tipo en una lista          
    template <typename T, typename... Ts>
    struct pos_type{ static_assert(sizeof...(Ts) != 0,"Error: type doesn't exist"); };
    template <typename T, typename... Ts>
    static constexpr char pos_type_v = pos_type<T,Ts ...>::value;
    template <typename T, typename... Ts>
    struct pos_type<T,T,Ts...> : constant< char, 0> {};
    template <typename T, typename U, typename... Ts>
    struct pos_type<T,U,Ts...> 
        : constant< char, (1 + pos_type_v<T, Ts ...>) >{};  

    template < typename... Ts>
    struct Typelist{
        //tamaño de la lista de tipos
        constexpr static char size() noexcept { return sizeof... (Ts); }
        //devuelve true si la lista contiene el tipo pasada como argumento
        template<typename T>
        constexpr static bool contains() noexcept { 
            return ( false || ... || std::is_same<T, Ts>::value );
        }
        //la posicion numerica del tipo en la lista
        template<typename T>
        constexpr static char pos() noexcept { 
            static_assert(contains<T>());
            return pos_type_v<T,Ts ...>;
        }            
        // //regresa el template n-esimo
        template< char N > 
        using select = nth_type_t<N,Ts...>;

        //agrega un(o mas) tipo(s) a la lista
        template<class... Args>
        struct push { using typeList = Typelist<Ts...>; };        
         //helper de push 
        template<class... Args>
        using push_L = typename push<Args...>::typeList;         
        //recibimos un solo tipo, y lo agregamos
        template<class T>
        struct push<T> { using typeList = Typelist<Ts...,T>; };
        //recibimos un Typelist y agregamos sus tipos contenidos
        template <class... Args>
        struct push<Typelist<Args...>> { using typeList = Typelist<Ts...,Args...>; };       
        //agrega un(o mas) tipo(s) o una(o mas) lista(s) a la lista
        template<class T,class... Args>
        struct push<T,Args...> {
            using typeList = typename push_L< T >::template push_L<Args...> ;
        };        
   
    };    

    template< typename... Ts > 
    struct use_TypeList{ using typeList = typename Typelist<>::template push_L< Ts... > ; };
    //checar si el typo es una especializacion de un template dado y devuelve en una Typelist
    //los tipos que especializan a TEMP
    template <class T, template <class...> class Template>
    struct is_specialization : constant<bool,false> ,use_TypeList<> { using cmp = class Typelist<>; };
    template <class T, template <class...> class Template>   //helper de valor 
    static constexpr bool is_specialization_v = is_specialization<T,Template>::value;  
    template <class T, template <class...> class Template>   //helper de Lista de tipos 
    using is_specialization_L = typename is_specialization<T,Template>::typeList;          
    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : 
        constant<bool,true> ,use_TypeList<Args...> { using cmp = class Template<Args...> ; };
    //toma tipos y descubre cuantos tipos son especializaciones de TEMP y guarda una Typelist con todos los
    //tipos para los que TEMP esta especializado
    template<template <class...> class TEMP,class... Ts>
    struct are_specializations :  constant< char, (is_specialization_v<Ts,TEMP>+...+0) > ,
    use_TypeList< is_specialization_L<Ts,TEMP>... > { 
        using cmps = typename Typelist<>::template push_L< typename is_specialization<Ts,TEMP>::cmp... > ; 
    };
    template<template <class...> class TEMP,class... Ts> //helper
    using are_specializations_L = typename are_specializations<TEMP,Ts...>::typeList;
    

    //lista de templates
    template < template <typename,typename...> class... TEMPS>
    struct TempList{
        //tamaño de la lista de templates
        constexpr static char size() noexcept { return sizeof... (TEMPS); }        
        //toma un pack de tipos y descubre cuantos son especializaciones de cada TEMPS
        // y guarda en una Typelist todos los tipos que especializan cada uno de los TEMPS
        template<class... Ts>
        struct are_components{ 
            //array de banderas con indice respectivo a cada (TEMPS...), true si cada (TEMPS...) fue especializado
            static constexpr bool CMPSdefs[sizeof...(TEMPS)] { (are_specializations<TEMPS,Ts...>::value>0)... };
            //todos los tipos (Ts...) son especializaciones de algunos de los (TEMP...)
            static constexpr bool all_are_components { 
                ( are_specializations<TEMPS,Ts...>::value + ... + 0 ) == sizeof... (Ts)
            };
            //dos o mas (Ts...) son especializaciones de un (TEMPS...) ?
            static constexpr bool any_repeated { 
                (( are_specializations<TEMPS,Ts...>::value > 1 ) || ... || false)
            };            
            //alguno de los (TEMPS...) no tiene especializacion en (Ts...)
            static constexpr bool any_missing { 
                (( are_specializations<TEMPS,Ts...>::value == 0 ) || ... || false)
            };
            //algun (TEMPS...) tienes mas de una especializacion
            static constexpr bool any_overSpecialized { 
                (( are_specializations_L<TEMPS,Ts...>::size() > 1 ) || ... || false)
            };            
            using typeList = Typelist< are_specializations_L<TEMPS,Ts...>... >;
            using cmps = Typelist< typename are_specializations<TEMPS,Ts...>::cmps... >;
        };
    };

};
    template<uint8_t N> static consteval uint8_t arraySize( auto(&)[N] ) noexcept { return N; }
    // tipo referencia a un array
    template<class T,uint8_t N> using arr_ref = T(&)[N] ;
    template<class T,uint8_t N> using arr_temporary = T(&&)[N] ;    
    template<class T,uint8_t N> using arr_ptr = T(*)[N] ;

namespace CE{
    //toma un array y toma todos sus componentes y los toma para construir array de Objetos Consumer de tamaño N
    template < class Consumer, uint8_t N ,typename IS = decltype( std::make_index_sequence< N >() ) > 
    struct Generator;
    // especializacion para crear un template container de los indices y crear el array de objetos 
    template < class Consumer,uint8_t N, std::size_t... I>
    struct Generator< Consumer,N, std::index_sequence<I...>> {
        //v tiene que ser una expresion constante para poder ser expandido asi...
        template<class elementType>
        static constexpr arr_ptr<Consumer,N> construction( elementType (&v)[N] ) noexcept {
            return reinterpret_cast< Consumer(*)[N] >( new Consumer[N]{ v[I]... } ); 
        }
    };
    //usado como helper de Generator para deducir el tamaño del array a partir del array entrada
    //crea un array de objetos Consumer inicializados cada uno con un valor respectivo del array v
    template <class Consumer, uint8_t N> 
    static constexpr arr_ptr<Consumer,N> arrayGenerator( const auto (&v)[N] ) noexcept {
        return Generator<Consumer,N>::construction( v );
    }
} 


#endif

