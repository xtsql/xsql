#pragma once
#ifndef _dynamic_type_h
#define _dynamic_type_h

/**
* 版本： 1.1.0
* 说明：
*	本库文件用于被外部代码include。
*	通过 var 类提供对动态数据类型的支持。
* 
*	当前支持的类型有：
* 		bool, char, short, int, long long, float, double, std::string, std::vector<unsigned char>
*
*	此外，正在测试中（不可使用）的类型有：
*		自定义时间类型 var::daytime_t（一日中的时间，精确到秒），取值范围 0:0:0 - 23:59:59（使用struct） 
*		自定义日期类型 var::date_t（年月日信息），年份范围为 short 取值范围，月、日分别为[1,12] [1,31]
*		unsigned int
*		unsigned long long
*		time_t （时间戳类型，其存储实例为 long long 型，二者仅在to_string与type上有区别）
*	如果存在空类型，类型名称为 var::Type_t::none
*
*	当前存在的异常列表，带*的异常可以通过宏关闭这些异常的抛出：
*		var						运算中使用了不支持的类型
*		* std::invalid_argument	传入了非法参数（如使用空指针初始化）。会导致运算结果变为空对象
*		* std::range_error		对 array-like 使用非法下标时可能抛出
*		* var::opNone			尝试对none型变量进行操作。比如在 - * / 操作中会抛出此异常。
*		* var::ViA				尝试对无法转换为数值型的 array-like 变量进行数值型操作。
								如果禁用，在运算中视为none，使用类型转换则转换为0）
*		* var::USF				使用了不支持的方法。如对数值型使用了len()方法
*		* double				尝试对float型或double型进行取余相关运算时报出（并返回0）
*	需要注意的是，除法可能会抛出除零异常。通过控制宏可以配置是抛出异常还是返回none
* 
*	当前支持重载的运算符：（测试中的类型并不支持）
*		算术运算（全部支持）： +  -   *   /   %  ++  --
*		关系运算（未 支 持）： 未支持的有： ==  !=  >  <  >=  <=
*		逻辑运算（全部支持）： &&  ||  !
*		位 运 算（未 支 持）： &   |   ^  ~  <<  >>
*		赋值运算（部分支持）： =  +=  -=  *=  /=  %=  未支持的有：   <<=  >>=  &=  ^=  |=
*			a op= b 的操作符等效于 a = a op b，例如，即使 b=1 且 op 为 + 也不执行为 a++
* 
*	特殊数据类型的方法（暂未实现）：
*		需要注意的是，如果采用了不支持的特殊方法，可能抛出USF异常
*		std::string 与 Bytes 支持特殊方法有：
*			int obj.len()	返回字符串/二进制串长度（根据类型决定）
*			obj[int pos]	返回pos所对应的字符下标（可为只读或读写）。如果下标越界，由对应STL类处理异常
*			var obj.sub(int start = 0,int end = 0, int step = 1)
*				返回obj下标在 [start,end) 之间的子串，并且相邻两个字符坐标差为step。
				关于坐标：正数表示从字符串头部开始，负数表示从字符串尾部开始。为0时表示包括头、尾部端点字符（即规则与python相同）
				如果 end - start 与 step 异号（不包括坐标均为0），或下标超限、step为0，则可抛出 std::range_error 并返回none
*		std::string 独占的方法：
*			int obj.find(const string& str, const int start = 0)	从start开始（包括）寻找obj是否包括str子串，是则返回起始下标，否返回-1
*		var::time_t 独占的方法：
*			char obj.hour() obj.minute() obj.second()，获取只读的时分秒
*			void obj.set_hour(char x) obj.set_minute(char x) obj.set_second(char x)，写入时分秒，会进行类型校验
*/

// 控制是否支持算术运算符的重载（含对应赋值运算），注释掉以关闭此功能
#define DYNAMIC_VAR_ENABLE_ARITH_OP true

// 在定义了 DYNAMIC_VAR_ENABLE_ARITH_OP 下：
// 不启用此宏时将仅重载含 var 对象的运算
// 启用此宏则重载【数值型】与【字符串、字节数组型】运算以产生 var 对象，注释掉以关闭此功能
// 本特性暂不支持
//#define DYNAMIC_VAR_ENABLE_ALL_ARITH_OP true

// 控制是否支持逻辑运算符的重载（含对应赋值运算），注释掉以关闭此功能
#define DYNAMIC_VAR_ENABLE_LOGIC_OP true

// 控制是否支持位运算符的重载（含对应赋值运算），注释掉以关闭此功能
#define DYNAMIC_VAR_ENABLE_BIT_OP true

// 控制是否支持赋值运算符的重载（不含 = ），注释掉以关闭此功能
#define DYNAMIC_VAR_ENABLE_ASSIGN_OP true

//取消注释以关闭 std::invalid_argument型 异常的抛出
//#define DYNAMIC_VAR_MUTE_INVALID_ARG

//取消注释以关闭 opNone型 异常的抛出
//#define DYNAMIC_VAR_MUTE_OPNONE

