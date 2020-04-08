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

// ����һ�������������������룬��һ�������У��ڶ���������Ҫ�жϵ�λ�ã���������򽻻�
using ElementSort = func_<3,
	if_<
		// depute_itself_<input_get_t, in_<0>, in_<2> -> ��ȡ��0����2��������Ȼ�󴫸�input_get_t�������ؽ����
		// depute_itself_<input_get_t, in_<0>, in_<1> -> ��ȡ��0����1��������Ȼ�󴫸�input_get_t�������ؽ������
		// depute_itself_<ic_less, ��ȡ�������������Ȼ�󴫸� ic_less
		//���д����ʾ���Ԫ�صĵ�N+1��С�ڵ�N������ִ������Ĵ���
		depute_itself_<ic_less, depute_itself_<input_get_t, in_<0>, in_<2>>, depute_itself_<input_get_t, in_<0>, in_<1>>>,
		// ������ʱ����Temporary1��Temporary2
		alloc_<Temporary1, depute_itself_<input_get_t, in_<0>, in_<1>>>,
		alloc_<Temporary2, depute_itself_<input_get_t, in_<0>, in_<2>>>,
		// ��������ʱ������ֵ������
		equal_<in_<0>, depute_itself_<input_set_t, in_<0>, para_<Temporary1>, in_<2>>>,
		equal_<in_<0>, depute_itself_<input_set_t, in_<0>, para_<Temporary2>, in_<1>>>
	>,
	// ���ص�0������
	return_<in_<0>>
>;

struct Ite {};
struct Ite1 {};
struct Ite2 {};

// ����һ�������������������룬��һ�������У��ڶ���������Ԫ�صĸ���
using Sort = func_ < 2,
	// ������еĳ���С������
	if_<depute_itself_<ic_less, in_<1>, ic<2>>,
		//ֱ�ӷ���
		return_<in_<0>>
	//����
	>::template else_<
		// ������ʱ����Ite
		alloc_<Ite, ic<0>>,
		// ����ѭ��
		while_<
			// ��� Ite < ����1�������ѭ��
			depute_itself_<ic_less, para_<Ite>, in_<1>>,
			// ������ʱ����Ite1��Ite2
			alloc_<Ite1, ic<0>>,
			alloc_<Ite2, ic<1>>,
			// ����ѭ��
			while_<
				// �����ʱ���� Ite2 С�ڲ���1�������ѭ��
				depute_itself_<ic_less, para_<Ite2>, in_<1>>,
				// �����У�����Ite1������Ite2Ϊ���������ú���ElementSort��Ȼ�󽫽����������0
				equal_<in_<0>, depute_call_<ElementSort, in_<0>, para_<Ite1>, para_<Ite2>>>,
				// ��ʱ���� Ite1 +=1; Ite2+=1;
				equal_<Ite1, depute_itself_<ic_add_one_t, para_<Ite1>>>,
				equal_<Ite2, depute_itself_<ic_add_one_t, para_<Ite1>>>
			>,
			// Ite +=1;
			equal_<Ite, depute_itself_<ic_add_one_t, para_<Ite>>>
		>,
		// ���ز���0
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