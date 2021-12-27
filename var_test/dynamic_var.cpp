#include "dynamic_var.h"

char test_to_num(const std::string& str, long long& int_result)
{
	// | double?
	// | long long?
	// | | int?
	// | | | short?
	// | | | | char?
	int_result = 0;
	double tempd = 0;
	long long templl = 0;
	//尝试转为double
	try
	{
		tempd = std::stod(str);
	}
	//不行
	catch (...)
	{
		return 0;
	}
	//可以，看看是不是浮点数（包括范围过大的整数）
	//尝试转为long long
	try
	{
		templl = std::stoll(str);
	}
	//不行，只能用浮点表示
	catch (...)
	{
		return 1;
	}
	//可以转long long，判定其值范围
	//  范围过大丢失精度
	if (((long long)tempd) != templl)
	{
		return 3;
	}
	//存在小数
	else if (tempd != ((double)templl))
	{
		return 1;
	}

	//先写回结果，进一步判别类型
	int_result = templl;
	if (((char)templl) == templl)
		return 6;
	else if (((short)templl) == templl)
		return 5;
	else if (((int)templl) == templl)
		return 4;
	else
		return 3;
}

char test_all_num(std::string str)
{
	std::string blanks("\f\v\r\t\n ");
	str.erase(0, str.find_first_not_of(blanks));
	str.erase(str.find_last_not_of(blanks) + 1);
	//特殊情况
	if (str == ".")return 0;
	if (str == "-.")return 0;
	if (str == "+.")return 0;
	if (str.empty())return 0;
	//正常情况
	char point_founded = 0;
	int len = str.length();
	int i = 0;
	if ((str[0] == '-')|| (str[0] == '+'))
		++i;
	//只有一个负号
	if (i == len)
		return 0;
	//检查整数部分
	for (; i < len; ++i)
	{
		if ((str[i] >= '0') && (str[i] <= '9'))
			continue;
		if (str[i] == '.')
		{
			point_founded = 1;
			++i;
			break;
		}
		//不是 0-9 也不是小数点，报错
		return 0;
	}
	//检查是否已到末尾
	if (i >= len)
		return 1;
	//继续
	for (; i < len; ++i)
	{
		if ((str[i] >= '0') && (str[i] <= '9'))
			continue;
		//不是 0-9 ，报错
		return 0;
	}
	return 2;
}

var::var()
{
	this->_type = Type_t::none;
	this->_data.double_d = 0;
}
var::var(const var& val)
{
	this->_type = val._type;
	this->_data = val._data;
	if (this->_type == Type_t::string_kind)
	{
		this->_data.string_d = new std::string(*val._data.string_d);
	}
	else if (this->_type == Type_t::bytes)
	{
		this->_data.bytes_d = new Bytes(*val._data.bytes_d);
	}
}
var::var(const var::Type_t typ)
{
	this->_type = typ;
	switch (typ)
	{
	case var::Type_t::none:
		break;
	case var::Type_t::bool_kind:
		this->_data.bool_d = false;
		break;
	case var::Type_t::int8_kind:
		this->_data.int8_d = 0;
		break;
	case var::Type_t::int16_kind:
		this->_data.int16_d = 0;
		break;
	case var::Type_t::int32_kind:
		this->_data.int32_d = 0;
		break;
	case var::Type_t::int64_kind:
		this->_data.int64_d = 0;
		break;
	case var::Type_t::double_kind:
		this->_data.double_d = 0.0;
		break;
	case var::Type_t::float_kind:
		this->_data.float_d = 0.0f;
		break;
	case var::Type_t::daytime_kind:
		this->_data.daytimd_d.hour = 0;
		this->_data.daytimd_d.minute = 0;
		this->_data.daytimd_d.second = 0;
		break;
	case var::Type_t::string_kind:
		this->_data.string_d = new std::string;
		break;
	case var::Type_t::bytes:
		this->_data.bytes_d = new Bytes;
		break;
	default:
		throw var();
		break;
	}
}
// 带初始值的构造函数
var::var(bool data)
{
	this->_type = Type_t::bool_kind;
	this->_data.int8_d = data;
}
var::var(char data)
{
	this->_type = Type_t::int8_kind;
	this->_data.int8_d = data;
}
var::var(short data)
{
	this->_type = Type_t::int16_kind;
	this->_data.int16_d = data;
}
var::var(int data)
{
	this->_type = Type_t::int32_kind;
	this->_data.int32_d = data;
}
var::var(long long data)
{
	this->_type = Type_t::int64_kind;
	this->_data.int64_d = data;
}
var::var(double data)
{
	this->_type = Type_t::double_kind;
	this->_data.double_d = data;
}
var::var(float data)
{
	this->_type = Type_t::float_kind;
	this->_data.float_d = data;
}
var::var(const var::daytime_t& data)
{
	char hour, min, sec;
	this->_type = Type_t::daytime_kind;
	if (data.hour < 0 || data.hour >23)hour = 0;
	else hour = data.hour;
	if (data.minute < 0 || data.minute >59)min = 0;
	else min = data.minute;
	if (data.second < 0 || data.second >59)sec = 0;
	else sec = data.second;
	this->_data.daytimd_d.hour = hour;
	this->_data.daytimd_d.minute = min;
	this->_data.daytimd_d.second = sec;
}
var::var(char hour, char min, char sec)
{
	this->_type = Type_t::daytime_kind;
	if (hour < 0 || hour >23)hour = 0;
	if (min < 0 || min >59)min = 0;
	if (sec < 0 || sec >59)sec = 0;
	this->_data.daytimd_d.hour = hour;
	this->_data.daytimd_d.minute = min;
	this->_data.daytimd_d.second = sec;
}
var::var(const std::string& data)
{
	this->_type = Type_t::string_kind;
	this->_data.string_d = new std::string(data);
}
var::var(const char* data)
{
	if (!data)
	{
		this->_type = Type_t::none;
#ifndef DYNAMIC_VAR_MUTE_INVALID_ARG
		throw std::invalid_argument("nullptr receieved");
#endif // !DYNAMIC_VAR_MUTE_INVALID_ARG
	}
	this->_type = Type_t::string_kind;
	this->_data.string_d = new std::string(data);
}
var::var(const Bytes& data)
{
	this->_type = Type_t::bytes;
	this->_data.bytes_d = new Bytes(data);
}

// 析构函数
var::~var()
{
	this->clear();
}

// 读取当前类型
var::Type_t var::type()const
{
	return this->_type;
}
std::string var::type_name()const
{
	switch (this->_type)
	{
	case var::Type_t::none:
		return "none";
		break;
	case var::Type_t::bool_kind:
		return "bool";
		break;
	case var::Type_t::int8_kind:
		return "int8(char)";
		break;
	case var::Type_t::int16_kind:
		return "int16(short)";
		break;
	case var::Type_t::int32_kind:
		return "int32(int)";
		break;
	case var::Type_t::int64_kind:
		return "int64(long long)";
		break;
	case var::Type_t::double_kind:
		return "double";
		break;
	case var::Type_t::float_kind:
		return "float";
		break;
	case var::Type_t::string_kind:
		return "std::string";
		break;
	case var::Type_t::bytes:
		return "bytes(std::vector<unsigned char>)";
		break;
	default:
		throw var();
		return "unknown!";
		break;
	}
}
std::string var::type_name(const var& obj)
{
	return obj.type_name();
}
// 判断是否为none
bool var::is_none()const
{
	return (this->_type == Type_t::none);
}