//取消注释以关闭 ViA型 异常的抛出
//#define DYNAMIC_VAR_MUTE_VIA

//取消注释以以关闭 浮点%浮点 时 double型 异常的抛出
#define DYNAMIC_VAR_MUTE_DOUBLE

//取消注释以关闭 整型除零 异常的抛出，改为返回 none 变量
#define DYNAMIC_VAR_MUTE_INT_DIV_ZERO

//取消注释以将 0/0 由 nan 改为返回 none 变量
//#define DYNAMIC_VAR_NAN2NONE

//取消注释以将 x/0 由 inf 改为返回 none 变量
//#define DYNAMIC_VAR_INF2NONE



#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

// 使用STL中的vector托管字节流
typedef std::vector<unsigned char> Bytes;

// 本个函数用于尝试一个string是否能被转换为整型或double
//  - str : 待转换的字符串。类似"1,23" "1.ab"等效，均可转换为1
//  - int_result : 可以转换为任何int型时，填入转换结果，否则填入0
// 返回值：
//  -> 1:可以转换为double
//  -> 3:可以转换为int64
//  -> 4:可以转换为int
//  -> 5:可以转换为short
//  -> 6:可以转换为char
//	-> 0:不可以
char test_to_num(const std::string& str, long long& int_result);

//本函数用于测试是否为纯十进制数字
// 满足： x.y x.y x x 形式（前面可带 + - 且两端无空格的）认为是纯数字
// 返回值：
// -> 1:整型数字（但很可能是超长数字）。小数点在末尾也认为是整型
// -> 2:浮点型数字，-.x与.x，0.0也认为是浮点
// -> 0:非数字
char test_all_num(std::string str);

// 实现动态数据类型的主要类。
// 由于关系数据库不允许数组存在，因此，不支持除了字节流外的数组类型。
// 数组可以使用 var[]实现，而var内本身并不支持数组
class var
{
public:
	// 可选数据类型定义表
	enum class Type_t : char
	{
		none = 0,			// 空类型，即null
		bool_kind = 1,		// 布尔值
		int8_kind = 2,		// 8位有符号整型
		int16_kind = 3,		// 16位有符号整型
		int32_kind = 4,		// 32位有符号整型
		int64_kind = 5,		// 64位有符号整型
		double_kind = 6,	// 双精度浮点型
		float_kind = 7,		// 单精度浮点型
		daytime_kind = 16,	// 时间型（一日内的HH:MM:SS）
		string_kind = 65,	// 字符串（以 \0 结尾）
		bytes = 66			// 字节数组（可包含任意符号）

	};
	//时间类型
	struct daytime_t
	{
		char hour;
		char minute;
		char second;
	};

	/* 异常处理开始 */
		// var中存放的为数组型（如string与Bytes）时
		// 尝试直接读取为基本类型的过程中引发的错误
		struct ViA : public std::exception
		{
			const char* what()const throw()
			{
				return "Cannot get single value from Array-like var";
			}
		};
		// var中存放的为数组型（如string与Bytes）时
		// 尝试直接读取为基本类型的过程中引发的错误
		struct USF : public std::exception
		{
			const char* what()const throw()
			{
				return "unsupported function";
			}
		};
		// 尝试操作 none 时引发的错误
		// 应当使用 is_none() 方法来判断是否为 none
		struct opNone : public std::exception
		{
			const char* what()const throw()
			{
				return "Cannot operate none-object";
			}
		};
	/* 异常处理结束 */


	/* 构造函数开始 */
		// 默认构造函数，得到none型
		var();
		// 拷贝构造函数
		var(const var& old);
		// 指定类型，使用类型的默认值初始化的构造函数
		var(const var::Type_t typ);
		// 带初始值的构造函数
		var(bool data);
		var(int data);
		var(char data);
		var(short data);
		var(long long data);
		var(double data);
		var(float data);
		var(const var::daytime_t& data);
		var(char hour, char min, char sec = 0);
		var(const std::string& data);
		var(const char* data);
		var(const Bytes& data);
		template<int len>
		var(const unsigned char (&data)[len])
		{
			this->_type = Type_t::bytes;
			this->_data.bytes_d = new Bytes(len);
			for (int i = 0; i < len; ++i)
				(*(this->_data.bytes_d))[i] = data[i];
		}
	/* 构造函数结束 */


	/* 析构函数开始 */
		// 析构函数
		~var();
	/* 析构函数结束 */


	/* 类型判别函数开始 */
		// 读取当前类型
		Type_t type()const;
		// 读取当前类型名称
		std::string type_name()const;
		static std::string type_name(const var& obj);
		// 判断是否为none
		bool is_none()const;
	/* 类型判别函数结束 */


	/* 自动类型转换函数开始 */
		//只保留转向string、double的自动类型转换（需要注意long long 转double很可能降低精度）
		
