#pragma once
#include "magic_macro.h"
#include <utility>
#include <type_traits> 
namespace PO {
	namespace Tool {

		template<typename T, T t> struct const_value { static constexpr T value = t; };

		template<typename T> struct itself { typedef T type; };

		template<bool, template<typename ...> class t1 = itself, template<typename...> class t2 = itself > struct bind_conditional {
			template<typename ...AT> using bind = t1<AT...>;
		};
		template<template<typename ...> class t1, template<typename ...> class t2 > struct bind_conditional<false, t1, t2> {
			template<typename ...AT> using bind = t2<AT...>;
		};

		template<bool, typename T, template<typename> class role> struct mapping_if {
			typedef typename role<T>::type type;
		};

		template< typename T, template<typename> class role> struct mapping_if< false, T, role> {
			typedef typename T type;
		};

		template<typename T> struct get_type {
			typedef typename T::type type;
		};

		template<typename T> struct get_deep_type {
			typedef typename T::type::type type;
		};

		template<template<typename...> class tank, typename ...AT> struct instant {
			typedef typename tank<AT...>::type type;
		};

		template<template<typename...> class intruct, template<typename...> class tank, typename ...L> struct depute_instant {
			typedef typename tank< typename intruct<L>::type ... >::type type;
		};

		template<template<typename...> class tank, typename ...AT> struct extract {
			typedef typename tank<AT...> type;
		};

		template<template<typename...> class intruct, template<typename...> class tank, typename ...AT> struct depute_extract {
			typedef typename tank<typename intruct<AT>::type ...> type;
		};

		template< bool def , bool ...other > struct value_and : public std::conditional<def,value_and<other...>,std::false_type>::type{};
		template< bool def> struct value_and<def> : public std::conditional<def, std::true_type, std::false_type>::type{};
		
		template< bool def, bool ...other > struct value_or : public std::conditional<def, std::true_type, value_or<other...>>::type {};
		template< bool def> struct value_or<def> : public std::conditional<def, std::true_type, std::false_type>::type{};

		template<typename T, typename ...AT> struct is_one_of:public std::false_type {};

		template<typename T, typename I,typename ...AT> 
		struct is_one_of<T,I,AT...>:public std::conditional< std::is_same<T, I>::value,std::true_type, is_one_of<T, AT...> >::type {};

		template<typename ...AT> struct is_repeat : public std::false_type {};
		template<typename T, typename ...AT> 
		struct is_repeat <T, AT...>:public std::conditional< is_one_of<T,AT...>::value , std::true_type, is_repeat < AT...>  >::type{};

		template<typename...> struct template_void {};
		template<typename T, typename K, typename U> struct type_conditional {
			typedef typename std::conditional<T::value, K, U>::type type;
		};

		template<typename T, typename K> struct link {};
		template<template<typename...> class tank, typename ...AK, typename ...AK2> struct link<tank<AK...>, tank<AK2...>> {
			typedef tank<AK..., AK2...> type;
		};

		template<typename T> struct is_type_template :std::false_type {};
		template<typename ...T, template <typename ...> class tank> struct is_type_template<tank<T...>> :std::true_type {};



		struct control_ {
			struct empty_ {};
			struct return_ {};
			struct this_space_ {};
			struct last_space_ {};
			struct break_ {};
			struct continue_ {};
		};

	}


}