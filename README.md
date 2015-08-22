type_template_calculate
===

这个原本是用来将复杂的函数式模板元编程转换成过程式编程。但是写完之后发现其编译效率确实有点捉急，而且也并没有说十分有用，所以只能作为一个玩具放在GITHUB上了。或许哪天有兴致会来优化一下吧  （：P） 

以下代码只通过VS2015的编译器测试，MINGW等GUN系的编译器尚没有测试。（其实我只是懒而已）

完整代码见text_cpp.cpp

好吧，废话不多说，先上一些辅助用的代码：

```cpp
#include "type_template_calculate.h"
using namespace PO::Tool::TTC; //主命名空间
struct input_type {};

//这个函数啥都不干，只是用来给编译器报错用。
void text_input_type ( input_type ) {}
int main() {}
```

如果编译通过了，那么该库就可以使用了。为了方便，我们使用std::integral_constant来代表不同的类型（假设integral_constant<size_t,0>与integral_constant<size_t,1>是不同类型。。不过事实上也是）。为了方便，我们在以上代码的基础上再定义一些方便使用的类。

```cpp
#include "type_template_calculate.h"
using namespace PO::Tool::TTC;
struct input_type {};

template<size_t i > using ic_ = std::integral_constant<size_t, i>;

void text_input_type(input_type) {}
int main() {}
```

（ 以下的代码都是在上面的代码添加的基础上。 ）

好了，我们现在定义一个模板元函数，这个函数接受0个参数，里面啥也不干，并取名为empty_func。

```cpp
typedef func_< 0 > empty_func;
```

虽然这个函数接受0个参数，并且啥也不干，但是这个函数实际上会返回一个类型，这个类型是PO::Tool::control_::empty_。
我们在main函数里面试着调用一下：

```cpp
int main(){
	text_input_type(call_<empty_func>::type());
}
```

当然了，此时编译器会报错：
```cpp
 error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'PO::Tool::control_::empty_' to 'input_type'
```

好了，这个就是返回值。
如果需要更改其返回值，我们可以在func_内通过return_来设置，例如：

```cpp
typedef func_< 0 , return_<ic_<1>> > empty_func001;

//call_t等于call_::type
text_input_type(call_t<empty_func001>());
```

此时编译器报的错是：
```cpp
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,1>' to 'input_type'
```

成功了是不是？

当然了，如果只有这样的话那么这个库也是没啥用的。我们加上条件返回，这里面就需要if - else 语句了，例如：

```cpp
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
```

此时报的错分别是：

```cpp
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,1>' to 'input_type'
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,2>' to 'input_type'
```

那么多个return_会不会干扰呢？

```cpp
typedef func_< 0 ,
			if_< std::false_type ,
				return_< ic_<0> >
			>,
			return_<ic_<1>>,
			return_<ic_<2>>
> func_multy_return;

text_input_type(call_t<func_multy_return>());
```

报错为:
```cpp
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,1>' to 'input_type'
```

以下高能
===

函数中允许临时变量的存在。在函数中，每一个变量都必须由两个类型来进行描述。一个是描述符，是不可变的，函数将会通过该描述符来提取变量。而另一个将是保存的此时该描述符所关联的类型。描述符与其关联的类型可以相同。可以对临时变量进行赋值和返回：

```cpp
struct scri_1{};//描述符
struct scri_2{};//描述符
typedef func_< 0, 
		alloc_< scri_1 , ic_<0> >, //创建一个临时变量
		alloc_< scri_2 , ic_<1> >, //创建一个临时变量
		equal_< scri_1 , ic_<2> >, //将描述符为scri_1的变量关联成ic_<2>
		equal_< scri_2 , para_< scri_1 > >, //将scri_1所关联的变量提取出来，并关联给scri_2
		return_< para_< scri_2 > > //将scri_2关联的变量提取出来，并返回
> xxx_func;
text_input_type(call_t<xxx_func>());

```

报错为：

```cpp
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,2>' to 'input_type'
```

函数的参数能用para_来提取出来。另外的，通过修改func_的第一个参数，能够改变函数所支持的参数的个数。而输入的参数，自动以in_来描述。

以下定义一个函数，输入三个参数，一个ic_类型，一个bool类型（true_type或者false_type）。如果是true，则两个相加，如果是false，则将只加1。

先定义一个新的辅助类操作：

```cpp
template<typename T, typename K> struct add_ic_ { typedef ic_< T::value + K::value > type; };
```

接下来编写代码：
```cpp
typedef func_< 3, //接受的参数，不能多也不能少
		if_<  in_<2>,//提取第三个参数作为这个值

			//这里的depute_会将in_<0>，也就是第一个参数提取出来，将第二参数提取出来，并构造一个add_ic_< xx,yy >，并取add_ic_< xx,yy >::type作为这个表达式的右值。如果是depute_itself_将用add_ic_< xx,yy >作为这个表达式的右值
			equal_< in_<0>, depute_< add_ic_, in_<0>, in_<1> > >
		>::else_<
		
			//这里的depute_会将in_<0>，也就是第一个参数提取出来，与ic_<1>一起构造一个add_ic_< xx,ic_<1> >，并取add_ic_< xx,ic_<1> >::type作为这个表达式的右值。如果是depute_itself_将用add_ic_< xx,ic_<1> >作为这个表达式的右值
			equal_< in_<0>, depute_< add_ic_, in_<0>, ic_<1> > >
		>,
		return_< in_<0> >
	> func_10086;
text_input_type(call_t<func_10086, ic_<5>, ic_<9>, std::false_type  >());
text_input_type(call_t<func_10086, ic_<5>, ic_<9>, std::true_type  >());
```

此对应的错误信息为：
```cpp
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,6>' to 'input_type'
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,14>' to 'input_type'
```

函数里面支持循环，但是因为本人懒所以只有while循环，while循环内部支持continue和break操作，与在C语言中的意义一样，例如如下代码将计算ic_的最大约数。
先定义一个操作函数：
```cpp
template<typename T, typename K> struct mod_ic_ { typedef ic_< T::value % K::value > type; };
template<typename T, typename K> struct less_ic_ { static constexpr bool value = T::value < K::value; };
```

下面是正式的代码：
```cpp
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
```

错误信息为：
```cpp
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,5>' to 'input_type'
error C2664: 'void text_input_type(input_type)': cannot convert argument 1 from 'std::integral_constant<size_t,7>' to 'input_type'
```

当然啦，还可以用
```cpp
depute_call_<final_func,para_<...> > 
```
在其他的func_里面调用这个func_

