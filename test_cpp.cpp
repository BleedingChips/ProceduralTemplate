#include "type_template_calculate.h"
using namespace PO::Tool::TTC; //主命名空间
struct input_type {};

template<size_t i > using ic_ = std::integral_constant<size_t, i>;

typedef func_< 0 > empty_func;

template<typename T, typename K> struct add_ic_ { typedef ic_< T::value + K::value > type; };
template<typename T, typename K> struct mod_ic_ { typedef ic_< T::value % K::value > type; };
template<typename T, typename K> struct less_ic_ { static constexpr bool value = T::value < K::value; };


//这个函数啥都不干，只是用来给编译器报错用。
void text_input_type(input_type) {}
int main() {
	/*
	typedef func_< 0 > empty_func;
	text_input_type(call_<empty_func>::type());
	*/

	/*
	typedef func_< 0, return_<ic_<1>> > empty_func001;
	//call_t等于call_::type
	text_input_type(call_t<empty_func001>());
	*/

	/*
	typedef func_< 0,
		if_< std::true_type,
			return_<ic_<1>>
		>::else_<
			return_<ic_<2>>
		>
	> func_if_001;

	typedef func_< 0,
		if_< std::false_type,
			return_<ic_<1>>
		>::else_<
			return_<ic_<2>>
		>
	> func_if_002;

	text_input_type(call_t<func_if_001>());
	text_input_type(call_t<func_if_002>());
	*/

	/*
	typedef func_< 0,
		if_< std::false_type,
		return_< ic_<0> >
		>,
		return_<ic_<1>>,
		return_<ic_<2>>
	> func_multy_return;

	text_input_type(call_t<func_multy_return>());
	*/

	/*
	struct scri_1 {};//描述符
	struct scri_2 {};//描述符
	typedef func_< 0,
		alloc_< scri_1, ic_<0> >, //创建一个临时变量
		alloc_< scri_2, ic_<1> >, //创建一个临时变量
		equal_< scri_1, ic_<2> >, //将描述符为scri_1的变量关联成ic_<2>
		equal_< scri_2, para_< scri_1 > >, //将scri_1所关联的变量提取出来，并赋值给scri_2
		return_< para_< scri_2 > > //将scri_2关联的变量提取出来，并返回
	> xxx_func;
	text_input_type(call_t<xxx_func>());
	*/

	/*
	typedef func_< 3,
		if_<  in_<2>,
			equal_< in_<0>, depute_< add_ic_, in_<0>, in_<1> > >
		>::else_<
			equal_< in_<0>, depute_< add_ic_, in_<0>, ic_<1> > >
		>,
		return_< in_<0> >
	> func_10086;
	text_input_type(call_t<func_10086, ic_<5>, ic_<9>, std::false_type  >());
	text_input_type(call_t<func_10086, ic_<5>, ic_<9>, std::true_type  >());
	*/

	struct cur_ic_ {};
	struct resoult_ic_ {};
	typedef func_< 1,
		if_<
			depute_itself_< std::is_same, in_<0>, ic_<0> >,//如果是0
			return_< ic_<0> > //直接返回
		>::else_<
			alloc_ < cur_ic_, ic_<1> >, //创建一个变量放当前的约数
			alloc_ < resoult_ic_, ic_<1> >, //创建一个变量放结果的约数
			while_ < 
				depute_itself_< less_ic_ , para_< cur_ic_ >, in_<0> >, //若变量小于cur_ic_
				if_<
					//若余数为0
					depute_itself_< std::is_same , depute_< mod_ic_ , in_<0> , para_< cur_ic_ > > , ic_<0> >, 
					//赋值resoult_ic_
					equal_< resoult_ic_ , para_< cur_ic_ > >
				>,
				//给cur_ic_加1
				equal_< cur_ic_ , depute_< add_ic_ , para_< cur_ic_ > , ic_<1> > >
			>,
			//返回resoult_ic_
			return_< para_<resoult_ic_> >
		>
	> final_func;
	text_input_type(call_t<final_func, ic_<10>  >());
	text_input_type(call_t<final_func, ic_<21>  >());

}