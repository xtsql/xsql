## 目录
[1. 目录说明](#目录说明)  
[2. 变量类型支持](#变量类型支持)  
[&emsp;2.1 已支持类型](#可支持类型)  
[&emsp;2.2 空变量](#空类型)  
[&emsp;2.3 测试中的类型](#测试中类型)  
[3. 异常](#异常)  
[4. 运算符重载](#运算符重载)  
[5. 特殊数据类型的方法](#特殊数据类型的方法)  
[6. 可配置的宏](#可配置的宏)  



## 目录说明
本目录对可变类型支持进行测试

* 版本： 1.1.0  
* 通过 var 类提供对动态数据类型的支持。

## 变量类型支持
* 所有可支持的类型定义在枚举类 ___var::Type_t___ 中
<span id = "可支持类型"></span>  
* 当前支持的类型有：
 		bool, char, short, int, long long, float, double, std::string, std::vector<unsigned char>
<span id = "空类型"></span>  
* 存在空类型，类型名称为 __var::Type_t::none__

<span id = "测试中类型"></span>  

* 此外，正在测试中（不可使用）的类型有：
	1. __自定义时间类型 var::daytime_t__  
   （一日中的时间，精确到秒），取值范围 0:0:0 - 23:59:59（使用struct） 
	2. __自定义日期类型 var::date_t__  
   （年月日信息），年份范围为 short 取值范围，月、日分别为[1,12] [1,31]
	3. __unsigned int__  
	4. __unsigned long long__
	5. __time_t__  
    时间戳类型，其存储实例为 long long 型，二者仅在使用to_string与type两种方法上有区别  
## 异常
当前存在的异常列表，带*的异常可以通过宏关闭这些异常的抛出：
|异常名|异常说明|
|:-:|-|  
|var|运算中使用了不支持的类型|
|* std::invalid_argument|传入了非法参数（如使用空指针初始化）。会导致运算结果变为空对象|
|* std::range_error	|对 array-like 使用非法下标时可能抛出|
|* var::opNone|尝试对none型变量进行操作。比如在 - * / 操作中会抛出此异常。|
|* var::ViA	|尝试对无法转换为数值型的 array-like 变量进行数值型操作。<br>如果禁用，在运算中视为none，使用类型转换则转换为0）|
|* var::USF	|使用了不支持的方法。如对数值型使用了len()方法|
|* double	|尝试对float型或double型进行取余相关运算时报出（并返回0）|

需要注意的是，除法可能会抛出除零异常。通过控制宏可以配置是抛出异常还是返回none  
## 运算符重载
当前支持重载的运算符如下：（测试中的类型并不支持）  
1.	算术运算（全部支持）  
 `+`  `-`   `*`   `/`   `%`  `++`  `--`  
1. 	关系运算（未支持）：  
    未支持的有： `==`  `!=`  `>`  `<`  `>=`  `<=`  
2. 	逻辑运算（全部支持）：   
   `&&`  `||`  `!`  
3. 	位运算（未支持）：
 `&`   `|`   `^`  `~`  `<<`  `>>`  
4. 	赋值运算（部分支持）：  
    `=`  `+=`  `-=`  `*=`  `/=`  `%=`  
    未支持的有：   `<<=`  `>>=`  `&=`  `^=`  `|=`  
		a op= b 的操作符等效于 a = a op b，例如，即使 b=1 且 op 为 + 也不执行为 a++

## 特殊数据类型的方法（暂未实现）
需要注意的是，如果采用了不支持的特殊方法，可能抛出USF异常(见异常描述章节)  
1. std::string 与 Bytes 支持特殊方法有：  
   |函数声明|说明|
   |:-:|-|
    int obj.len()|	返回字符串/二进制串长度（根据类型决定）
    obj[int pos]|	返回pos所对应的字符下标（可为只读或读写）。如果下标越界，由对应STL类处理异常
    var obj.sub(int start = 0,int end = 0, int step = 1)|返回obj下标在 [start,end) 之间的子串，并且相邻两个字符坐标差为step。<br>关于坐标：正数表示从字符串头部开始，负数表示从字符串尾部开始。为0时表示包括头、尾部端点字符（即规则与python相同）<br>如果 end - start 与 step 异号（不包括坐标均为0），或下标超限、step为0，则可抛出 std::range_error 并返回none
1. std::string 独占的方法：  
   |函数声明|说明|
   |:-:|-|
    int obj.find(const string& str, const int start = 0)|从start开始（包括）寻找obj是否包括str子串，是则返回起始下标，否返回-1
1. var::time_t 独占的方法：  
   |函数声明|说明|
   |:-:|-|
    char obj.hour()<br>obj.minute()<br> obj.second()<br>|获取只读的时分秒
    void obj.set_hour(char x)<br>obj.set_minute(char x)<br>obj.set_second(char x)|写入时分秒，会进行类型校验

## 可配置的宏
|宏名称|宏说明|
|:-:|-|
DYNAMIC_VAR_ENABLE_ARITH_OP|控制是否支持算术运算符的重载（含对应赋值运算），注释掉以关闭此功能
DYNAMIC_VAR_ENABLE_ALL_ARITH_OP|在定义了 DYNAMIC_VAR_ENABLE_ARITH_OP 下：<br>不启用此宏时将仅重载含 var 对象的运算<br>启用此宏则重载【数值型】与【字符串、字节数组型】运算以产生 var 对象，注释掉以关闭此功能<br>本特性暂不支持
DYNAMIC_VAR_ENABLE_LOGIC_OP|控制是否支持逻辑运算符的重载（含对应赋值运算），注释掉以关闭此功能
DYNAMIC_VAR_ENABLE_BIT_OP|控制是否支持位运算符的重载（含对应赋值运算），注释掉以关闭此功能
DYNAMIC_VAR_ENABLE_ASSIGN_OP|控制是否支持赋值运算符的重载（不含 = ），注释掉以关闭此功能
DYNAMIC_VAR_MUTE_INVALID_ARG|取消注释以关闭 std::invalid_argument型 异常的抛出
DYNAMIC_VAR_MUTE_OPNONE|取消注释以关闭 opNone型 异常的抛出
DYNAMIC_VAR_MUTE_VIA|取消注释以关闭 ViA型 异常的抛出
DYNAMIC_VAR_MUTE_DOUBLE|取消注释以以关闭 浮点%浮点 时 double型 异常的抛出
DYNAMIC_VAR_MUTE_INT_DIV_ZERO|取消注释以关闭 整型除零 异常的抛出，改为返回 none 变量
DYNAMIC_VAR_NAN2NONE|取消注释以将 0/0 由 nan 改为返回 none 变量
DYNAMIC_VAR_INF2NONE|取消注释以将 x/0 由 inf 改为返回 none 变量