		//所有类型均可使用此转换。如果字节流类型末尾非0，则其大小将+1
		//none的转换结果为空字符串""
		operator std::string(void);
		//none 类型将转换得到0。string型、bytes型则尝试解析为浮点数,解析失败可能报ViA异常
		operator double(void);
		//整型的非零、array型的非空均为true，其余为false。
		//浮点型的转换规则跟随编译器
		// none也为false
		operator bool(void);
	/* 自动类型转换函数结束 */


	/* 强制类型转换方法开始 */
		bool to_bool()const;
		char to_char()const;
		short to_short()const;
		int to_int()const;
		long long to_int64()const;
		double to_double()const;
		float to_float()const;
		//none的转换结果为空字符串""
		//bool的转换结果为 True 或 False
		std::string to_string()const;
		//此方法遇到none返回空对象，其余非Bytes类型将解析为String后转译。
		Bytes to_bytes()const;
	/* 强制类型转换方法结束 */


		/* 赋值运算重载开始 */
		var operator=(const var& val);
		var operator=(bool val);
		var operator=(char val);
		var operator=(short val);
		var operator=(int val);
		var operator=(long long val);
		var operator=(double val);
		var operator=(float val);
		// 创建为 string 型
		var operator=(const std::string& val);
		// 创建为 string 型
		var operator=(const char* val);
		// 创建为 byte_stream 型
		var operator=(const Bytes& val);
		// 创建为 byte_stream 型
		template <int len>
		var operator=(const unsigned char(&val)[len])
		{
			this->clear();
			this->_type = Type_t::bytes;
			this->_data.bytes_d = new Bytes(len);
			for (int i = 0; i < len; ++i)
				(*this->_data.bytes_d)[i] = val[i];
			return *this;
		}
	/* 赋值运算重载结束*/

#ifdef DYNAMIC_VAR_ENABLE_ARITH_OP
	/* 算术运算重载开始 */

		// 加法运算较为复杂
		// bool转为char参与运算
		// none 型与其他对象相加均得到另一对象（两个none相加得到none）
		// op1 为string型则必得string（字符串合并）。none对象视为空而不是“none”文本
		// 浮点与整型运算必得浮点
		// 两个Bytes相加为二者合并
		// Bytes + string 得到Bytes，Bytes + 数值视Bytes所表示的文本是否为数值
		friend var operator+(const var& op1, const var& op2);
		friend var operator+(const var& op1, const bool op2);
		friend var operator+(const bool op1, const var& op2);
		friend var operator+(const var& op1, const char op2);
		friend var operator+(const char op1, const var& op2);
		friend var operator+(const var& op1, const short op2);
		friend var operator+(const short op1, const var& op2);
		friend var operator+(const var& op1, const int op2);
		friend var operator+(const int op1, const var& op2);
		friend var operator+(const var& op1, const long long op2);
		friend var operator+(const long long op1, const var& op2);
		friend var operator+(const var& op1, const float op2);
		friend var operator+(const float op1, const var& op2);
		friend var operator+(const var& op1, const double op2);
		friend var operator+(const double op1, const var& op2);
		friend var operator+(const var& op1, const std::string& op2);
		friend var operator+(const std::string& op1, const var& op2);
		friend var operator+(const var& op1, const char* const op2);
		friend var operator+(const char* const op1, const var& op2);
		friend var operator+(const var& op1, const Bytes& op2);
		friend var operator+(const Bytes& op1, const var& op2);
		template <int len>
		friend var operator+(const var& op1, const unsigned char(&op2)[len])
		{
			return op1 + (var)op2;
		}
		template <int len>
		friend var operator+(const unsigned char(&op1)[len], const var& op2)
		{
			return (var)op1 + op2;
		}

		//减法运算较为简单：
		//仅支持数值减法，若传入字符串/字节数组则无法转换为数值时返回none，并可抛出ViA异常
		// none不可作为减法中的任一操作数，否则可抛出opNone异常
		// 类型转换的顺序为
		// char -> short -> int   -> int64
		//                 float  ->double
		friend var operator-(const var& op1, const var& op2);
		friend var operator-(const var& op1, const bool op2);
		friend var operator-(const bool op1, const var& op2);
		friend var operator-(const var& op1, const char op2);
		friend var operator-(const char op1, const var& op2);
		friend var operator-(const var& op1, const short op2);
		friend var operator-(const short op1, const var& op2);
		friend var operator-(const var& op1, const int op2);
		friend var operator-(const int op1, const var& op2);
		friend var operator-(const var& op1, const long long op2);
		friend var operator-(const long long op1, const var& op2);
		friend var operator-(const var& op1, const float op2);
		friend var operator-(const float op1, const var& op2);
		friend var operator-(const var& op1, const double op2);
		friend var operator-(const double op1, const var& op2);
		friend var operator-(const var& op1, const std::string& op2);
		friend var operator-(const std::string& op1, const var& op2);
		friend var operator-(const var& op1, const char* const op2);
		friend var operator-(const char* const op1, const var& op2);
		friend var operator-(const var& op1, const Bytes& op2);
		friend var operator-(const Bytes& op1, const var& op2);
		template <int len>
		friend var operator-(const var& op1, const unsigned char(&op2)[len])
		{
			return op1 - (var)op2;
		}
		template <int len>
		friend var operator-(const unsigned char(&op1)[len], const var& op2)
		{
			return (var)op1 - op2;
		}

