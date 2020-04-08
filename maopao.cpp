#include "type_template_calculate.h"
#include <iostream>
#include <tuple>

using namespace PO::Tool::TTC;
using namespace std;

template<size_t i > using ic = std::integral_constant<size_t, i>;
template<typename T, typename K> struct ic_less;
template<size_t i, size_t k> struct ic_less<ic<i>, ic<k>> : std::conditional_t < i<k, std::true_type, std::false_type>{};
template<typename T> struct ic_add_one;
template<size_t i> struct ic_add_one<ic<i>> { using type = ic<i+1>; };
template<typename T> using ic_add_one_t = typename ic_add_one<T>::type;

template<typename T, typename Index>
struct input_get;
template<typename T, size_t i>
struct input_get<T, ic<i>> { using type = std::tuple_element_t<i, T>; };
template<typename T, typename Index> using input_get_t = typename input_get<T, Index>::type;


template<typename Replace, typename Output, typename Input, size_t ite, size_t target>
struct input_set_imp;

template<typename Replace, typename ...Output, typename Cur, typename ...Input, size_t ite, size_t target>
struct input_set_imp<Replace, std::tuple<Output...>, std::tuple<Cur, Input...>, ite, target> {
	using type = typename input_set_imp<Replace, std::tuple<Output..., Cur>, std::tuple<Input...>, ite + 1, target>::type;
};

template<typename Replace, typename ...Output, typename Cur, typename ...Input, size_t target>
struct input_set_imp<Replace, std::tuple<Output...>, std::tuple<Cur, Input...>, target, target> {
	using type = typename input_set_imp<Replace, std::tuple<Output..., Replace>, std::tuple<Input...>, target + 1, target>::type;
};

template<typename Replace, typename ...Output, size_t ite, size_t target>
struct input_set_imp<Replace, std::tuple<Output...>, std::tuple<>, ite, target> {
	using type = std::tuple<Output...>;
};

template<typename T, typename Input, typename Index>
struct input_set;
template<typename T, typename Input, size_t i>
struct input_set<T, Input, ic<i>> { using type = typename input_set_imp<Input, std::tuple<>, T, 0, i>::type; };
template<typename T, typename Input, typename Index> using input_set_t = typename input_set<T, Input, Index>::type;


struct Temporary1 {};
struct Temporary2 {};

// 定义一个函数，接受三个输入，第一个是数列，第二，三个是要判断的位置，如果大于则交换
using ElementSort = func_<3,
	if_<
		// depute_itself_<input_get_t, in_<0>, in_<2> -> 提取第0，第2个参数，然后传给input_get_t，并返回结果。
		// depute_itself_<input_get_t, in_<0>, in_<1> -> 提取第0，第1个参数，然后传给input_get_t，并返回结果。、
		// depute_itself_<ic_less, 提取上述两个结果，然后传给 ic_less
		//该行代码表示如果元素的第N+1项小于第N项，则继续执行下面的代码
		depute_itself_<ic_less, depute_itself_<input_get_t, in_<0>, in_<2>>, depute_itself_<input_get_t, in_<0>, in_<1>>>,
		// 创建临时变量Temporary1和Temporary2
		alloc_<Temporary1, depute_itself_<input_get_t, in_<0>, in_<1>>>,
		alloc_<Temporary2, depute_itself_<input_get_t, in_<0>, in_<2>>>,
		// 将两个临时变量赋值给数列
		equal_<in_<0>, depute_itself_<input_set_t, in_<0>, para_<Temporary1>, in_<2>>>,
		equal_<in_<0>, depute_itself_<input_set_t, in_<0>, para_<Temporary2>, in_<1>>>
	>,
	// 返回第0个参数
	return_<in_<0>>
>;

struct Ite {};
struct Ite1 {};
struct Ite2 {};

// 定义一个函数，接受两个输入，第一个是数列，第二个是数列元素的个数
using Sort = func_ < 2,
	// 如果数列的长度小于两个
	if_<depute_itself_<ic_less, in_<1>, ic<2>>,
		//直接返回
		return_<in_<0>>
	//否则
	>::template else_<
		// 创建临时变量Ite
		alloc_<Ite, ic<0>>,
		// 进入循环
		while_<
			// 如果 Ite < 参数1，则继续循环
			depute_itself_<ic_less, para_<Ite>, in_<1>>,
			// 创建临时变量Ite1与Ite2
			alloc_<Ite1, ic<0>>,
			alloc_<Ite2, ic<1>>,
			// 进入循环
			while_<
				// 如果临时变量 Ite2 小于参数1，则继续循环
				depute_itself_<ic_less, para_<Ite2>, in_<1>>,
				// 用数列，参数Ite1，参数Ite2为参数，调用函数ElementSort，然后将结果传给参数0
				equal_<in_<0>, depute_call_<ElementSort, in_<0>, para_<Ite1>, para_<Ite2>>>,
				// 临时变量 Ite1 +=1; Ite2+=1;
				equal_<Ite1, depute_itself_<ic_add_one_t, para_<Ite1>>>,
				equal_<Ite2, depute_itself_<ic_add_one_t, para_<Ite1>>>
			>,
			// Ite +=1;
			equal_<Ite, depute_itself_<ic_add_one_t, para_<Ite>>>
		>,
		// 返回参数0
		return_<in_<0>>
	>
>;


using input = std::tuple<ic<6>, ic<4>, ic<2>, ic<0>, ic<5>, ic<3>, ic<1>>;

template<typename T> void print(T&& t, ic<0>){}

template<typename T, size_t k> void print(T&& t, ic<k>)
{
	print(t, ic<k - 1>{});
	std::cout << std::tuple_element_t<k - 1, std::remove_reference_t<T>>::value << ", ";
}

template<typename T> void print(T&& t)
{
	print(t, ic<std::tuple_size_v<T>>{});
}

int main()
{
	print(input{});
	std::cout << std::endl << "=====" << std::endl;
	print(call_t<Sort, input, ic<std::tuple_size_v<input>>>{});
	std::cout << std::endl;
}