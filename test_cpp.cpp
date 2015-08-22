#include "type_template_calculate.h"
using namespace PO::Tool::TTC; //�������ռ�
struct input_type {};

template<size_t i > using ic_ = std::integral_constant<size_t, i>;

typedef func_< 0 > empty_func;

template<typename T, typename K> struct add_ic_ { typedef ic_< T::value + K::value > type; };
template<typename T, typename K> struct mod_ic_ { typedef ic_< T::value % K::value > type; };
template<typename T, typename K> struct less_ic_ { static constexpr bool value = T::value < K::value; };


//�������ɶ�����ɣ�ֻ�������������������á�
void text_input_type(input_type) {}
int main() {
	/*
	typedef func_< 0 > empty_func;
	text_input_type(call_<empty_func>::type());
	*/

	/*
	typedef func_< 0, return_<ic_<1>> > empty_func001;
	//call_t����call_::type
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
	struct scri_1 {};//������
	struct scri_2 {};//������
	typedef func_< 0,
		alloc_< scri_1, ic_<0> >, //����һ����ʱ����
		alloc_< scri_2, ic_<1> >, //����һ����ʱ����
		equal_< scri_1, ic_<2> >, //��������Ϊscri_1�ı���������ic_<2>
		equal_< scri_2, para_< scri_1 > >, //��scri_1�������ı�����ȡ����������ֵ��scri_2
		return_< para_< scri_2 > > //��scri_2�����ı�����ȡ������������
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
			depute_itself_< std::is_same, in_<0>, ic_<0> >,//�����0
			return_< ic_<0> > //ֱ�ӷ���
		>::else_<
			alloc_ < cur_ic_, ic_<1> >, //����һ�������ŵ�ǰ��Լ��
			alloc_ < resoult_ic_, ic_<1> >, //����һ�������Ž����Լ��
			while_ < 
				depute_itself_< less_ic_ , para_< cur_ic_ >, in_<0> >, //������С��cur_ic_
				if_<
					//������Ϊ0
					depute_itself_< std::is_same , depute_< mod_ic_ , in_<0> , para_< cur_ic_ > > , ic_<0> >, 
					//��ֵresoult_ic_
					equal_< resoult_ic_ , para_< cur_ic_ > >
				>,
				//��cur_ic_��1
				equal_< cur_ic_ , depute_< add_ic_ , para_< cur_ic_ > , ic_<1> > >
			>,
			//����resoult_ic_
			return_< para_<resoult_ic_> >
		>
	> final_func;
	text_input_type(call_t<final_func, ic_<10>  >());
	text_input_type(call_t<final_func, ic_<21>  >());

}