		//乘法运算规则类似减法运算：
		//仅支持数值乘法，若传入字符串/字节数组则无法转换为数值时返回none（除法下文特例），并可抛出ViA异常
		// none不可作为乘法中的任一操作数，否则可抛出opNone异常
		// 当 op1 为array且op2可转换为整型（x.0不视为整型）且非负时，运算结果为将 op1 重复 op2 次（op2为0得到空字符串）
		// 如果op2可转整型，op1为array必判定是否为重复op2次（op2不为非负整型时抛ViA）
		// 类型转换的顺序为
		// char -> short -> int   -> int64
		//                 float  ->double
		friend var operator*(const var& op1, const var& op2);
		friend var operator*(const var& op1, const bool op2);
		friend var operator*(const bool op1, const var& op2);
		friend var operator*(const var& op1, const char op2);
		friend var operator*(const char op1, const var& op2);
		friend var operator*(const var& op1, const short op2);
		friend var operator*(const short op1, const var& op2);
		friend var operator*(const var& op1, const int op2);
		friend var operator*(const int op1, const var& op2);
		friend var operator*(const var& op1, const long long op2);
		friend var operator*(const long long op1, const var& op2);
		friend var operator*(const var& op1, const float op2);
		friend var operator*(const float op1, const var& op2);
		friend var operator*(const var& op1, const double op2);
		friend var operator*(const double op1, const var& op2);
		friend var operator*(const var& op1, const std::string& op2);
		friend var operator*(const std::string& op1, const var& op2);
		friend var operator*(const var& op1, const char* const op2);
		friend var operator*(const char* const op1, const var& op2);
		friend var operator*(const var& op1, const Bytes& op2);
		friend var operator*(const Bytes& op1, const var& op2);
		template <int len>
		friend var operator*(const var& op1, const unsigned char(&op2)[len])
		{
			return op1 * (var)op2;
		}
		template <int len>
		friend var operator*(const unsigned char(&op1)[len], const var& op2)
		{
			return (var)op1 * op2;
		}

		//除法运算类似减法：
		//仅支持数值除法，若传入字符串/字节数组则无法转换为数值时返回none，并可抛出ViA异常
		// none不可作为除法中的任一操作数，否则可抛出opNone异常
		// 类型转换的顺序为
		// char -> short -> int   -> int64
		//                 float  ->double
		friend var operator/(const var& op1, const var& op2);
		friend var operator/(const var& op1, const bool op2);
		friend var operator/(const bool op1, const var& op2);
		friend var operator/(const var& op1, const char op2);
		friend var operator/(const char op1, const var& op2);
		friend var operator/(const var& op1, const short op2);
		friend var operator/(const short op1, const var& op2);
		friend var operator/(const var& op1, const int op2);
		friend var operator/(const int op1, const var& op2);
		friend var operator/(const var& op1, const long long op2);
		friend var operator/(const long long op1, const var& op2);
		friend var operator/(const var& op1, const float op2);
		friend var operator/(const float op1, const var& op2);
		friend var operator/(const var& op1, const double op2);
		friend var operator/(const double op1, const var& op2);
		friend var operator/(const var& op1, const std::string& op2);
		friend var operator/(const std::string& op1, const var& op2);
		friend var operator/(const var& op1, const char* const op2);
		friend var operator/(const char* const op1, const var& op2);
		friend var operator/(const var& op1, const Bytes& op2);
		friend var operator/(const Bytes& op1, const var& op2);
		template <int len>
		friend var operator/(const var& op1, const unsigned char(&op2)[len])
		{
			return op1 / (var)op2;
		}
		template <int len>
		friend var operator/(const unsigned char(&op1)[len], const var& op2)
		{
			return (var)op1 / op2;
		}

		//取余运算中：
		// 两种操作数均仅支持整型
		// 如传入 float/double型则可抛出对应类型异常，并返回none
		// 若传入字符串/字节数组则无法转换为整型时返回none，并可抛出ViA异常
		// none不可作为任一操作数，否则可抛出opNone异常
		// 需要注意的是，如果均抛出异常，优先级为：
		// op1 ViA，op1 double，op2 ViA，op2 double
		// 异常返回均为none
		// 类型转换的顺序为
		// char -> short -> int   -> int64
		//                 float  ->double
		friend var operator%(const var& op1, const var& op2);
		friend var operator%(const var& op1, const bool op2);
		friend var operator%(const bool op1, const var& op2);
		friend var operator%(const var& op1, const char op2);
		friend var operator%(const char op1, const var& op2);
		friend var operator%(const var& op1, const short op2);
		friend var operator%(const short op1, const var& op2);
		friend var operator%(const var& op1, const int op2);
		friend var operator%(const int op1, const var& op2);
		friend var operator%(const var& op1, const long long op2);
		friend var operator%(const long long op1, const var& op2);
		friend var operator%(const var& op1, const std::string& op2);
		friend var operator%(const std::string& op1, const var& op2);
		friend var operator%(const var& op1, const char* const op2);
		friend var operator%(const char* const op1, const var& op2);
		friend var operator%(const var& op1, const Bytes& op2);
		friend var operator%(const Bytes& op1, const var& op2);
		template <int len>
		friend var operator%(const var& op1, const unsigned char(&op2)[len])
		{
			return op1 % (var)op2;
		}
		template <int len>
		friend var operator%(const unsigned char(&op1)[len], const var& op2)
		{
			return (var)op1 % op2;
		}

