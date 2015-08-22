#pragma once
#include "type_template_map.h"
#include <utility>
namespace PO {
	namespace Tool {
		namespace TTC { //type template calculate

			template<typename K = map_<>, typename ...T> class parameter_stack_ {
				static_assert(value_and<is_map_<K>::value, is_map_<T>::value...>::value, "parameter_stack_ only receive map_");
				template<typename C, typename P> struct get_have {
					typedef typename C::template have<P> type;
				};
				template<typename C, typename P> struct get_get {
					typedef typename C::template get<P>::type type;
				};
				template<typename C, typename P> struct get_equal {
					typedef typename C::template get<P>::type type;
				};
			public:
				template<typename C, typename P> struct alloc {
					static_assert(!K::template have<C>::value, "this parameter are already exist");
					typedef parameter_stack_<typename K::template insert<C, P>::type, T... > type;
				};

				struct pop {
					typedef parameter_stack_<T...> type;
				};

				struct push {
					typedef parameter_stack_<map_<>, K, T...> type;
				};

				template<typename C>
				struct have : std::conditional< K::template have<C>::value,
					itself<std::true_type>,
					instant<get_have,parameter_stack_<T...>,C>
				>::type::type {

				};

				template<typename C> struct get {
					typedef typename std::conditional<
						K::template have<C>::value,
						instant<get_get, K, C>,
						instant<get_get, parameter_stack_<T...>, C>
					>::type::type type;
				};

				template<typename C, typename P> struct equal {
					typedef typename std::conditional<
						K::template have<C>::value,
						itself<parameter_stack_< typename K::template insert<C,P>::type ,T...> >,
						depute_instant<
							get_type,
							link,
							itself< parameter_stack_<K> >,
							instant< parameter_stack_<T...>::template equal,C,P>
						>
					>::type::type type;
				};
			};

			template<typename K> class parameter_stack_<K> {
				static_assert(is_map_<K>::value, "parameter_map_ only receive map_");
			public:
				template<typename C, typename P> struct alloc {
					static_assert(!K::template have<C>::value, "this parameter are already exist");
					typedef parameter_stack_<typename K::template insert<C, P>::type > type;
				};

				struct push {
					typedef parameter_stack_<map_<>, K> type;
				};

				template<typename C>
				struct have : K::template have<C>{};

				template<typename C> struct get {
					static_assert(have<C>::value, "parameter_map_ do not have this value");
					typedef typename K::template get<C>::type type;
				};

				template<typename C, typename P> struct equal {
					static_assert(have<C>::value, "parameter_map_ do not have this value");
					typedef parameter_stack_<typename K::template insert<C, P>::type> type;
				};
			};

			template<typename T> struct is_parameter_stack_ : std::false_type {};
			template<typename K, typename ...T> struct is_parameter_stack_<parameter_stack_<K, T...>> : std::true_type{};

			template< typename attribute_type = map_<>, typename parameter_type = parameter_stack_<> >
			struct out_ {
				static_assert(is_parameter_stack_<parameter_type>::value, "out_ only receive parameter_stack_");
				static_assert(is_map_<attribute_type>::value, "out_ only receive map_");
				struct pop_ {
					typedef out_<attribute_type, typename parameter_type::pop::type> type;
				};
				struct push_ {
					typedef out_<attribute_type, typename parameter_type::push::type> type;
				};
				template<typename T,typename K>
				struct alloc_ {
					typedef out_<attribute_type, typename parameter_type::template alloc<T, K>::type> type;
				};
				template<typename T, typename K>
				struct insert_ {
					typedef out_<typename attribute_type::template insert<T,K>::type, parameter_type> type;
				};
				template<typename T, typename K>
				struct equal_ {
					typedef out_<attribute_type, typename parameter_type::template equal<T, K>::type> type;
				};
				template<typename T>
				struct get_parameter {
					typedef typename parameter_type::template get<T>::type type;
				};
				template<typename T>
				struct get_attribute {
					typedef typename attribute_type::template get<T>::type type;
				};
				template<typename T>
				struct have_parameter :parameter_type::template have<T>{};
				template<typename T>
				struct have_attribute :attribute_type::template have<T>{};

				template<typename T>
				struct erase_ {
					typedef out_<typename attribute_type::template erase<T>::type, parameter_type> type;
				};

				typedef attribute_type attribute;
				typedef parameter_type parameter;
			};

			template<typename T> struct get_attribute {
				typedef typename T::attribute type;
			};