var::operator std::string(void)
{
	return this->to_string();
}
var::operator double(void)
{
	return this->to_double();
}
var::operator bool(void)
{
	return this->to_bool();
}
// to_bool规则
inline bool is_bool_i8(const char& val){ return val != 0; }
inline bool is_bool_i16(const short& val){ return val != 0; }
inline bool is_bool_i32(const int& val){ return val != 0; }
inline bool is_bool_i64(const long long& val){ return val != 0; }
inline bool is_bool_f(const float& val){ return val; }
inline bool is_bool_d(const double& val){ return val; }
inline bool is_bool_s(const std::string& val) { return val.length() != 0; }
inline bool is_bool_b(const Bytes& val){ return val.size() != 0; }
bool var::to_bool()const
{
	switch (this->_type)
	{
	case var::Type_t::none:
		return false;
		break;
	case var::Type_t::bool_kind:
		return this->_data.bool_d;
		break;
	case var::Type_t::int8_kind:
		return is_bool_i8(this->_data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return is_bool_i16(this->_data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return is_bool_i32(this->_data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return is_bool_i64(this->_data.int64_d);
		break;
	case var::Type_t::double_kind:
		return is_bool_d(this->_data.double_d);
		break;
	case var::Type_t::float_kind:
		return is_bool_f(this->_data.float_d);
		break;
	case var::Type_t::string_kind:
		return is_bool_s(*this->_data.string_d);
		break;
	case var::Type_t::bytes:
		return is_bool_b(*this->_data.bytes_d);
		break;
	default:
		throw var();
		return false;
		break;
	}
}

char var::to_char()const
{
	if (this->_type == Type_t::int8_kind)
		return this->_data.int8_d;
	return (char)this->to_int64();
}
short var::to_short()const
{
	if (this->_type == Type_t::int16_kind)
		return this->_data.int16_d;
	return (short)this->to_int64();
}
int var::to_int()const
{
	if (this->_type == Type_t::int32_kind)
		return this->_data.int32_d;
	return (int)this->to_int64();
}
// 如果沿用double方法会造成精度损失，因此重写
long long var::to_int64()const
{
	switch (this->_type)
	{
	case var::Type_t::none:
		break;
	case var::Type_t::bool_kind:
		return (long long)this->_data.bool_d;
		break;
	case var::Type_t::int8_kind:
		return (long long)(this->_data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return (long long)(this->_data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return (long long)(this->_data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return (this->_data.int64_d);
		break;
	case var::Type_t::double_kind:
		return (long long)(this->_data.double_d);
		break;
	case var::Type_t::float_kind:
		return (long long)(this->_data.int8_d);
		break;
	case var::Type_t::string_kind:
	{
		if (test_all_num(*(this->_data.string_d)))
			return atoll((*(this->_data.string_d)).c_str());
		else
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return 0;
		}
		break;
	}
	case var::Type_t::bytes:
	{
		std::string temp = this->to_string();
		if (test_all_num(temp))
			return atoll(temp.c_str());
		else
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return 0;
		}
		break;
	}
	default:
		throw var();
		break;
	}
	return 0;
}
double var::to_double()const
{
	switch (this->_type)
	{
	case var::Type_t::none:
		break;
	case var::Type_t::bool_kind:
		return (double)this->_data.bool_d;
		break;
	case var::Type_t::int8_kind:
		return (double)(this->_data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return (double)(this->_data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return (double)(this->_data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return (double)(this->_data.int64_d);
		break;
	case var::Type_t::double_kind:
		return (this->_data.double_d);
		break;
	case var::Type_t::float_kind:
		return (double)(this->_data.float_d);
		break;
	case var::Type_t::string_kind:
	{
		if (test_all_num(*(this->_data.string_d)))
			return atof((*(this->_data.string_d)).c_str());
		else
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return 0;
		}
		break;
	}
	case var::Type_t::bytes:
	{
		std::string temp = this->to_string();
		if (test_all_num(temp))
			return atof(temp.c_str());
		else
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return 0;
		}
		break;
	}
	default:
		throw var();
		break;
	}
	return 0;
}
float var::to_float()const
{
	return (float)this->to_double();
}
std::string var::to_string()const
{
	switch (this->_type)
	{
	case var::Type_t::none:
		return "";
		break;
	case var::Type_t::bool_kind:
		if (this->_data.bool_d)
			return "True";
		else
			return "False";
		break;
	case var::Type_t::int8_kind:
		return std::to_string((int)(this->_data.int8_d));
		break;
	case var::Type_t::int16_kind:
		return std::to_string((int)(this->_data.int16_d));
		break;
	case var::Type_t::int32_kind:
		return std::to_string(this->_data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return std::to_string(this->_data.int64_d);
		break;
	case var::Type_t::double_kind:
		return std::to_string(this->_data.double_d);
		break;
	case var::Type_t::float_kind:
		return std::to_string(this->_data.float_d);
		break;
	case var::Type_t::string_kind:
		return std::string(*(this->_data.string_d));
		break;
	case var::Type_t::bytes:
	{
		std::string rt;
		/*if (this->_data.bytes_d->back())
			rt.resize(this->_data.bytes_d->size() + 1);
		else*/
			rt.resize(this->_data.bytes_d->size());
		int times = this->_data.bytes_d->size();
		for (int i = 0; i < times; ++i)
			rt[i] = (*(this->_data.bytes_d))[i];
		return rt;
		break;
	}
	default:
		throw var();
		return "no to_string() founded";
		break;
	}
}
Bytes var::to_bytes()const
{
	switch (this->_type)
	{
	case var::Type_t::none:
		return Bytes();
		break;
	case var::Type_t::bytes:
		return Bytes(*(this->_data.bytes_d));
		break;
	case var::Type_t::bool_kind:
	case var::Type_t::int8_kind:
	case var::Type_t::int16_kind:
	case var::Type_t::int32_kind:
	case var::Type_t::int64_kind:
	case var::Type_t::double_kind:
	case var::Type_t::float_kind:
	case var::Type_t::string_kind:
	{
		std::string temp = this->to_string();
		int len = temp.length();
		Bytes rt(len);
		for (int i = 0; i < len; ++i)
			rt[i] = (unsigned char)temp[i];
		return rt;
		break; 
	}
	default:
		throw var();
		return Bytes();
		break;
	}
}
var var::operator=(const var& val)
{
	this->clear();
	this->_type = val._type;
	this->_data = val._data;
	if (this->_type == Type_t::string_kind)
	{
		this->_data.string_d = new std::string(*val._data.string_d);
	}
	else if (this->_type == Type_t::bytes)
	{
		this->_data.bytes_d = new Bytes(*val._data.bytes_d);
	}
	return *this;
}

var var::operator=(bool val)
{
	this->clear();
	this->_type = Type_t::bool_kind;
	this->_data.int8_d = val;
	return *this;
}
var var::operator=(char val)
{
	this->clear();
	this->_type = Type_t::int8_kind;
	this->_data.int8_d = val;
	return *this;
}
var var::operator=(short val)
{
	this->clear();
	this->_type = Type_t::int16_kind;
	this->_data.int16_d = val;
	return *this;
}
var var::operator=(int val)
{
	this->clear();
	this->_type = Type_t::int32_kind;
	this->_data.int32_d = val;
	return *this;
}
var var::operator=(long long val)
{
	this->clear();
	this->_type = Type_t::int64_kind;
	this->_data.int64_d = val;
	return *this;
}
var var::operator=(double val)
{
	this->clear();
	this->_type = Type_t::double_kind;
	this->_data.double_d = val;
	return *this;
}
var var::operator=(float val)
{
	this->clear();
	this->_type = Type_t::float_kind;
	this->_data.float_d = val;
	return *this;
}
// 创建为 string 型
var var::operator=(const std::string& val)
{
	this->clear();
	this->_type = Type_t::string_kind;
	this->_data.string_d = new std::string(val);
	return *this;
}
// 创建为 string 型
var var::operator=(const char* val)
{
	this->clear();
	this->_type = Type_t::string_kind;
	this->_data.string_d = new std::string(val);
	return *this;
}
// 创建为 byte_stream 型
var var::operator=(const Bytes& val)
{
	this->clear();
	this->_type = Type_t::bytes;
	this->_data.bytes_d = new Bytes(val);
	return *this;
}

#ifdef DYNAMIC_VAR_ENABLE_ARITH_OP

/* 内建处理函数开始 */

//加法系列
var var::i8av(const char op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
		return var::i8a(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i8a(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16a(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32a(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::da(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fa(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i8as(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i8ab(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i16av(const short op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
		return var::i16a(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i16a(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16a(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32a(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::da(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fa(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i16as(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i16ab(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i32av(const int op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
		return var::i32a(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i32a(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i32a(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32a(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::da(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::da(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i32as(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i32ab(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i64av(const long long op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
		return var::i64a(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i64a(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i64a(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i64a(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::da((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::da((double)op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i64as(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i64ab(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::fav(const float op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
		return var::fa(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::fa(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::fa(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::da(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::da(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::da(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fa(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::das(op1, *op2._data.string_d, 0);
		break;
	case var::Type_t::bytes:
		return var::dab(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::dav(const double op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
		return var::da(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::da(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::da(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::da(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::da(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::da(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::da(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::das(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::dab(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::sav(const std::string& op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
	case var::Type_t::int8_kind:
	case var::Type_t::int16_kind:
	case var::Type_t::int32_kind:
	case var::Type_t::int64_kind:
	case var::Type_t::double_kind:
	case var::Type_t::float_kind:
	case var::Type_t::bytes:
		return var(op1 + (op2.to_string()));
		break;
	case var::Type_t::string_kind:
		return var(op1 + (*op2._data.string_d));
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::bav(const Bytes& op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
		return var(op1);
		break;
	case var::Type_t::bool_kind:
	case var::Type_t::int8_kind:
	case var::Type_t::int16_kind:
	case var::Type_t::int32_kind:
	case var::Type_t::int64_kind:
	case var::Type_t::double_kind:
	case var::Type_t::float_kind:
	{
		std::string temps = var(op1).to_string();
		// 是数值
		if (test_all_num(temps))
		{
			long long templl = 0;
			char type = test_to_num(temps, templl);
			if (type == 1)
			{
				return var::da(atof(temps.c_str()), op2.to_double());
			}
			else if ((type >= 3) && (type <= 6))
			{
				return var::ian(templl, op2);
			}
		}
		// 不是数值
		return var::ba(op1, op2.to_bytes());
		break;
	}
	case var::Type_t::bytes:
	{
		return var::ba(op1, *op2._data.bytes_d);
		break;
	}
	case var::Type_t::string_kind:
	{
		return var::ba(op1, op2.to_bytes());
		break;
	}
	default:
		throw var();
		return var();
		break;
	}
}
var var::ian(const long long op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::bool_kind:
	{
		if (((char)op1) == op1)
			return i8a((char)op1, op2._data.bool_d);
		else if (((short)op1) == op1)
			return i16a((short)op1, op2._data.bool_d);
		else if (((int)op1) == op1)
			return i32a((int)op1, op2._data.bool_d);
		else
			return i64a(op1, op2._data.bool_d);
		break;
	}
	case var::Type_t::int8_kind:
	{
		if (((char)op1) == op1)
			return i8a((char)op1, op2._data.int8_d);
		else if (((short)op1) == op1)
			return i16a((short)op1, op2._data.int8_d);
		else if (((int)op1) == op1)
			return i32a((int)op1, op2._data.int8_d);
		else
			return i64a(op1, op2._data.int8_d);
		break;
	}
	case var::Type_t::int16_kind:
	{
		if (((short)op1) == op1)
			return i16a((short)op1, op2._data.int16_d);
		else if (((int)op1) == op1)
			return i32a((int)op1, op2._data.int16_d);
		else
			return i64a(op1, op2._data.int16_d);
		break;
	}
	case var::Type_t::int32_kind:
	{
		if (((int)op1) == op1)
			return i32a((int)op1, op2._data.int32_d);
		else
			return i64a(op1, op2._data.int32_d);
		break;
	}
	case var::Type_t::int64_kind:
		return i64a(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return da((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
	{
		if (((short)op1) == op1)
			return fa((short)op1, op2._data.float_d);
		else
			return da((double)op1, op2._data.float_d);
		break;
	}
	default:
		throw var();
		return var();
		break;
	}
}
var var::i8a(const char op1, const char op2)
{
	return var((char)(op1 + op2));
}
var var::i16a(const short op1, const short op2)
{
	return var((short)(op1 + op2));
}
var var::i32a(const int op1, const int op2)
{
	return var((int)(op1 + op2));
}
var var::i64a(const long long op1, const long long op2)
{
	return var((long long)(op1 + op2));
}
var var::fa(const float op1, const float op2)
{
	return var((float)(op1 + op2));
}
var var::da(const double op1, const double op2)
{
	return var((double)(op1 + op2));
}
var var::ba(const Bytes& op1, const Bytes& op2)
{
	Bytes tempb;
	tempb.insert(tempb.begin(), op1.begin(), op1.end());
	tempb.insert(tempb.end(), op2.begin(), op2.end());
	return var(tempb);
}

var var::i8as(const char op1, const std::string& op2)
{
	//检查是否纯数字，不是则为合并字符串
	if (test_all_num(op2))
	{
		long long templl = 0;
		switch (test_to_num(op2,templl))
		{
		case 1: return var::da(op1, atof(op2.c_str())); break;
		case 3: return var::i64a(op1, templl); break;
		case 4: return var::i32a(op1, (int)templl); break;
		case 5: return var::i16a(op1, (short)templl); break;
		case 6: return var::i8a(op1, (char)templl); break;
		default:
			return var(var(op1).to_string() + op2);
			break;
		}
	}
	return var(var(op1).to_string() + op2);
}
var var::i8ab(const char op1, const Bytes& op2)
{
	return i8as(op1, var(op2).to_string());
}

var var::i16as(const short op1, const std::string& op2)
{
	//检查是否纯数字，不是则为合并字符串
	if (test_all_num(op2))
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::da(op1, atof(op2.c_str())); break;
		case 3: return var::i64a(op1, templl); break;
		case 4: return var::i32a(op1, (int)templl); break;
		case 5: case 6: return var::i16a(op1, (short)templl); break;
		default:
			return var(var(op1).to_string() + op2);
			break;
		}
	}
	return var(var(op1).to_string() + op2);
}
var var::i16ab(const short op1, const Bytes& op2)
{
	return i16as(op1, var(op2).to_string());
}

var var::i32as(const int op1, const std::string& op2)
{
	//检查是否纯数字，不是则为合并字符串
	if (test_all_num(op2))
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::da(op1, atof(op2.c_str())); break;
		case 3: return var::i64a(op1, templl); break;
		case 4: case 5: case 6: return var::i32a(op1, (int)templl); break;
		default:
			return var(var(op1).to_string() + op2);
			break;
		}
	}
	return var(var(op1).to_string() + op2);
}
var var::i32ab(const int op1, const Bytes& op2)
{
	return i32as(op1, var(op2).to_string());
}

var var::i64as(const long long op1, const std::string& op2)
{
	//检查是否纯数字，不是则为合并字符串
	if (test_all_num(op2))
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::da((double)op1, atof(op2.c_str())); break;
		case 3: case 4: case 5: case 6: return var::i64a(op1, templl); break;
		default:
			return var(var(op1).to_string() + op2);
			break;
		}
	}
	return var(var(op1).to_string() + op2);
}
var var::i64ab(const long long op1, const Bytes& op2)
{
	return i64as(op1, var(op2).to_string());
}

var var::das(const double op1, const std::string& op2, const char size)
{
	//检查是否纯数字，不是则为合并字符串
	if (test_all_num(op2))
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::da(op1, atof(op2.c_str())); break;
		case 3: case 4: return var::da(op1, (double)templl); break;
		case 5: case 6:
		{
			if (size)
				return var::da(op1, (double)templl);
			else
				return var::fa((float)op1, (float)templl);
			break;
		}
		default:
			break;
		}
	}
	return var(var(op1).to_string() + op2);
}
var var::dab(const double op1, const Bytes& op2, const char size)
{
	return das(op1, var(op2).to_string());
}

var var::bai(const Bytes& op1, const long long& op2, const char len)
{
	std::string temps = var(op1).to_string();
	if (test_all_num(temps))
	{
		long long templl = 0;
		switch (test_to_num(temps, templl))
		{
		case 1: return var::da(atof(temps.c_str()), (double)op2); break;
		case 3: return var::i64a(templl, op2); break;
		case 4:
		{
			if (len <= 2)return var::i32a((int)templl, (int)op2);
			else return var::i64a(templl, op2);
			break;
		}
		case 5:
		{
			if (len <= 1)return var::i16a((short)templl, (short)op2);
			else if (len <= 2)return var::i32a((int)templl, (int)op2);
			else return var::i64a(templl, op2);
			break;
		}
		case 6:
		{
			if (len == 0)return var::i8a((char)templl, (char)op2);
			else if (len == 1)return var::i16a((short)templl, (short)op2);
			else if (len <= 2)return var::i32a((int)templl, (int)op2);
			else return var::i64a(templl, op2);
			break;
		}
		default:
			break;
		}
	}
	return temps + var(op2).to_string();
}

var var::bad(const Bytes& op1, const double& op2, const char len)
{
	std::string temps = var(op1).to_string();
	if (test_all_num(temps))
	{
		long long templl = 0;
		switch (test_to_num(temps, templl))
		{
		case 1: return var::da(atof(temps.c_str()), op2); break;
		case 3: return var::da((double)templl, op2); break;
		case 4:	case 5:	case 6:
		{
			if (len)return var::da((double)templl, op2);
			else return var::fa((float)templl, (float)op2);
			break;
		}
		default:
			break;
		}
	}
	if (len) return temps + var(op2).to_string();
	else return temps + var((float)op2).to_string();
}


/* 内建处理函数结束 */

// 加法系列
var operator+(const var& op1, const var& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return var(op2);
		break;
	case var::Type_t::bool_kind:
		return var::i8av(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8av(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16av(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32av(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64av(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dav(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fav(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::sav(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bav(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const var& op1, const bool op2)
{
	return op1 + ((char)op2);
}
var operator+(const bool op1, const var& op2)
{
	return ((char)op1) + op2;
}
var operator+(const var& op1, const char op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return var(op2);
		break;
	case var::Type_t::bool_kind:
		return var::i8a(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8a(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16a(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32a(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::da(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fa(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return *op1._data.string_d + var(op2).to_string();
		break;
	case var::Type_t::bytes:
		return var::bai(*op1._data.bytes_d, op2, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const char op1, const var& op2)
{
	return var::i8av(op1, op2);
}
var operator+(const var& op1, const short op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return op1;
		break;
	case var::Type_t::bool_kind:
		return var::i16a(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i16a(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16a(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32a(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::da(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fa(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return *op1._data.string_d + var(op2).to_string();
		break;
	case var::Type_t::bytes:
		return var::bai(*op1._data.bytes_d, op2, 1);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const short op1, const var& op2)
{
	return var::i16av(op1, op2);
}
var operator+(const var& op1, const int op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return op1;
		break;
	case var::Type_t::bool_kind:
		return var::i32a(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i32a(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i32a(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32a(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::da(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::da(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return *op1._data.string_d + var(op2).to_string();
		break;
	case var::Type_t::bytes:
		return var::bai(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const int op1, const var& op2)
{
	return var::i32av(op1, op2);
}
var operator+(const var& op1, const long long op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return op1;
		break;
	case var::Type_t::bool_kind:
		return var::i64a(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i64a(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i64a(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i64a(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64a(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::da(op1._data.double_d, (double)op2);
		break;
	case var::Type_t::float_kind:
		return var::da(op1._data.float_d, (double)op2);
		break;
	case var::Type_t::string_kind:
		return *op1._data.string_d + var(op2).to_string();
		break;
	case var::Type_t::bytes:
		return var::bai(*op1._data.bytes_d, op2, 3);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const long long op1, const var& op2)
{
	return var::i64av(op1, op2);
}
var operator+(const var& op1, const float op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return op1;
		break;
	case var::Type_t::bool_kind:
		return var::fa(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::fa(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::fa(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::da(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::da((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::da(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fa(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return *op1._data.string_d + var(op2).to_string();
		break;
	case var::Type_t::bytes:
		return var::bad(*op1._data.bytes_d, op2, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const float op1, const var& op2)
{
	return var::fav(op1, op2);
}
var operator+(const var& op1, const double op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return op1;
		break;
	case var::Type_t::bool_kind:
		return var::da(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::da(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::da(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::da(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::da((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::da(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::da(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return *op1._data.string_d + var(op2).to_string();
		break;
	case var::Type_t::bytes:
		return var::bad(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const double op1, const var& op2)
{
	return var::dav(op1, op2);
}
var operator+(const var& op1, const std::string& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return op2;
		break;
	case var::Type_t::bool_kind:
		return var::i8as(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8as(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16as(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32as(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64as(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::das(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::das(op1._data.float_d, op2, 0);
		break;
	case var::Type_t::string_kind:
		return *op1._data.string_d + op2;
		break;
	case var::Type_t::bytes:
		return var::ba(*op1._data.bytes_d, var(op2).to_bytes());
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const std::string& op1, const var& op2)
{
	if (op2._type == var::Type_t::none)
		return op1;
	return op1 + op2.to_string();
}
var operator+(const var& op1, const char* const op2)
{
	return var(op1 + std::string(op2));
}
var operator+(const char* const op1, const var& op2)
{
	if (op2._type == var::Type_t::none)
		return std::string(op1);
	else return std::string(op1) + op2.to_string();
}
var operator+(const var& op1, const Bytes& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
		return var(op2);
		break;
	case var::Type_t::bool_kind:
		return var::i8ab(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8ab(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16ab(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32ab(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64ab(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dab(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dab(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return (*op1._data.string_d) + var(op2).to_string();
		break;
	case var::Type_t::bytes:
		return var::ba(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator+(const Bytes& op1, const var& op2)
{
	return var::bav(op1, op2);
}


// 减法组开始

var var::i8sv(const char op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8s(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i8s(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16s(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32s(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fs(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i8ss(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i8sb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i16sv(const short op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16s(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i16s(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16s(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32s(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fs(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i16ss(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i16sb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i32sv(const int op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32s(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i32s(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i32s(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32s(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::ds(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i32ss(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i32sb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i64sv(const long long op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64s(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i64s(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i64s(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i64s(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::ds((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::ds((double)op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i64ss(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i64sb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::fsv(const float op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::fs(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::fs(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::fs(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::ds(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::ds(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fs(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::dss(op1, *op2._data.string_d, 0);
		break;
	case var::Type_t::bytes:
		return var::dsb(op1, *op2._data.bytes_d, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::dsv(const double op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::ds(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::ds(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::ds(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::ds(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::ds(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::ds(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::dss(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::dsb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::ssv(const std::string& op1, const var& op2)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1:	return var::dsv(atof(op1.c_str()), op2); break;
		case 3: return var::i64sv(templl, op2); break;
		case 4: return var::i32sv((int)templl, op2); break;
		case 5: return var::i16sv((short)templl, op2); break;
		case 6: return var::i8sv((char)templl, op2); break;
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bsv(const Bytes& op1, const var& op2)
{
	return var::ssv(var(op1).to_string(), op2);
}
var var::isn(const long long op1, const var& op2)
{
	char size = 3;
	if (((short)op1) == op1)
	{
		size = 1;
		if (((short)op1) == op1)
		{
			size = 0;
		}
	}
	else if (((int)op1) == op1)
		size = 2;
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
	{
		switch (size)
		{
		case 0: return var::i8s((char)op1, op2._data.bool_d); break;
		case 1: return var::i16s((short)op1, op2._data.bool_d); break;
		case 2: return var::i32s((int)op1, op2._data.bool_d); break;
		case 3: return var::i64s(op1, op2._data.bool_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int8_kind:
	{
		switch (size)
		{
		case 0: return var::i8s((char)op1, op2._data.int8_d); break;
		case 1: return var::i16s((short)op1, op2._data.int8_d); break;
		case 2: return var::i32s((int)op1, op2._data.int8_d); break;
		case 3: return var::i64s(op1, op2._data.int8_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int16_kind:
	{
		switch (size)
		{
		case 0:
		case 1: return var::i16s((short)op1, op2._data.int16_d); break;
		case 2: return var::i32s((int)op1, op2._data.int16_d); break;
		case 3: return var::i64s(op1, op2._data.int16_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int32_kind:
	{
		switch (size)
		{
		case 0:
		case 1:
		case 2: return var::i32s((int)op1, op2._data.int32_d); break;
		case 3: return var::i64s(op1, op2._data.int32_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int64_kind:
		return var::i64s(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::ds((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		if(size>=2)
			return var::ds((double)op1, op2._data.float_d);
		return var::fs((float)op1, op2._data.float_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
//同类型相减
var var::i8s(const char op1, const char op2)
{
	return var((char)(op1 - op2));
}
var var::i16s(const short op1, const short op2)
{
	return var((short)(op1 - op2));
}
var var::i32s(const int op1, const int op2)
{
	return var((int)(op1 - op2));
}
var var::i64s(const long long op1, const long long op2)
{
	return var((long long)(op1 - op2));
}
var var::fs(const float op1, const float op2)
{
	return var((float)(op1 - op2));
}
var var::ds(const double op1, const double op2)
{
	return var((double)(op1 - op2));
}
var var::bs(const Bytes& op1, const Bytes& op2)
{
	return var::ss(var(op1).to_string(), var(op2).to_string());
}
var var::ss(const std::string& op1, const std::string& op2)
{
	//复用ssi与ssd函数，因此只判定op2
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		char size = 0;
		switch (test_to_num(op2,templl))
		{
		case 1:
			return var::ssd(op1, atof(op2.c_str()));
			break;
		case 3: size = 4; break;
		case 4: size = 3; break;
		case 5: size = 2; break;
		case 6: size = 1; break;
		default:
			break;
		}
		if(size)
			return var::ssi(op1, templl, size -1);
		else
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
		}
		break;
	}
	case 2:
	{
		return var::ssd(op1, atof(op2.c_str()));
		break;
	}
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}

//int8与array
var var::i8ss(const char op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2,templl))
		{
		case 1: return var::ds(op1, atof(op2.c_str())); break;
		case 3: return var::i64s(op1, templl); break;
		case 4: return var::i32s(op1, (int)templl); break;
		case 5: return var::i16s(op1, (short)templl); break;
		case 6: return var::i8s(op1, (char)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::ds(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i8sb(const char op1, const Bytes& op2)
{
	return var::i8ss(op1, var(op2).to_string());
}
//int16与array
var var::i16ss(const short op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::ds(op1, atof(op2.c_str())); break;
		case 3: return var::i64s(op1, templl); break;
		case 4: return var::i32s(op1, (int)templl); break;
		case 5: case 6: return var::i16s(op1, (short)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::ds(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i16sb(const short op1, const Bytes& op2)
{
	return var::i16ss(op1, var(op2).to_string());
}
//int32与array
var var::i32ss(const int op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::ds(op1, atof(op2.c_str())); break;
		case 3: return var::i64s(op1, templl); break;
		case 4: case 5: case 6: return var::i32s(op1, (int)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::ds(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i32sb(const int op1, const Bytes& op2)
{
	return var::i32ss(op1, var(op2).to_string());
}
//int64与array
var var::i64ss(const long long op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::ds((double)op1, atof(op2.c_str())); break;
		case 3: case 4: case 5: case 6: return var::i64s(op1, templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::ds((double)op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i64sb(const long long op1, const Bytes& op2)
{
	return var::i64ss(op1, var(op2).to_string());
}
//double、float与array
var var::dss(const double op1, const std::string& op2, const char size)

{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::ds(op1, atof(op2.c_str())); break;
		case 3: 
		case 4: 
			return var::ds(op1, (double)templl);
			break;
		case 5: 
		case 6: 
		{
			if(size)
				return var::ds(op1, (double)templl); 
			else
				return var::fs((float)op1, (float)templl);;
			break;
		}
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::ds(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::dsb(const double op1, const Bytes& op2, const char size)
{
	return var::dss(op1, var(op2).to_string());
}
//string、bytes与ints
//第三个参数取值取决于第二个参数的实际长度
// char short int <long long> 依次为0-3
var var::ssi(const std::string& op1, const long long& op2, const char len)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1: return var::ds(atof(op1.c_str()), (double)op2); break;
		case 3: return var::i64s(templl, op2); break;
		case 4:
		{
			if (len <= 2)return var::i32s((int)templl, (int)op2);
			else return var::i64s(templl, op2);
			break;
		}
		case 5:
		{
			if (len <= 1)return var::i16s((short)templl, (short)op2);
			else if (len <= 2)return var::i32s((int)templl, (int)op2);
			else return var::i64s(templl, op2);
			break;
		}
		case 6:
		{
			if (len == 0)return var::i8s((char)templl, (char)op2);
			else if (len == 1)return var::i16s((short)templl, (short)op2);
			else if (len <= 2)return var::i32s((int)templl, (int)op2);
			else return var::i64s(templl, op2);
			break;
		}
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bsi(const Bytes& op1, const long long& op2, const char len)
{
	return var::ssi(var(op1).to_string(), op2, len);
}
//string、bytes与浮点
//第三个参数取值取决于第二个参数的实际长度
var var::ssd(const std::string& op1, const double& op2, const char len)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1: return var::ds(atof(op1.c_str()), op2); break;
		case 3: case 4:	return var::ds((double)templl, op2); break;
		case 5:	case 6:
		{
			if (len)return var::ds((double)templl, op2);
			else return var::fs((float)templl, (float)op2);
			break;
		}
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bsd(const Bytes& op1, const double& op2, const char len)
{
	return var::ssd(var(op1).to_string(), op2, len);
}
var var::bss(const Bytes& op1, const std::string& op2)
{
	return var::ss(var(op1).to_string(), op2);
}
var var::ssb(const std::string& op1, const Bytes& op2)
{
	return var::ss(op1, var(op2).to_string());
}
var operator-(const var& op1, const var& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8sv(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8sv(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16sv(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32sv(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64sv(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dsv(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fsv(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::ssv(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bsv(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const var& op1, const bool op2)
{
	return var(op1 - ((char)op2));
}
var operator-(const bool op1, const var& op2)
{
	return ((char)op1) - op2;
}
var operator-(const var& op1, const char op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8s(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8s(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16s(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32s(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fs(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::ssi(*op1._data.string_d, op2, 0);
		break;
	case var::Type_t::bytes:
		return var::bsi(*op1._data.bytes_d, op2, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const char op1, const var& op2)
{
	return var::i8sv(op1, op2);
}
var operator-(const var& op1, const short op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16s(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i16s(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16s(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32s(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fs(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::ssi(*op1._data.string_d, op2, 1);
		break;
	case var::Type_t::bytes:
		return var::bsi(*op1._data.bytes_d, op2, 1);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const short op1, const var& op2)
{
	return var::i16sv(op1, op2);
}
var operator-(const var& op1, const int op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32s(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i32s(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i32s(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32s(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::ds(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::ssi(*op1._data.string_d, op2, 2);
		break;
	case var::Type_t::bytes:
		return var::bsi(*op1._data.bytes_d, op2, 2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const int op1, const var& op2)
{
	return var::i32sv(op1, op2);
}
var operator-(const var& op1, const long long op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64s(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i64s(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i64s(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i64s(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64s(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1._data.double_d, (double)op2);
		break;
	case var::Type_t::float_kind:
		return var::ds(op1._data.float_d, (double)op2);
		break;
	case var::Type_t::string_kind:
		return var::ssi(*op1._data.string_d, op2, 3);
		break;
	case var::Type_t::bytes:
		return var::bsi(*op1._data.bytes_d, op2, 3);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const long long op1, const var& op2)
{
	return var::i64sv(op1, op2);
}
var operator-(const var& op1, const float op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::fs(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::fs(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::fs(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::ds((double)op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::ds((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fs(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::ssd(*op1._data.string_d, op2, 0);
		break;
	case var::Type_t::bytes:
		return var::bsd(*op1._data.bytes_d, op2, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const float op1, const var& op2)
{
	return var::fsv(op1, op2);
}
var operator-(const var& op1, const double op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::ds(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::ds(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::ds(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::ds((double)op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::ds((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ds(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::ds(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::ssd(*op1._data.string_d, op2, 1);
		break;
	case var::Type_t::bytes:
		return var::bsd(*op1._data.bytes_d, op2, 1);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const double op1, const var& op2)
{
	return var::dsv(op1, op2);
}
var operator-(const var& op1, const std::string& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8ss(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8ss(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16ss(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32ss(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64ss(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dss(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dss(op1._data.float_d, op2, 0);
		break;
	case var::Type_t::string_kind:
		return var::ss(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bss(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const std::string& op1, const var& op2)
{
	return var::ssv(op1, op2);
}
var operator-(const var& op1, const char* const op2)
{
	return var(op1 - std::string(op2));
}
var operator-(const char* const op1, const var& op2)
{
	return var(std::string(op1) - op2);
}
var operator-(const var& op1, const Bytes& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8sb(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8sb(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16sb(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32sb(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64sb(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dsb(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dsb(op1._data.float_d, op2, 0);
		break;
	case var::Type_t::string_kind:
		return var::ssb(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bs(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator-(const Bytes& op1, const var& op2)
{
	return var::bsv(op1, op2);

}

// 减法组结束
// 乘法组开始

//已知op1类型，op2未知
var var::i8mv(const char op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8m(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i8m(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16m(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32m(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fm(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i8ms(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i8mb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i16mv(const short op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16m(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i16m(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16m(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32m(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fm(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i16ms(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i16mb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i32mv(const int op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32m(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i32m(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i32m(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32m(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::dm(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i32ms(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i32mb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i64mv(const long long op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64m(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i64m(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i64m(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i64m(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dm((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::dm((double)op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i64ms(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i64mb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::fmv(const float op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::fm(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::fm(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::fm(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::dm(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::dm(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fm(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::dms(op1, *op2._data.string_d, 0);
		break;
	case var::Type_t::bytes:
		return var::dmb(op1, *op2._data.bytes_d, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::dmv(const double op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::dm(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::dm(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::dm(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::dm(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::dm(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::dm(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::dms(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::dmb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::smv(const std::string& op1, const var& op2)
{
	//判断op2的类型
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::smi(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::smi(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::smi(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::smi(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::smi(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::smd();
		break;
	case var::Type_t::float_kind:
		return var::smd();
		break;
	case var::Type_t::string_kind:
		return var::sm(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::smb(op1, *op2._data.bytes_d);
		break;
	default:
		break;
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bmv(const Bytes& op1, const var& op2)
{
	return var::smv(var(op1).to_string(), op2);
}
//由string/bytes转换得的整型数与数值型变量相乘
// op1的取值也会影响相乘后的数值的类型
// 如果op2不是数值型而直接传入，会抛var()错误并返回none
var var::imn(const long long op1, const var& op2)
{
	char size = 3;
	if (((short)op1) == op1)
	{
		size = 1;
		if (((short)op1) == op1)
		{
			size = 0;
		}
	}
	else if (((int)op1) == op1)
		size = 2;
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
	{
		switch (size)
		{
		case 0: return var::i8m((char)op1, op2._data.bool_d); break;
		case 1: return var::i16m((short)op1, op2._data.bool_d); break;
		case 2: return var::i32m((int)op1, op2._data.bool_d); break;
		case 3: return var::i64m(op1, op2._data.bool_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int8_kind:
	{
		switch (size)
		{
		case 0: return var::i8m((char)op1, op2._data.int8_d); break;
		case 1: return var::i16m((short)op1, op2._data.int8_d); break;
		case 2: return var::i32m((int)op1, op2._data.int8_d); break;
		case 3: return var::i64m(op1, op2._data.int8_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int16_kind:
	{
		switch (size)
		{
		case 0:
		case 1: return var::i16m((short)op1, op2._data.int16_d); break;
		case 2: return var::i32m((int)op1, op2._data.int16_d); break;
		case 3: return var::i64m(op1, op2._data.int16_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int32_kind:
	{
		switch (size)
		{
		case 0:
		case 1:
		case 2: return var::i32m((int)op1, op2._data.int32_d); break;
		case 3: return var::i64m(op1, op2._data.int32_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int64_kind:
		return var::i64m(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dm((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		if (size >= 2)
			return var::dm((double)op1, op2._data.float_d);
		return var::fm((float)op1, op2._data.float_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
//同类型
var var::i8m(const char op1, const char op2)
{
	return var((char)(op1 * op2));
}
var var::i16m(const short op1, const short op2)
{
	return var((short)(op1 * op2));
}
var var::i32m(const int op1, const int op2)
{
	return var((int)(op1 * op2));
}
var var::i64m(const long long op1, const long long op2)
{
	return var((long long)(op1 * op2));
}
var var::fm(const float op1, const float op2)
{
	return var((float)(op1 * op2));
}
var var::dm(const double op1, const double op2)
{
	return var((double)(op1 * op2));
}
var var::bm(const Bytes& op1, const Bytes& op2)
{
	return var::sm(var(op1).to_string(), var(op2).to_string());
}
var var::sm(const std::string& op1, const std::string& op2)
{
	//由于无论如何op2应必为数值
	//复用ssi与ssd函数，因此只判定op2
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1:
			return var::smd();
			break;
		case 3: 
		case 4: 
		case 5: 
		case 6: return var::smi(op1, templl); break;
		default:
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
		}
		break;
	}
	case 2:
	{
		return var::smd();
		break;
	}
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
//int8与array
var var::i8ms(const char op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dm(op1, atof(op2.c_str())); break;
		case 3: return var::i64m(op1, templl); break;
		case 4: return var::i32m(op1, (int)templl); break;
		case 5: return var::i16m(op1, (short)templl); break;
		case 6: return var::i8m(op1, (char)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dm(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i8mb(const char op1, const Bytes& op2)
{
	return var::i8ms(op1, var(op2).to_string());
}
//int16与array
var var::i16ms(const short op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dm(op1, atof(op2.c_str())); break;
		case 3: return var::i64m(op1, templl); break;
		case 4: return var::i32m(op1, (int)templl); break;
		case 5: case 6: return var::i16m(op1, (short)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dm(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i16mb(const short op1, const Bytes& op2)
{
	return var::i16ms(op1, var(op2).to_string());
}
//int32与array
var var::i32ms(const int op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dm(op1, atof(op2.c_str())); break;
		case 3: return var::i64m(op1, templl); break;
		case 4: case 5: case 6: return var::i32m(op1, (int)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dm(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i32mb(const int op1, const Bytes& op2)
{
	return var::i32ms(op1, var(op2).to_string());
}
//int64与array
var var::i64ms(const long long op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dm((double)op1, atof(op2.c_str())); break;
		case 3: case 4: case 5: case 6: return var::i64m(op1, templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dm((double)op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i64mb(const long long op1, const Bytes& op2)
{
	return var::i64ms(op1, var(op2).to_string());
}
//double、float与array
var var::dms(const double op1, const std::string& op2, const char size) {
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dm(op1, atof(op2.c_str())); break;
		case 3:
		case 4:
			return var::dm(op1, (double)templl);
			break;
		case 5:
		case 6:
		{
			if (size)
				return var::dm(op1, (double)templl);
			else
				return var::fm((float)op1, (float)templl);;
			break;
		}
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dm(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::dmb(const double op1, const Bytes& op2, const char size)
{
	return var::dms(op1, var(op2).to_string());
}
//string、bytes与ints
var var::smi(const std::string& op1, const long long& op2)
{
	//进入字符串重复模式
	if (op2 >= 0)
	{
		var rt = "";
		for (long long i = 0; i < op2; ++i)
			rt._data.string_d->append(op1);
		return rt;
	}
	//不满足重复模式的要求
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bmi(const Bytes& op1, const long long& op2)
{
	return var::smi(var(op1).to_string(), op2);
}
//string、bytes浮点
//第三个参数取值取决于第二个参数的实际长度
//浮点不支持重复模式，因此不匹配就报错了
var var::smd()
{
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bmd()
{
	return var::smd();
}
var var::bms(const Bytes& op1, const std::string& op2)
{
	return var::sm(var(op1).to_string(), op2);
}
var var::smb(const std::string& op1, const Bytes& op2)
{
	return var::sm(op1, var(op2).to_string());
}
var operator*(const var& op1, const var& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8mv(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8mv(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16mv(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32mv(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64mv(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmv(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fmv(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::smv(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmv(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const var& op1, const bool op2)
{
	return var(op1 * ((char)op2));
}
var operator*(const bool op1, const var& op2)
{
	return ((char)op1) * op2;
}
var operator*(const var& op1, const char op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8m(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8m(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16m(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32m(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fm(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::smi(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmi(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const char op1, const var& op2)
{
	return var::i8mv(op1, op2);
}
var operator*(const var& op1, const short op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16m(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i16m(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16m(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32m(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fm(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::smi(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmi(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const short op1, const var& op2)
{
	return var::i16mv(op1, op2);
}
var operator*(const var& op1, const int op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32m(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i32m(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i32m(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32m(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dm(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::smi(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmi(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const int op1, const var& op2)
{
	return var::i32mv(op1, op2);
}
var operator*(const var& op1, const long long op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64m(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i64m(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i64m(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i64m(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64m(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1._data.double_d, (double)op2);
		break;
	case var::Type_t::float_kind:
		return var::dm(op1._data.float_d, (double)op2);
		break;
	case var::Type_t::string_kind:
		return var::smi(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmi(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const long long op1, const var& op2)
{
	return var::i64mv(op1, op2);
}
var operator*(const var& op1, const float op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::fm(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::fm(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::fm(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::dm((double)op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::dm((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fm(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::smd();
		break;
	case var::Type_t::bytes:
		return var::bmd();
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const float op1, const var& op2)
{
	return var::fmv(op1, op2);
}
var operator*(const var& op1, const double op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::dm(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::dm(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::dm(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::dm((double)op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::dm((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dm(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dm(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::smd();
		break;
	case var::Type_t::bytes:
		return var::bmd();
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const double op1, const var& op2)
{
	return var::dmv(op1, op2);
}
var operator*(const var& op1, const std::string& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8ms(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8ms(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16ms(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32ms(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64ms(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dms(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dms(op1._data.float_d, op2, 0);
		break;
	case var::Type_t::string_kind:
		return var::sm(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bms(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const std::string& op1, const var& op2)
{
	return var::smv(op1, op2);
}
var operator*(const var& op1, const char* const op2)
{
	return var(op1 * std::string(op2));
}
var operator*(const char* const op1, const var& op2)
{
	return var(std::string(op1) * op2);
}
var operator*(const var& op1, const Bytes& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8mb(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8mb(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16mb(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32mb(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64mb(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmb(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dmb(op1._data.float_d, op2, 0);
		break;
	case var::Type_t::string_kind:
		return var::smb(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bm(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator*(const Bytes& op1, const var& op2)
{
	return var::bmv(op1, op2);
}
// 乘法组结束
// 除法组开始

var var::i8dv(const char op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8d(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i8d(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16d(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32d(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fd(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i8ds(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i8db(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i16dv(const short op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16d(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i16d(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16d(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32d(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fd(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i16ds(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i16db(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i32dv(const int op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32d(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i32d(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i32d(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32d(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::dd(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i32ds(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i32db(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i64dv(const long long op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64d(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i64d(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i64d(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i64d(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dd((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::dd((double)op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::i64ds(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i64db(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::fdv(const float op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::fd(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::fd(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::fd(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::dd(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::dd(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::fd(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::dds(op1, *op2._data.string_d, 0);
		break;
	case var::Type_t::bytes:
		return var::ddb(op1, *op2._data.bytes_d, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::ddv(const double op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::dd(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::dd(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::dd(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::dd(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::dd(op1, (double)op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		return var::dd(op1, op2._data.float_d);
		break;
	case var::Type_t::string_kind:
		return var::dds(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::ddb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::sdv(const std::string& op1, const var& op2)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1:	return var::ddv(atof(op1.c_str()), op2); break;
		case 3: return var::i64dv(templl, op2); break;
		case 4: return var::i32dv((int)templl, op2); break;
		case 5: return var::i16dv((short)templl, op2); break;
		case 6: return var::i8dv((char)templl, op2); break;
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bdv(const Bytes& op1, const var& op2)
{
	return var::sdv(var(op1).to_string(), op2);
}
var var::idn(const long long op1, const var& op2)
{
	char size = 3;
	if (((short)op1) == op1)
	{
		size = 1;
		if (((short)op1) == op1)
		{
			size = 0;
		}
	}
	else if (((int)op1) == op1)
		size = 2;
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
	{
		switch (size)
		{
		case 0: return var::i8d((char)op1, op2._data.bool_d); break;
		case 1: return var::i16d((short)op1, op2._data.bool_d); break;
		case 2: return var::i32d((int)op1, op2._data.bool_d); break;
		case 3: return var::i64d(op1, op2._data.bool_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int8_kind:
	{
		switch (size)
		{
		case 0: return var::i8d((char)op1, op2._data.int8_d); break;
		case 1: return var::i16d((short)op1, op2._data.int8_d); break;
		case 2: return var::i32d((int)op1, op2._data.int8_d); break;
		case 3: return var::i64d(op1, op2._data.int8_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int16_kind:
	{
		switch (size)
		{
		case 0:
		case 1: return var::i16d((short)op1, op2._data.int16_d); break;
		case 2: return var::i32d((int)op1, op2._data.int16_d); break;
		case 3: return var::i64d(op1, op2._data.int16_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int32_kind:
	{
		switch (size)
		{
		case 0:
		case 1:
		case 2: return var::i32d((int)op1, op2._data.int32_d); break;
		case 3: return var::i64d(op1, op2._data.int32_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int64_kind:
		return var::i64d(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dd((double)op1, op2._data.double_d);
		break;
	case var::Type_t::float_kind:
		if (size >= 2)
			return var::dd((double)op1, op2._data.float_d);
		return var::fd((float)op1, op2._data.float_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
//同类型相除
var var::i8d(const char op1, const char op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	return var((char)(op1 / op2));
}
var var::i16d(const short op1, const short op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	return var((short)(op1 / op2));
}
var var::i32d(const int op1, const int op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_DIV_ZERO
	return var((int)(op1 / op2));
}
var var::i64d(const long long op1, const long long op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	return var((long long)(op1 / op2));
}
var var::fd(const float op1, const float op2)
{
#ifdef DYNAMIC_VAR_INF2NONE
	if ((op2 == 0.0f) && (op1))
		return var();
#endif // DYNAMIC_VAR_INF2NONE
#ifdef DYNAMIC_VAR_NAN2NONE
	if ((op2 == 0.0f) && (op1 == 0.0f))
		return var();
#endif // DYNAMIC_VAR_NAN2NONE
	return var((float)(op1 / op2));
}
var var::dd(const double op1, const double op2)
{
#ifdef DYNAMIC_VAR_INF2NONE
	if ((op2 == 0.0) && (op1))
		return var();
#endif // DYNAMIC_VAR_INF2NONE
#ifdef DYNAMIC_VAR_NAN2NONE
	if ((op2 == 0.0) && (op1 == 0.0))
		return var();
#endif // DYNAMIC_VAR_NAN2NONE
	return var((double)(op1 / op2));
}
var var::bd(const Bytes& op1, const Bytes& op2)
{
	return var::sd(var(op1).to_string(), var(op2).to_string());
}
var var::sd(const std::string& op1, const std::string& op2)
{
	//复用ssi与ssd函数，因此只判定op2
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		char size = 0;
		switch (test_to_num(op2, templl))
		{
		case 1:
			return var::sdd(op1, atof(op2.c_str()));
			break;
		case 3: size = 4; break;
		case 4: size = 3; break;
		case 5: size = 2; break;
		case 6: size = 1; break;
		default:
			break;
		}
		if (size)
			return var::sdi(op1, templl, size - 1);
		else
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
		}
		break;
	}
	case 2:
	{
		return var::sdd(op1, atof(op2.c_str()));
		break;
	}
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
//int8与array
var var::i8ds(const char op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dd(op1, atof(op2.c_str())); break;
		case 3: return var::i64d(op1, templl); break;
		case 4: return var::i32d(op1, (int)templl); break;
		case 5: return var::i16d(op1, (short)templl); break;
		case 6: return var::i8d(op1, (char)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dd(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i8db(const char op1, const Bytes& op2)
{
	return var::i8ds(op1, var(op2).to_string());
}
//int16与array
var var::i16ds(const short op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dd(op1, atof(op2.c_str())); break;
		case 3: return var::i64d(op1, templl); break;
		case 4: return var::i32d(op1, (int)templl); break;
		case 5: case 6: return var::i16d(op1, (short)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dd(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i16db(const short op1, const Bytes& op2)
{
	return var::i16ds(op1, var(op2).to_string());
}
//int32与array
var var::i32ds(const int op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dd(op1, atof(op2.c_str())); break;
		case 3: return var::i64d(op1, templl); break;
		case 4: case 5: case 6: return var::i32d(op1, (int)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dd(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i32db(const int op1, const Bytes& op2)
{
	return var::i32ds(op1, var(op2).to_string());
}
//int64与array
var var::i64ds(const long long op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dd((double)op1, atof(op2.c_str())); break;
		case 3: case 4: case 5: case 6: return var::i64d(op1, templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dd((double)op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i64db(const long long op1, const Bytes& op2)
{
	return var::i64ds(op1, var(op2).to_string());
}
//double、float与array
var var::dds(const double op1, const std::string& op2, const char size)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dd(op1, atof(op2.c_str())); break;
		case 3:
		case 4:
			return var::dd(op1, (double)templl);
			break;
		case 5:
		case 6:
		{
			if (size)
				return var::dd(op1, (double)templl);
			else
				return var::fd((float)op1, (float)templl);;
			break;
		}
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dd(op1, atof(op2.c_str()));
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::ddb(const double op1, const Bytes& op2, const char size)
{
	return var::dds(op1, var(op2).to_string());
}
//string、bytes与ints
//第三个参数取值取决于第二个参数的实际长度
// char short int <long long> 依次为0-3
var var::sdi(const std::string& op1, const long long& op2, const char len)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1: return var::dd(atof(op1.c_str()), (double)op2); break;
		case 3: return var::i64d(templl, op2); break;
		case 4:
		{
			if (len <= 2)return var::i32d((int)templl, (int)op2);
			else return var::i64d(templl, op2);
			break;
		}
		case 5:
		{
			if (len <= 1)return var::i16d((short)templl, (short)op2);
			else if (len <= 2)return var::i32d((int)templl, (int)op2);
			else return var::i64d(templl, op2);
			break;
		}
		case 6:
		{
			if (len == 0)return var::i8d((char)templl, (char)op2);
			else if (len == 1)return var::i16d((short)templl, (short)op2);
			else if (len <= 2)return var::i32d((int)templl, (int)op2);
			else return var::i64d(templl, op2);
			break;
		}
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bdi(const Bytes& op1, const long long& op2, const char len)
{
	return var::sdi(var(op1).to_string(), op2, len);
}
//string、bytes与浮点
//第三个参数取值取决于第二个参数的实际长度
var var::sdd(const std::string& op1, const double& op2, const char len)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1: return var::dd(atof(op1.c_str()), op2); break;
		case 3: case 4:	return var::dd((double)templl, op2); break;
		case 5:	case 6:
		{
			if (len)return var::dd((double)templl, op2);
			else return var::fd((float)templl, (float)op2);
			break;
		}
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bdd(const Bytes& op1, const double& op2, const char len)
{
	return var::sdd(var(op1).to_string(), op2, len);
}
var var::bds(const Bytes& op1, const std::string& op2)
{
	return var::sd(var(op1).to_string(), op2);
}
var var::sdb(const std::string& op1, const Bytes& op2)
{
	return var::sd(op1, var(op2).to_string());
}
var operator/(const var& op1, const var& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8dv(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8dv(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16dv(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32dv(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64dv(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ddv(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fdv(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::sdv(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bdv(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const var& op1, const bool op2)
{
	return var(op1 / ((char)op2));
}
var operator/(const bool op1, const var& op2)
{
	return ((char)op1) / op2;
}
var operator/(const var& op1, const char op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8d(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8d(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16d(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32d(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fd(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::sdi(*op1._data.string_d, op2, 0);
		break;
	case var::Type_t::bytes:
		return var::bdi(*op1._data.bytes_d, op2, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const char op1, const var& op2)
{
	return var::i8dv(op1, op2);
}
var operator/(const var& op1, const short op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16d(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i16d(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16d(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32d(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fd(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::sdi(*op1._data.string_d, op2, 1);
		break;
	case var::Type_t::bytes:
		return var::bdi(*op1._data.bytes_d, op2, 1);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const short op1, const var& op2)
{
	return var::i16dv(op1, op2);
}
var operator/(const var& op1, const int op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32d(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i32d(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i32d(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32d(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dd(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::sdi(*op1._data.string_d, op2, 2);
		break;
	case var::Type_t::bytes:
		return var::bdi(*op1._data.bytes_d, op2, 2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const int op1, const var& op2)
{
	return var::i32dv(op1, op2);
}
var operator/(const var& op1, const long long op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64d(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i64d(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i64d(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i64d(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64d(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1._data.double_d, (double)op2);
		break;
	case var::Type_t::float_kind:
		return var::dd(op1._data.float_d, (double)op2);
		break;
	case var::Type_t::string_kind:
		return var::sdi(*op1._data.string_d, op2, 3);
		break;
	case var::Type_t::bytes:
		return var::bdi(*op1._data.bytes_d, op2, 3);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const long long op1, const var& op2)
{
	return var::i64dv(op1, op2);
}
var operator/(const var& op1, const float op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::fd(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::fd(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::fd(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::dd((double)op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::dd((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::fd(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::sdd(*op1._data.string_d, op2, 0);
		break;
	case var::Type_t::bytes:
		return var::bdd(*op1._data.bytes_d, op2, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const float op1, const var& op2)
{
	return var::fdv(op1, op2);
}
var operator/(const var& op1, const double op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::dd(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::dd(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::dd(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::dd((double)op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::dd((double)op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dd(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dd(op1._data.float_d, op2);
		break;
	case var::Type_t::string_kind:
		return var::sdd(*op1._data.string_d, op2, 1);
		break;
	case var::Type_t::bytes:
		return var::bdd(*op1._data.bytes_d, op2, 1);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const double op1, const var& op2)
{
	return var::ddv(op1, op2);
}
var operator/(const var& op1, const std::string& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8ds(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8ds(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16ds(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32ds(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64ds(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dds(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::dds(op1._data.float_d, op2, 0);
		break;
	case var::Type_t::string_kind:
		return var::sd(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bds(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const std::string& op1, const var& op2)
{
	return var::sdv(op1, op2);

}
var operator/(const var& op1, const char* const op2)
{
	return var(op1 / std::string(op2));
}
var operator/(const char* const op1, const var& op2)
{
	return var(std::string(op1) / op2);
}
var operator/(const var& op1, const Bytes& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8db(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8db(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16db(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32db(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64db(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::ddb(op1._data.double_d, op2);
		break;
	case var::Type_t::float_kind:
		return var::ddb(op1._data.float_d, op2, 0);
		break;
	case var::Type_t::string_kind:
		return var::sdb(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bd(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator/(const Bytes& op1, const var& op2)
{
	return var::bdv(op1, op2);
}
// 除法组结束
// 取余组开始
var var::i8modv(const char op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8mod(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i8mod(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16mod(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32mod(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::fmod();
		break;
	case var::Type_t::string_kind:
		return var::i8mods(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i8modb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i16modv(const short op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16mod(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i16mod(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i16mod(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32mod(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::fmod();
		break;
	case var::Type_t::string_kind:
		return var::i16mods(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i16modb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i32modv(const int op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32mod(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i32mod(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i32mod(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i32mod(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::dmod();
		break;
	case var::Type_t::string_kind:
		return var::i32mods(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i32modb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::i64modv(const long long op1, const var& op2)
{
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64mod(op1, op2._data.bool_d);
		break;
	case var::Type_t::int8_kind:
		return var::i64mod(op1, op2._data.int8_d);
		break;
	case var::Type_t::int16_kind:
		return var::i64mod(op1, op2._data.int16_d);
		break;
	case var::Type_t::int32_kind:
		return var::i64mod(op1, op2._data.int32_d);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::dmod();
		break;
	case var::Type_t::string_kind:
		return var::i64mods(op1, *op2._data.string_d);
		break;
	case var::Type_t::bytes:
		return var::i64modb(op1, *op2._data.bytes_d);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var var::fmodv()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
var var::dmodv()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
var var::smodv(const std::string& op1, const var& op2)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1:	return var::dmodv(); break;
		case 3: return var::i64modv(templl, op2); break;
		case 4: return var::i32modv((int)templl, op2); break;
		case 5: return var::i16modv((short)templl, op2); break;
		case 6: return var::i8modv((char)templl, op2); break;
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bmodv(const Bytes& op1, const var& op2)
{
	return var::smodv(var(op1).to_string(), op2);
}
var var::imodn(const long long op1, const var& op2)
{
	char size = 3;
	if (((short)op1) == op1)
	{
		size = 1;
		if (((short)op1) == op1)
		{
			size = 0;
		}
	}
	else if (((int)op1) == op1)
		size = 2;
	switch (op2._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
	{
		switch (size)
		{
		case 0: return var::i8mod((char)op1, op2._data.bool_d); break;
		case 1: return var::i16mod((short)op1, op2._data.bool_d); break;
		case 2: return var::i32mod((int)op1, op2._data.bool_d); break;
		case 3: return var::i64mod(op1, op2._data.bool_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int8_kind:
	{
		switch (size)
		{
		case 0: return var::i8mod((char)op1, op2._data.int8_d); break;
		case 1: return var::i16mod((short)op1, op2._data.int8_d); break;
		case 2: return var::i32mod((int)op1, op2._data.int8_d); break;
		case 3: return var::i64mod(op1, op2._data.int8_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int16_kind:
	{
		switch (size)
		{
		case 0:
		case 1: return var::i16mod((short)op1, op2._data.int16_d); break;
		case 2: return var::i32mod((int)op1, op2._data.int16_d); break;
		case 3: return var::i64mod(op1, op2._data.int16_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int32_kind:
	{
		switch (size)
		{
		case 0:
		case 1:
		case 2: return var::i32mod((int)op1, op2._data.int32_d); break;
		case 3: return var::i64mod(op1, op2._data.int32_d); break;
		default:
			break;
		}
		throw var();
		return var();
		break;
	}
	case var::Type_t::int64_kind:
		return var::i64mod(op1, op2._data.int64_d);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::fmod();
		break;
	default:
		throw var();
		return var();
		break;
	}
}
//同类型相除
var var::i8mod(const char op1, const char op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	return var((char)(op1 % op2));
}
var var::i16mod(const short op1, const short op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	return var((short)(op1 % op2));
}
var var::i32mod(const int op1, const int op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_DIV_ZERO
	return var((int)(op1 % op2));
}
var var::i64mod(const long long op1, const long long op2)
{
#ifdef DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	if (!op2)
		return var();
#endif // DYNAMIC_VAR_MUTE_INT_DIV_ZERO
	return var((long long)(op1 % op2));
}
var var::fmod()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
var var::dmod()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
var var::bmod(const Bytes& op1, const Bytes& op2)
{
	return var::smod(var(op1).to_string(), var(op2).to_string());
}
var var::smod(const std::string& op1, const std::string& op2)
{
	//复用ssi与ssd函数，因此只判定op2
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		char size = 0;
		switch (test_to_num(op2, templl))
		{
		case 1:
			return var::smodd();
			break;
		case 3: size = 4; break;
		case 4: size = 3; break;
		case 5: size = 2; break;
		case 6: size = 1; break;
		default:
			break;
		}
		if (size)
			return var::smodi(op1, templl, size - 1);
		else
		{
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
		}
		break;
	}
	case 2:
	{
		return var::smodd();
		break;
	}
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
//int8与array
var var::i8mods(const char op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dmod(); break;
		case 3: return var::i64mod(op1, templl); break;
		case 4: return var::i32mod(op1, (int)templl); break;
		case 5: return var::i16mod(op1, (short)templl); break;
		case 6: return var::i8mod(op1, (char)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dmod();
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i8modb(const char op1, const Bytes& op2)
{
	return var::i8mods(op1, var(op2).to_string());
}
//int16与array
var var::i16mods(const short op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dmod(); break;
		case 3: return var::i64mod(op1, templl); break;
		case 4: return var::i32mod(op1, (int)templl); break;
		case 5: case 6: return var::i16mod(op1, (short)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dmod();
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i16modb(const short op1, const Bytes& op2)
{
	return var::i16mods(op1, var(op2).to_string());
}
//int32与array
var var::i32mods(const int op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dmod(); break;
		case 3: return var::i64mod(op1, templl); break;
		case 4: case 5: case 6: return var::i32mod(op1, (int)templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dmod();
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i32modb(const int op1, const Bytes& op2)
{
	return var::i32mods(op1, var(op2).to_string());
}
//int64与array
var var::i64mods(const long long op1, const std::string& op2)
{
	switch (test_all_num(op2))
	{
	case 1:
	{
		long long templl = 0;
		switch (test_to_num(op2, templl))
		{
		case 1: return var::dmod(); break;
		case 3: case 4: case 5: case 6: return var::i64mod(op1, templl); break;
		default:
#ifndef DYNAMIC_VAR_MUTE_VIA
			throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
			return var();
			break;
		}
	}
	case 2:
		return var::dmod();
		break;
	default:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return var();
		break;
	}
}
var var::i64modb(const long long op1, const Bytes& op2)
{
	return var::i64mods(op1, var(op2).to_string());
}
//double、float与array
var var::dmods()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
var var::dmodb()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
//string、bytes与ints
//第三个参数取值取决于第二个参数的实际长度
// char short int <long long> 依次为0-3
var var::smodi(const std::string& op1, const long long& op2, const char len)
{
	if (test_all_num(op1))
	{
		long long templl = 0;
		switch (test_to_num(op1, templl))
		{
		case 1: return var::dmod(); break;
		case 3: return var::i64mod(templl, op2); break;
		case 4:
		{
			if (len <= 2)return var::i32mod((int)templl, (int)op2);
			else return var::i64mod(templl, op2);
			break;
		}
		case 5:
		{
			if (len <= 1)return var::i16mod((short)templl, (short)op2);
			else if (len <= 2)return var::i32mod((int)templl, (int)op2);
			else return var::i64mod(templl, op2);
			break;
		}
		case 6:
		{
			if (len == 0)return var::i8mod((char)templl, (char)op2);
			else if (len == 1)return var::i16mod((short)templl, (short)op2);
			else if (len <= 2)return var::i32mod((int)templl, (int)op2);
			else return var::i64mod(templl, op2);
			break;
		}
		default:
			break;
		}
	}
#ifndef DYNAMIC_VAR_MUTE_VIA
	throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
	return var();
}
var var::bmodi(const Bytes& op1, const long long& op2, const char len)
{
	return var::smodi(var(op1).to_string(), op2, len);
}
//string、bytes与浮点
//第三个参数取值取决于第二个参数的实际长度
var var::smodd()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
var var::bmodd()
{
#ifndef DYNAMIC_VAR_MUTE_DOUBLE
	throw double(0.0);
#endif // !DYNAMIC_VAR_MUTE_DOUBLE
	return var();
}
var var::bmods(const Bytes& op1, const std::string& op2)
{
	return var::smod(var(op1).to_string(), op2);
}
var var::smodb(const std::string& op1, const Bytes& op2)
{
	return var::smod(op1, var(op2).to_string());
}
var operator%(const var& op1, const var& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8modv(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8modv(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16modv(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32modv(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64modv(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmodv();
		break;
	case var::Type_t::float_kind:
		return var::fmodv();
		break;
	case var::Type_t::string_kind:
		return var::smodv(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmodv(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator%(const var& op1, const bool op2)
{
	return var(op1 % ((char)op2));
}
var operator%(const bool op1, const var& op2)
{
	return ((char)op1) % op2;
}
var operator%(const var& op1, const char op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8mod(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8mod(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16mod(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32mod(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::fmod();
		break;
	case var::Type_t::string_kind:
		return var::smodi(*op1._data.string_d, op2, 0);
		break;
	case var::Type_t::bytes:
		return var::bmodi(*op1._data.bytes_d, op2, 0);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator%(const char op1, const var& op2)
{
	return var::i8modv(op1, op2);
}
var operator%(const var& op1, const short op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i16mod(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i16mod(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16mod(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32mod(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::fmod();
		break;
	case var::Type_t::string_kind:
		return var::smodi(*op1._data.string_d, op2, 1);
		break;
	case var::Type_t::bytes:
		return var::bmodi(*op1._data.bytes_d, op2, 1);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator%(const short op1, const var& op2)
{
	return var::i16modv(op1, op2);
}
var operator%(const var& op1, const int op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i32mod(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i32mod(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i32mod(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32mod(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::dmod();
		break;
	case var::Type_t::string_kind:
		return var::smodi(*op1._data.string_d, op2, 2);
		break;
	case var::Type_t::bytes:
		return var::bmodi(*op1._data.bytes_d, op2, 2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator%(const int op1, const var& op2)
{
	return var::i32modv(op1, op2);
}
var operator%(const var& op1, const long long op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i64mod(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i64mod(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i64mod(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i64mod(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64mod(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmod();
		break;
	case var::Type_t::float_kind:
		return var::dmod();
		break;
	case var::Type_t::string_kind:
		return var::smodi(*op1._data.string_d, op2, 3);
		break;
	case var::Type_t::bytes:
		return var::bmodi(*op1._data.bytes_d, op2, 3);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator%(const long long op1, const var& op2)
{
	return var::i64modv(op1, op2);
}
var operator%(const var& op1, const std::string& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8mods(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8mods(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16mods(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32mods(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64mods(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmods();
		break;
	case var::Type_t::float_kind:
		return var::dmods();
		break;
	case var::Type_t::string_kind:
		return var::smod(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmods(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator%(const std::string& op1, const var& op2)
{
	return var::smodv(op1, op2);

}
var operator%(const var& op1, const char* const op2)
{
	return var(op1 % std::string(op2));
}
var operator%(const char* const op1, const var& op2)
{
	return var(std::string(op1) % op2);
}
var operator%(const var& op1, const Bytes& op2)
{
	switch (op1._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return var();
		break;
	case var::Type_t::bool_kind:
		return var::i8modb(op1._data.bool_d, op2);
		break;
	case var::Type_t::int8_kind:
		return var::i8modb(op1._data.int8_d, op2);
		break;
	case var::Type_t::int16_kind:
		return var::i16modb(op1._data.int16_d, op2);
		break;
	case var::Type_t::int32_kind:
		return var::i32modb(op1._data.int32_d, op2);
		break;
	case var::Type_t::int64_kind:
		return var::i64modb(op1._data.int64_d, op2);
		break;
	case var::Type_t::double_kind:
		return var::dmodb();
		break;
	case var::Type_t::float_kind:
		return var::dmodb();
		break;
	case var::Type_t::string_kind:
		return var::smodb(*op1._data.string_d, op2);
		break;
	case var::Type_t::bytes:
		return var::bmod(*op1._data.bytes_d, op2);
		break;
	default:
		throw var();
		return var();
		break;
	}
}
var operator%(const Bytes& op1, const var& op2)
{
	return var::bmodv(op1, op2);
}
// 取余组结束
// 自增自减组开始
var& operator++(var& op)
{
	switch (op._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return op;
		break;
	case var::Type_t::bool_kind:
		op._data.bool_d = true;
		return op;
		break;
	case var::Type_t::int8_kind:
		++op._data.int8_d;
		return op;
		break;
	case var::Type_t::int16_kind:
		++op._data.int16_d;
		return op;
		break;
	case var::Type_t::int32_kind:
		++op._data.int32_d;
		return op;
		break;
	case var::Type_t::int64_kind:
		++op._data.int64_d;
		return op;
		break;
	case var::Type_t::double_kind:
		++op._data.double_d;
		return op;
		break;
	case var::Type_t::float_kind:
		++op._data.float_d;
		return op;
		break;
	case var::Type_t::string_kind:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return op;
		break;
	case var::Type_t::bytes:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return op;
		break;
	default:
		throw var();
		return op;
		break;
	}
}
var operator++(var& op, int)
{
	var rt(op);
	switch (op._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return rt;
		break;
	case var::Type_t::bool_kind:
		op._data.bool_d = true;
		return rt;
		break;
	case var::Type_t::int8_kind:
		++op._data.int8_d;
		return rt;
		break;
	case var::Type_t::int16_kind:
		++op._data.int16_d;
		return rt;
		break;
	case var::Type_t::int32_kind:
		++op._data.int32_d;
		return rt;
		break;
	case var::Type_t::int64_kind:
		++op._data.int64_d;
		return rt;
		break;
	case var::Type_t::double_kind:
		++op._data.double_d;
		return rt;
		break;
	case var::Type_t::float_kind:
		++op._data.float_d;
		return rt;
		break;
	case var::Type_t::string_kind:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return rt;
		break;
	case var::Type_t::bytes:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return rt;
		break;
	default:
		throw var();
		return rt;
		break;
	}
}
var& operator--(var& op)
{
	switch (op._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return op;
		break;
	case var::Type_t::bool_kind:
		op._data.bool_d = false;
		return op;
		break;
	case var::Type_t::int8_kind:
		--op._data.int8_d;
		return op;
		break;
	case var::Type_t::int16_kind:
		--op._data.int16_d;
		return op;
		break;
	case var::Type_t::int32_kind:
		--op._data.int32_d;
		return op;
		break;
	case var::Type_t::int64_kind:
		--op._data.int64_d;
		return op;
		break;
	case var::Type_t::double_kind:
		--op._data.double_d;
		return op;
		break;
	case var::Type_t::float_kind:
		--op._data.float_d;
		return op;
		break;
	case var::Type_t::string_kind:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return op;
		break;
	case var::Type_t::bytes:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return op;
		break;
	default:
		throw var();
		return op;
		break;
	}
}
var operator--(var& op, int)
{
	var rt(op);
	switch (op._type)
	{
	case var::Type_t::none:
#ifndef DYNAMIC_VAR_MUTE_OPNONE
		throw var::opNone();
#endif // !DYNAMIC_VAR_MUTE_OPNONE
		return rt;
		break;
	case var::Type_t::bool_kind:
		op._data.bool_d = false;
		return rt;
		break;
	case var::Type_t::int8_kind:
		--op._data.int8_d;
		return rt;
		break;
	case var::Type_t::int16_kind:
		--op._data.int16_d;
		return rt;
		break;
	case var::Type_t::int32_kind:
		--op._data.int32_d;
		return rt;
		break;
	case var::Type_t::int64_kind:
		--op._data.int64_d;
		return rt;
		break;
	case var::Type_t::double_kind:
		--op._data.double_d;
		return rt;
		break;
	case var::Type_t::float_kind:
		--op._data.float_d;
		return rt;
		break;
	case var::Type_t::string_kind:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return rt;
		break;
	case var::Type_t::bytes:
#ifndef DYNAMIC_VAR_MUTE_VIA
		throw var::ViA();
#endif // !DYNAMIC_VAR_MUTE_VIA
		return rt;
		break;
	default:
		throw var();
		return rt;
		break;
	}
}
// 自增自减组结束


#ifdef DYNAMIC_VAR_ENABLE_ALL_ARITH_OP

#endif // DYNAMIC_VAR_ENABLE_ALL_ARITH_OP
/* 算术运算重载结束 */
#endif // DYNAMIC_VAR_ENABLE_ARITH_OP


#ifdef DYNAMIC_VAR_ENABLE_LOGIC_OP
/* 逻辑运算重载开始 */
bool operator&&(const var& op1, const var& op2)
{
	return (op1.to_bool()) && (op2.to_bool());
}
bool operator&&(const var& op1, const bool op2){return op1.to_bool() && op2; }
bool operator&&(const bool op1, const var& op2){return op1 && op2.to_bool(); }
bool operator&&(const var& op1, const char op2){return op1.to_bool() && op2; }
bool operator&&(const char op1, const var& op2){return op1 && op2.to_bool(); }
bool operator&&(const var& op1, const short op2){return op1.to_bool() && op2; }
bool operator&&(const short op1, const var& op2){return op1 && op2.to_bool(); }
bool operator&&(const var& op1, const int op2){return op1.to_bool() && op2; }
bool operator&&(const int op1, const var& op2){return op1 && op2.to_bool(); }
bool operator&&(const var& op1, const long long op2){return op1.to_bool() && op2; }
bool operator&&(const long long op1, const var& op2){return op1 && op2.to_bool(); }
bool operator&&(const var& op1, const float op2){return op1.to_bool() && op2; }
bool operator&&(const float op1, const var& op2){return op1 && op2.to_bool(); }
bool operator&&(const var& op1, const double op2){return op1.to_bool() && op2; }
bool operator&&(const double op1, const var& op2){return op1 && op2.to_bool(); }
bool operator&&(const var& op1, const std::string& op2){return op1 && var(op2);}
bool operator&&(const std::string& op1, const var& op2){return var(op1) && op2;}
bool operator&&(const var& op1, const char* const op2){return op1 && var(op2);}
bool operator&&(const char* const op1, const var& op2){return var(op1) && op2;}
bool operator&&(const var& op1, const Bytes& op2){return op1 && var(op2);}
bool operator&&(const Bytes& op1, const var& op2){return var(op1) && op2;}
bool operator||(const var& op1, const var& op2)
{
	return (op1.to_bool()) || (op2.to_bool());
}
bool operator||(const var& op1, const bool op2) { return op1.to_bool() || op2; }
bool operator||(const bool op1, const var& op2) { return op1 || op2.to_bool(); }
bool operator||(const var& op1, const char op2) { return op1.to_bool() || op2; }
bool operator||(const char op1, const var& op2) { return op1 || op2.to_bool(); }
bool operator||(const var& op1, const short op2) { return op1.to_bool() || op2; }
bool operator||(const short op1, const var& op2) { return op1 || op2.to_bool(); }
bool operator||(const var& op1, const int op2) { return op1.to_bool() || op2; }
bool operator||(const int op1, const var& op2) { return op1 || op2.to_bool(); }
bool operator||(const var& op1, const long long op2) { return op1.to_bool() || op2; }
bool operator||(const long long op1, const var& op2) { return op1 || op2.to_bool(); }
bool operator||(const var& op1, const float op2) { return op1.to_bool() || op2; }
bool operator||(const float op1, const var& op2) { return op1 || op2.to_bool(); }
bool operator||(const var& op1, const double op2) { return op1.to_bool() || op2; }
bool operator||(const double op1, const var& op2) { return op1 || op2.to_bool(); }
bool operator||(const var& op1, const std::string& op2) { return op1 || var(op2); }
bool operator||(const std::string& op1, const var& op2) { return var(op1) || op2; }
bool operator||(const var& op1, const char* const op2) { return op1 || var(op2); }
bool operator||(const char* const op1, const var& op2) { return var(op1) || op2; }
bool operator||(const var& op1, const Bytes& op2) { return op1 || var(op2); }
bool operator||(const Bytes& op1, const var& op2) { return var(op1) || op2; }
bool operator!(const var& op)
{
	return !(op.to_bool());
}
/* 逻辑运算重载结束 */
#endif // DYNAMIC_VAR_ENABLE_LOGIC_OP

#ifdef DYNAMIC_VAR_ENABLE_BIT_OP
/* 位运算重载开始 */

/* 位运算重载结束 */
#endif // DYNAMIC_VAR_ENABLE_BIT_OP

#ifdef DYNAMIC_VAR_ENABLE_ASSIGN_OP
/* 赋值运算重载开始 */
// 加
var& var::operator+=(const var& op){	*this = *this + op;	return *this;}
var& var::operator+=(const bool op){	*this = *this + op;	return *this;}
var& var::operator+=(const char op){	*this = *this + op;	return *this;}
var& var::operator+=(const short op){	*this = *this + op;	return *this;}
var& var::operator+=(const int op){	*this = *this + op;	return *this;}
var& var::operator+=(const long long op){	*this = *this + op;	return *this;}
var& var::operator+=(const float op){	*this = *this + op;	return *this;}
var& var::operator+=(const double op){	*this = *this + op;	return *this;}
var& var::operator+=(const std::string& op){	*this = *this + op;	return *this;}
var& var::operator+=(const char* const op){	*this = *this + op;	return *this;}
var& var::operator+=(const Bytes& op){	*this = *this + op;	return *this;}
// 减
var& var::operator-=(const var& op){	*this = *this - op;	return *this;}
var& var::operator-=(const bool op){	*this = *this - op;	return *this;}
var& var::operator-=(const char op){	*this = *this - op;	return *this;}
var& var::operator-=(const short op){	*this = *this - op;	return *this;}
var& var::operator-=(const int op){	*this = *this - op;	return *this;}
var& var::operator-=(const long long op){	*this = *this - op;	return *this;}
var& var::operator-=(const float op){	*this = *this - op;	return *this;}
var& var::operator-=(const double op){	*this = *this - op;	return *this;}
var& var::operator-=(const std::string& op){	*this = *this - op;	return *this;}
var& var::operator-=(const char* const op){	*this = *this - op;	return *this;}
var& var::operator-=(const Bytes& op){	*this = *this - op;	return *this;}
// 乘
var& var::operator*=(const var& op){	*this = *this * op;	return *this;}
var& var::operator*=(const bool op){	*this = *this * op;	return *this;}
var& var::operator*=(const char op){	*this = *this * op;	return *this;}
var& var::operator*=(const short op){	*this = *this * op;	return *this;}
var& var::operator*=(const int op){	*this = *this * op;	return *this;}
var& var::operator*=(const long long op){	*this = *this * op;	return *this;}
var& var::operator*=(const float op){	*this = *this * op;	return *this;}
var& var::operator*=(const double op){	*this = *this * op;	return *this;}
var& var::operator*=(const std::string& op){	*this = *this * op;	return *this;}
var& var::operator*=(const char* const op){	*this = *this * op;	return *this;}
var& var::operator*=(const Bytes& op){	*this = *this * op;	return *this;}
// 除
var& var::operator/=(const var& op){	*this = *this / op;	return *this;}
var& var::operator/=(const bool op){	*this = *this / op;	return *this;}
var& var::operator/=(const char op){	*this = *this / op;	return *this;}
var& var::operator/=(const short op){	*this = *this / op;	return *this;}
var& var::operator/=(const int op){	*this = *this / op;	return *this;}
var& var::operator/=(const long long op){	*this = *this / op;	return *this;}
var& var::operator/=(const float op){	*this = *this / op;	return *this;}
var& var::operator/=(const double op){	*this = *this / op;	return *this;}
var& var::operator/=(const std::string& op){	*this = *this / op;	return *this;}
var& var::operator/=(const char* const op){	*this = *this / op;	return *this;}
var& var::operator/=(const Bytes& op){	*this = *this / op;	return *this;}
// 取余
var& var::operator%=(const var& op){	*this = *this % op;	return *this;}
var& var::operator%=(const bool op){	*this = *this % op;	return *this;}
var& var::operator%=(const char op){	*this = *this % op;	return *this;}
var& var::operator%=(const short op){	*this = *this % op;	return *this;}
var& var::operator%=(const int op){	*this = *this % op;	return *this;}
var& var::operator%=(const long long op){	*this = *this % op;	return *this;}
var& var::operator%=(const std::string& op){	*this = *this % op;	return *this;}
var& var::operator%=(const char* const op){	*this = *this % op;	return *this;}
var& var::operator%=(const Bytes& op){	*this = *this % op;	return *this;}
/* 赋值运算重载结束 */
#endif // DYNAMIC_VAR_ENABLE_ASSIGN_OP

















void var::clear()
{
	if (this->_type == Type_t::bytes)
	{
		delete this->_data.bytes_d;
		this->_data.bytes_d = 0;
	}
	if (this->_type == Type_t::string_kind)
	{
		delete this->_data.string_d;
		this->_data.string_d = 0;
	}
	this->_type = Type_t::none;
}
bool var::pointed()
{
	if (this->_type == Type_t::bytes) return true;
	if (this->_type == Type_t::string_kind) return true;
	return false;
}