		//自增自减运算
		//不自动进行类型转换。但浮点型仍然能以+1、-1进行
		//对于 bool型，++结果必为true，--结果必为false
		//允许的数据类型： bool char short int <long long> double float
		friend var& operator++(var& op);	// ++var
		friend var operator++(var& op, int place);	// var++
		friend var& operator--(var& op);	// --var
		friend var operator--(var& op, int place);	//var--

#ifdef DYNAMIC_VAR_ENABLE_ALL_ARITH_OP
#endif // DYNAMIC_VAR_ENABLE_ALL_ARITH_OP
#endif // DYNAMIC_VAR_ENABLE_ARITH_OP
	/* 算术运算重载结束 */


#ifdef DYNAMIC_VAR_ENABLE_LOGIC_OP
	/* 逻辑运算重载开始 */
		friend bool operator&&(const var& op1, const var& op2);
		friend bool operator&&(const var& op1, const bool op2);
		friend bool operator&&(const bool op1, const var& op2);
		friend bool operator&&(const var& op1, const char op2);
		friend bool operator&&(const char op1, const var& op2);
		friend bool operator&&(const var& op1, const short op2);
		friend bool operator&&(const short op1, const var& op2);
		friend bool operator&&(const var& op1, const int op2);
		friend bool operator&&(const int op1, const var& op2);
		friend bool operator&&(const var& op1, const long long op2);
		friend bool operator&&(const long long op1, const var& op2);
		friend bool operator&&(const var& op1, const float op2);
		friend bool operator&&(const float op1, const var& op2);
		friend bool operator&&(const var& op1, const double op2);
		friend bool operator&&(const double op1, const var& op2);
		friend bool operator&&(const var& op1, const std::string& op2);
		friend bool operator&&(const std::string& op1, const var& op2);
		friend bool operator&&(const var& op1, const char* const op2);
		friend bool operator&&(const char* const op1, const var& op2);
		friend bool operator&&(const var& op1, const Bytes& op2);
		friend bool operator&&(const Bytes& op1, const var& op2);
		template <int len>
		friend bool operator&&(const var& op1, const unsigned char(&op2)[len])
		{
			return op1 && (var)op2;
		}
		template <int len>
		friend bool operator&&(const unsigned char(&op1)[len], const var& op2)
		{
			return (var)op1 && op2;
		}
		friend bool operator||(const var& op1, const var& op2);
		friend bool operator||(const var& op1, const bool op2);
		friend bool operator||(const bool op1, const var& op2);
		friend bool operator||(const var& op1, const char op2);
		friend bool operator||(const char op1, const var& op2);
		friend bool operator||(const var& op1, const short op2);
		friend bool operator||(const short op1, const var& op2);
		friend bool operator||(const var& op1, const int op2);
		friend bool operator||(const int op1, const var& op2);
		friend bool operator||(const var& op1, const long long op2);
		friend bool operator||(const long long op1, const var& op2);
		friend bool operator||(const var& op1, const float op2);
		friend bool operator||(const float op1, const var& op2);
		friend bool operator||(const var& op1, const double op2);
		friend bool operator||(const double op1, const var& op2);
		friend bool operator||(const var& op1, const std::string& op2);
		friend bool operator||(const std::string& op1, const var& op2);
		friend bool operator||(const var& op1, const char* const op2);
		friend bool operator||(const char* const op1, const var& op2);
		friend bool operator||(const var& op1, const Bytes& op2);
		friend bool operator||(const Bytes& op1, const var& op2);
		template <int len>
		friend bool operator||(const var& op1, const unsigned char(&op2)[len])
		{
			return op1 || (var)op2;
		}
		template <int len>
		friend bool operator||(const unsigned char(&op1)[len], const var& op2)
		{
			return (var)op1 || op2;
		}
		friend bool operator!(const var& op);
	/* 逻辑运算重载结束 */
#endif // DYNAMIC_VAR_ENABLE_LOGIC_OP

#ifdef DYNAMIC_VAR_ENABLE_ASSIGN_OP
/* 赋值运算重载开始 */
	// 加
		var& operator+=(const var& op);
		var& operator+=(const bool op);
		var& operator+=(const char op);
		var& operator+=(const short op);
		var& operator+=(const int op);
		var& operator+=(const long long op);
		var& operator+=(const float op);
		var& operator+=(const double op);
		var& operator+=(const std::string& op);
		var& operator+=(const char* const op);
		var& operator+=(const Bytes& op);
		template <int len>
		var& operator+=(const unsigned char(&op)[len])
		{
			return *this += var(op);
		}
	// 减
		var& operator-=(const var& op);
		var& operator-=(const bool op);
		var& operator-=(const char op);
		var& operator-=(const short op);
		var& operator-=(const int op);
		var& operator-=(const long long op);
		var& operator-=(const float op);
		var& operator-=(const double op);
		var& operator-=(const std::string& op);
		var& operator-=(const char* const op);
		var& operator-=(const Bytes& op);
		template <int len>
		var& operator-=(const unsigned char(&op)[len])
		{
			return *this -= var(op);
		}
	// 乘
		var& operator*=(const var& op);
		var& operator*=(const bool op);
		var& operator*=(const char op);
		var& operator*=(const short op);
		var& operator*=(const int op);
		var& operator*=(const long long op);
		var& operator*=(const float op);
		var& operator*=(const double op);
		var& operator*=(const std::string& op);
		var& operator*=(const char* const op);
		var& operator*=(const Bytes& op);
		template <int len>
		var& operator*=(const unsigned char(&op)[len])
		{
			return *this *= var(op);
		}
	// 除
		var& operator/=(const var& op);
		var& operator/=(const bool op);
		var& operator/=(const char op);
		var& operator/=(const short op);
		var& operator/=(const int op);
		var& operator/=(const long long op);
		var& operator/=(const float op);
		var& operator/=(const double op);
		var& operator/=(const std::string& op);
		var& operator/=(const char* const op);
		var& operator/=(const Bytes& op);
		template <int len>
		var& operator/=(const unsigned char(&op)[len])
		{
			return *this /= var(op);
		}
	// 取余
		var& operator%=(const var& op);
		var& operator%=(const bool op);
		var& operator%=(const char op);
		var& operator%=(const short op);
		var& operator%=(const int op);
		var& operator%=(const long long op);
		var& operator%=(const std::string& op);
		var& operator%=(const char* const op);
		var& operator%=(const Bytes& op);
		template <int len>
		var& operator%=(const unsigned char(&op)[len])
		{
			return *this %= var(op);
		}
/* 赋值运算重载结束 */
#endif // DYNAMIC_VAR_ENABLE_ASSIGN_OP

private:
	//当前类型
	Type_t _type;
	//数据存储空间
	union _data_t
	{
		bool	bool_d;
		char	int8_d;
		short	int16_d;
		int		int32_d;
		long long int64_d;
		double	double_d;
		float	float_d;
		daytime_t	daytimd_d;
		std::string* string_d;
		Bytes* bytes_d;
	}_data;