			template<typename T> struct get_parameter {
				typedef typename T::parameter type;
			};

			template<typename T> struct is_out_ :std::false_type {};
			template<typename T, typename P> struct is_out_<out_<T, P>> :std::true_type{};

			template<typename T> class have_input_template_execute {
				template<template<typename...> class> struct detector;
				template<typename P> static std::true_type func(detector<P::template input>*);
				template<typename P> static std::false_type func(...);
			public:
				typedef decltype(func<T>(nullptr)) type;
			};

			template<typename T> struct have_input_template :have_input_template_execute<T>::type {};

			template<typename T, typename ...AT> struct instant_input {
				typedef typename T::template input<AT...>::type type;
			};

			template<typename T, typename ...AT> struct instant_extract {
				typedef typename T::template extract<AT...>::type type;
			};


			
			template<typename T> struct para_ {
				template<typename inp_t> struct extract {
					static_assert(inp_t::template have_parameter<T>::value, "para_ can not catch this parameter");
					typedef typename inp_t::template get_parameter<T>::type type;
				};
			};

			template<size_t i> struct in_ {
				template<typename inp_t> struct extract {
					typedef typename para_<in_<i>>::template extract<inp_t>::type type;
				};
			};

			template<typename T> struct attr_ {
				template<typename inp_t> struct extract {
					static_assert(inp_t::template have_parameter<T>::value, "attr_ can not catch this parameter");
					typedef typename inp_t::template get_parameter<T>::type type;
				};
			};

			template<typename T> class have_extract_template_execute {
				template<template<typename...> class> struct detector;
				template<typename P> static std::true_type func(detector<P::template extract>*);
				template<typename P> static std::false_type func(...);
			public:
				typedef decltype(func<T>(nullptr)) type;
			};

			template<typename T> struct have_extract_template :have_extract_template_execute<T>::type {};

			template<typename T, typename in> struct get_extract_type_ {
				typedef typename std::conditional<
					have_extract_template<T>::value,
					instant_extract<T, in>,
					itself<T>
				>::type::type type;
			};

			template<typename ...T> struct statement_ {
				template<typename int_t> struct input {
					static_assert(is_out_<int_t>::value, "statement_ only receive template out_");
					typedef int_t type;
				};
			};

			template<typename T, typename ...AT> struct statement_<T, AT...> {
				static_assert(have_input_template<T>::value, "statement_ only receive type which have template input");
				template<typename inp_t> class input {
					static_assert(is_out_<inp_t>::value, "statement_ only receive template out_");
					typedef typename T::template input<inp_t>::type this_resoult;
				public:
					typedef typename std::conditional< 
						this_resoult::template have_attribute<control_::return_>::value
						|| this_resoult::template have_attribute<control_::continue_>::value
						|| this_resoult::template have_attribute<control_::break_>::value,
						itself<this_resoult>,
						instant<
							statement_<AT...>::template input,
							this_resoult
						>
					>::type::type type;
				};
			};


			template<size_t i,typename ...T> struct func_ {
				template<typename inp_t> struct input {
					typedef typename statement_<T...>::template input<inp_t>::type type;
					static_assert(
						!type::template have_attribute<control_::break_>::value &&
						!type::template have_attribute<control_::continue_>::value,
						"out of aera of break_ or continue_"
						);
				};
			};

			template<typename T> struct is_func_ :std::false_type {};
			template<size_t k,typename ...T> struct is_func_<func_<k,T...>> :std::true_type {};
			template<typename T, size_t i> struct is_func_parameter_indexflow {
				static_assert(is_func_<T>::value,"must be func_");
			};
			template<typename ...T,size_t k, size_t i> 
			struct is_func_parameter_indexflow<func_<k, T...>, i> :std::conditional<k == i, std::true_type, std::false_type>::type {};

			template<typename T, typename ...AT> struct if_ {
				template<typename inp_t> class input {
					static_assert(is_out_<inp_t>::value, "input only receive out_");
					typedef typename inp_t::push_::type para;
					typedef typename std::conditional<
						get_extract_type_<T, inp_t>::type::value,
						instant<statement_<AT...>::template input,para>,
						itself<para>
					>::type::type tem_type;
				public:
					typedef typename tem_type::pop_::type type;
				};

				template<typename ...AK> struct else_ {
					template<typename inp_t> class input {
						static_assert(is_out_<inp_t>::value, "input only receive out_");
						typedef typename inp_t::push_::type para;
						typedef typename std::conditional<
							get_extract_type_<T, inp_t>::type::value,
							instant<statement_<AT...>::template input, para>,
							instant<statement_<AK...>::template input, para>
						>::type::type temp_type;
					public:
						typedef typename temp_type::pop_::type type;
					};
				};
			};