	//适时清理指针对象的函数
	void clear();
	//判断是否为指针类型对象
	bool pointed();

	/* 运算符重载的内部处理函数 */
		
		//加法系列
		
		//已知op1类型，op2未知
		static inline var i8av(const char op1, const var& op2);
		static inline var i16av(const short op1, const var& op2);
		static inline var i32av(const int op1, const var& op2);
		static inline var i64av(const long long op1, const var& op2);
		static inline var fav(const float op1, const var& op2);
		static inline var dav(const double op1, const var& op2);
		static inline var sav(const std::string& op1, const var& op2);
		static inline var bav(const Bytes& op1, const var& op2);
		//由string/bytes转换得的整型数与数值型变量相加
		// op1的取值也会影响相加后的数值的类型
		// 如果op2不是数值型而直接传入，会抛var()错误并返回none
		static inline var ian(const long long op1, const var& op2);
		//同类型相加
		static inline var i8a(const char op1, const char op2);
		static inline var i16a(const short op1, const short op2);
		static inline var i32a(const int op1, const int op2);
		static inline var i64a(const long long op1, const long long op2);
		static inline var fa(const float op1, const float op2);
		static inline var da(const double op1, const double op2);
		static inline var ba(const Bytes& op1, const Bytes& op2);
		//int8与array
		static inline var i8as(const char op1, const std::string& op2);
		static inline var i8ab(const char op1, const Bytes& op2);
		//int16与array
		static inline var i16as(const short op1, const std::string& op2);
		static inline var i16ab(const short op1, const Bytes& op2);
		//int32与array
		static inline var i32as(const int op1, const std::string& op2);
		static inline var i32ab(const int op1, const Bytes& op2);
		//int64与array
		static inline var i64as(const long long op1, const std::string& op2);
		static inline var i64ab(const long long op1, const Bytes& op2);
		//double、float与array
		static inline var das(const double op1, const std::string& op2, const char size = 1);
		static inline var dab(const double op1, const Bytes& op2, const char size = 1);
		//string、bytes与ints
		//第三个参数取值取决于第二个参数的实际长度
		// char short int <long long> 依次为0-3
		static inline var bai(const Bytes& op1, const long long& op2, const char len = 2);
		//string、bytes与浮点
		//第三个参数取值取决于第二个参数的实际长度
		// float double 依次为0-1
		static inline var bad(const Bytes& op1, const double& op2, const char len = 1);


	//减法系列

		//已知op1类型，op2未知
		static inline var i8sv(const char op1, const var& op2);
		static inline var i16sv(const short op1, const var& op2);
		static inline var i32sv(const int op1, const var& op2);
		static inline var i64sv(const long long op1, const var& op2);
		static inline var fsv(const float op1, const var& op2);
		static inline var dsv(const double op1, const var& op2);
		static inline var ssv(const std::string& op1, const var& op2);
		static inline var bsv(const Bytes& op1, const var& op2);
		//由string/bytes转换得的整型数与数值型变量相减
		// op1的取值也会影响相减后的数值的类型
		// 如果op2不是数值型而直接传入，会抛var()错误并返回none
		static inline var isn(const long long op1, const var& op2);
		//同类型相减
		static inline var i8s(const char op1, const char op2);
		static inline var i16s(const short op1, const short op2);
		static inline var i32s(const int op1, const int op2);
		static inline var i64s(const long long op1, const long long op2);
		static inline var fs(const float op1, const float op2);
		static inline var ds(const double op1, const double op2);
		static inline var bs(const Bytes& op1, const Bytes& op2);
		static inline var ss(const std::string& op1, const std::string& op2);
		//int8与array
		static inline var i8ss(const char op1, const std::string& op2);
		static inline var i8sb(const char op1, const Bytes& op2);
		//int16与array
		static inline var i16ss(const short op1, const std::string& op2);
		static inline var i16sb(const short op1, const Bytes& op2);
		//int32与array
		static inline var i32ss(const int op1, const std::string& op2);
		static inline var i32sb(const int op1, const Bytes& op2);
		//int64与array
		static inline var i64ss(const long long op1, const std::string& op2);
		static inline var i64sb(const long long op1, const Bytes& op2);
		//double、float与array
		static inline var dss(const double op1, const std::string& op2, const char size = 1);
		static inline var dsb(const double op1, const Bytes& op2, const char size = 1);
		//string、bytes与ints
		//第三个参数取值取决于第二个参数的实际长度
		// char short int <long long> 依次为0-3
		static inline var ssi(const std::string& op1, const long long& op2, const char len = 2);
		static inline var bsi(const Bytes& op1, const long long& op2, const char len = 2);
		//string、bytes与浮点
		//第三个参数取值取决于第二个参数的实际长度
		static inline var ssd(const std::string& op1, const double& op2, const char len = 1);
		static inline var bsd(const Bytes& op1, const double& op2, const char len = 1);
		static inline var bss(const Bytes& op1, const std::string& op2);
		static inline var ssb(const std::string& op1, const Bytes& op2);


	//乘法系列

		//已知op1类型，op2未知
		static inline var i8mv(const char op1, const var& op2);
		static inline var i16mv(const short op1, const var& op2);
		static inline var i32mv(const int op1, const var& op2);
		static inline var i64mv(const long long op1, const var& op2);
		static inline var fmv(const float op1, const var& op2);
		static inline var dmv(const double op1, const var& op2);
		static inline var smv(const std::string& op1, const var& op2);
		static inline var bmv(const Bytes& op1, const var& op2);
		//由string/bytes转换得的整型数与数值型变量相乘
		// op1的取值也会影响相乘后的数值的类型
		// 如果op2不是数值型而直接传入，会抛var()错误并返回none
		static inline var imn(const long long op1, const var& op2);
		//同类型
		static inline var i8m(const char op1, const char op2);
		static inline var i16m(const short op1, const short op2);
		static inline var i32m(const int op1, const int op2);
		static inline var i64m(const long long op1, const long long op2);
		static inline var fm(const float op1, const float op2);
		static inline var dm(const double op1, const double op2);
		static inline var bm(const Bytes& op1, const Bytes& op2);
		static inline var sm(const std::string& op1, const std::string& op2);
		//int8与array
		static inline var i8ms(const char op1, const std::string& op2);
		static inline var i8mb(const char op1, const Bytes& op2);
		//int16与array
		static inline var i16ms(const short op1, const std::string& op2);
		static inline var i16mb(const short op1, const Bytes& op2);
		//int32与array
		static inline var i32ms(const int op1, const std::string& op2);
		static inline var i32mb(const int op1, const Bytes& op2);
		//int64与array
		static inline var i64ms(const long long op1, const std::string& op2);
		static inline var i64mb(const long long op1, const Bytes& op2);
		//double、float与array
		static inline var dms(const double op1, const std::string& op2, const char size = 1);
		static inline var dmb(const double op1, const Bytes& op2, const char size = 1);
		//string、bytes与ints
		static inline var smi(const std::string& op1, const long long& op2);
		static inline var bmi(const Bytes& op1, const long long& op2);
		//string、bytes浮点，不可操作
		static inline var smd();
		static inline var bmd();
		