			template<typename T, typename P = control_::empty_> struct alloc_ {
				template<typename inp_t> struct input {
					static_assert(is_out_<inp_t>::value, "input only receive out_");
					typedef typename inp_t::template alloc_< T, typename get_extract_type_<P, inp_t>::type >::type type;
				};
			};

			template<typename T, typename P> struct equal_ {
				template<typename inp_t> struct input {
					static_assert(is_out_<inp_t>::value, "input only receive out_");
					typedef typename inp_t::template equal_< T,typename get_extract_type_<P,inp_t>::type >::type type;
				};
			};

			template<typename T = control_::empty_> struct return_ {
				template<typename inp_t> struct input {
					static_assert(is_out_<inp_t>::value, "input only receive out_");
					typedef typename inp_t::template insert_<control_::return_, typename get_extract_type_<T, inp_t>::type>::type type;
				};
			};

			template<template<typename...> class tank, typename ...AT> struct depute_ {
				template<typename in> struct extract {
					typedef typename instant<
						tank,
						typename get_extract_type_<AT, in>::type...
					>::type type;
				};
			};

			template<template<typename...> class tank, typename ...AT> struct depute_itself_ {
				template<typename in> struct extract {
					typedef tank< typename get_extract_type_<AT, in>::type ...     > type;
				};
			};

			template<size_t ,typename Up,typename ...T> struct packing_parameter_{
				static_assert(is_map_<Up>::value, "packing_parameter_ only receive map_");
				typedef Up type;
			};
			template<size_t k, typename Up,typename P, typename ...T> struct packing_parameter_<k,Up,P,T...> {
				static_assert(is_map_<Up>::value, "packing_parameter_ only receive map_");
				typedef typename packing_parameter_<k+1,typename Up::template insert<in_<k>,P>::type,T...>::type type;
			};


			template<typename T, typename ...P> struct call_{
				static_assert(is_func_<T>::value,"call_ only receive func_");
				static_assert(is_func_parameter_indexflow<T,sizeof...(P)>::value,"parameter number is not correct");
				typedef typename parameter_stack_< typename packing_parameter_<0, map_<>, P...>::type >::push::type parameter;
				typedef typename out_<map_<>, parameter> input_type;
				typedef typename T::template input<input_type>::type out_put;
			public:
				typedef typename std::conditional<
					out_put::template have_attribute<control_::return_>::value,
					instant< out_put::template get_attribute,control_::return_ >,
					itself<control_::empty_>
				>::type::type type;
				
			};

			template<typename T, typename ...P> using call_t = typename call_<T, P...>::type;

			template<typename T, typename ...K> struct depute_call_ {
				template<typename in> struct extract {
					static_assert(is_out_<in>::value, "depute_call_::extract only receive out_");
					typedef typename call_<T, typename get_extract_type_<K,in>::type...>::type type;
				};
			};

			struct break_ {
				template <typename in> struct input {
					static_assert(is_out_<in>::value, "call_ only receive func_");
					typedef typename in::template insert_<control_::break_, control_::empty_>::type type;
				};
			};

			template<typename T, typename ...K> struct while_ {
				template<typename in> class input {
					static_assert(is_out_<in>::value, "while_ only receive out_");
					typedef typename in::push_::type::template erase_<control_::continue_>::type in_type;
					typedef typename std::conditional<
						get_extract_type_<T, in_type>::type::value &&
						!in_type::template have_attribute<control_::break_>::value &&
						!in_type::template have_attribute<control_::return_>::value,
						instant< statement_<K...>::template input, in_type>,
						itself< in_type >
					>::type::type out_1;
					typedef typename out_1::pop_::type out_2;
					typedef typename out_2::template erase_<control_::break_>::type out_type;
				public:
					typedef typename std::conditional<
						get_extract_type_<T, out_type>::type::value &&
						!out_2::template have_attribute<control_::break_>::value &&
						!out_2::template have_attribute<control_::return_>::value,
						instant< while_<T,K...>::template input, out_type>, 
						itself<out_type>
					>::type::type type;
				};
			};
		}
	}
}