		static inline var bms(const Bytes& op1, const std::string& op2);
		static inline var smb(const std::string& op1, const Bytes& op2);


	//除法系列

		//已知op1类型，op2未知
		static inline var i8dv(const char op1, const var& op2);
		static inline var i16dv(const short op1, const var& op2);
		static inline var i32dv(const int op1, const var& op2);
		static inline var i64dv(const long long op1, const var& op2);
		static inline var fdv(const float op1, const var& op2);
		static inline var ddv(const double op1, const var& op2);
		static inline var sdv(const std::string& op1, const var& op2);
		static inline var bdv(const Bytes& op1, const var& op2);
		//由string/bytes转换得的整型数与数值型变量相减
		// op1的取值也会影响相减后的数值的类型
		// 如果op2不是数值型而直接传入，会抛var()错误并返回none
		static inline var idn(const long long op1, const var& op2);
		//同类型相减
		static inline var i8d(const char op1, const char op2);
		static inline var i16d(const short op1, const short op2);
		static inline var i32d(const int op1, const int op2);
		static inline var i64d(const long long op1, const long long op2);
		static inline var fd(const float op1, const float op2);
		static inline var dd(const double op1, const double op2);
		static inline var bd(const Bytes& op1, const Bytes& op2);
		static inline var sd(const std::string& op1, const std::string& op2);
		//int8与array
		static inline var i8ds(const char op1, const std::string& op2);
		static inline var i8db(const char op1, const Bytes& op2);
		//int16与array
		static inline var i16ds(const short op1, const std::string& op2);
		static inline var i16db(const short op1, const Bytes& op2);
		//int32与array
		static inline var i32ds(const int op1, const std::string& op2);
		static inline var i32db(const int op1, const Bytes& op2);
		//int64与array
		static inline var i64ds(const long long op1, const std::string& op2);
		static inline var i64db(const long long op1, const Bytes& op2);
		//double、float与array
		static inline var dds(const double op1, const std::string& op2, const char size = 1);
		static inline var ddb(const double op1, const Bytes& op2, const char size = 1);
		//string、bytes与ints
		//第三个参数取值取决于第二个参数的实际长度
		// char short int <long long> 依次为0-3
		static inline var sdi(const std::string& op1, const long long& op2, const char len = 2);
		static inline var bdi(const Bytes& op1, const long long& op2, const char len = 2);
		//string、bytes与浮点
		//第三个参数取值取决于第二个参数的实际长度
		static inline var sdd(const std::string& op1, const double& op2, const char len = 1);
		static inline var bdd(const Bytes& op1, const double& op2, const char len = 1);
		static inline var bds(const Bytes& op1, const std::string& op2);
		static inline var sdb(const std::string& op1, const Bytes& op2);

	//取余系列

		//已知op1类型，op2未知
		static inline var i8modv(const char op1, const var& op2);
		static inline var i16modv(const short op1, const var& op2);
		static inline var i32modv(const int op1, const var& op2);
		static inline var i64modv(const long long op1, const var& op2);
		static inline var fmodv();
		static inline var dmodv();
		static inline var smodv(const std::string& op1, const var& op2);
		static inline var bmodv(const Bytes& op1, const var& op2);
		//由string/bytes转换得的整型数与数值型变量相乘
		// op1的取值也会影响相乘后的数值的类型
		// 如果op2不是数值型而直接传入，会抛var()错误并返回none
		static inline var imodn(const long long op1, const var& op2);
		//同类型
		static inline var i8mod(const char op1, const char op2);
		static inline var i16mod(const short op1, const short op2);
		static inline var i32mod(const int op1, const int op2);
		static inline var i64mod(const long long op1, const long long op2);
		static inline var fmod();
		static inline var dmod();
		static inline var bmod(const Bytes& op1, const Bytes& op2);
		static inline var smod(const std::string& op1, const std::string& op2);
		//int8与array
		static inline var i8mods(const char op1, const std::string& op2);
		static inline var i8modb(const char op1, const Bytes& op2);
		//int16与array
		static inline var i16mods(const short op1, const std::string& op2);
		static inline var i16modb(const short op1, const Bytes& op2);
		//int32与array
		static inline var i32mods(const int op1, const std::string& op2);
		static inline var i32modb(const int op1, const Bytes& op2);
		//int64与array
		static inline var i64mods(const long long op1, const std::string& op2);
		static inline var i64modb(const long long op1, const Bytes& op2);
		//double、float与array
		static inline var dmods();
		static inline var dmodb();
		//string、bytes与ints
		static inline var smodi(const std::string& op1, const long long& op2, const char len = 1);
		static inline var bmodi(const Bytes& op1, const long long& op2, const char len = 1);
		//string、bytes浮点，不可操作
		static inline var smodd();
		static inline var bmodd();

		static inline var bmods(const Bytes& op1, const std::string& op2);
		static inline var smodb(const std::string& op1, const Bytes& op2);
};

#endif