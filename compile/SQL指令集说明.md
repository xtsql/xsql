## 简介
这里的"SQL指令集"指方便执行器识别和执行所用的特殊序列。通过序列中每一行的值，执行器可以知晓当前应当对哪一个表（包括临时表，下同）的表头或数据进行何种操作。

如下午指令总览所展示的：
- op1      通常为一个 Table 指针，指向数据来源；
- op2      根据需要可能为不同的类型，这点需要执行器注意；
- op3(f)   也通常为一个 Table 指针，一般指向处理后的数据存放目标。一些指令对当前表进行操作，因此可能为空指针。

## 指令总览

以下指令中，使用 __粗体__ 标注出来的数据说明表示不保证可以正常执行的数据，需要执行器注意。

|序号|名称|说明|op1|op2|op3(f)|
|:-:|:-:|:-:|:-:|:-:|:-:|
|1|SELECT|select指令|`Table*`<br>数据来源的表对象指针|`std::vector<int64_t>*`[[1]](#dis1)<br>指示被选中的下标数组|`Table*`<br>数据输出的目标表对象指针|
|2|SELECTX|select指令，但面向存在聚集函数的情况|||未实现|
|3|WHERE|where指令|`Table*`<br>数据来源的表对象指针|`int`<br>函数序号[[2]](#dis2)|`Table*`<br>数据输出的目标表对象指针|
|4|CARTESIAN|笛卡尔积（表连接）[[3]](#dis3)|`Table*`<br>数据来源的表 1 对象指针|`Table*`<br>数据来源的表 2 对象指针|`Table*`<br>数据输出的目标表对象指针|
|5|EXEC|行内数据更改|`Table*`<br>数据来源的表对象指针|`int`<br>函数序号|-|
|6|RELEASE<span id = 'release'></span>|释放一个临时表|`Table*`<br>要被释放的表对象指针|-|-|
|7|CREATET|创建一个表|`std::string*`<br>__要创建的表名__|`cp::colum_infos_t*`<br>按顺序表示每一列的属性|`std::string*`<br>__目标数据库名称__[[5]](#dis5)|
|8|CREATED|创建一个数据库|`std::string*`<br>__要创建的数据库名称__|-|-|
|9|INSERT|向表中插入一条或多条数据|`int64_t`<br>要插入的数据数量|`std::vector<Tuple>*`<br>待插入Tuple[[4]](#dis4)|`Table*`<br>数据输出的目标表对象指针|
|10|DROPT|删除一个表|`std::string*`<br>__要删除的表名__|`std::string*`<br>__目标数据库名称__[[5]](#dis5)|-|
|11|DROPD|删除一个数据库|`std::string*`<br>__要删除的数据库名称__|-|-|
|12|DELETE|从一个表中删除数据|`std::string*`<br>__表名__|`std::string*`<br>__表所在数据库名__[[5]](#dis5)|`int`<br>函数序号|
|13|USE|切换当前活动的数据库|`std::string*`<br>__目标数据库名__|-|-|
|14|SHOWDB|显示当前保存的数据库|-|-|-|
|15|SHOWT|显示当前活动数据库内的所有表|-|-|-|
|1||||||

<span id = 'dis1'>[1] </span>
所有非 `Table*` 类型的指针，在指令执行完成后立即释放；`Table*` 指针等待 [RELEASE](#release) 指令再进行释放。


<span id = 'dis2'>[2] </span>
每行筛选用的函数入口位于exec列表中的序号。该列表是编译出sql_program_t的一部分，可以直接读写 Tuple 空间，用于运行布尔表达式判定该行是否会被选中，或修改 Tuple 的值。使用exec_t对象的run方法，并传入一个 Tuple 对象，程序将会自动运行。exec_t对象在WHERE、INSERT、UPDATE等控制命令中都有使用。在 WHERE 与 DELETE 中，使用其返回值判断该 Tuple 是否该被选中/删除；在 INSERT 与 UPDATE 中，则使用到了 Tuple 写入的功能。


<span id = 'dis3'>[3] </span>
不同行的输出顺序并无强制规定，但由于 exec_t 对 Tuple 空间的访问不使用名称，因此应当保证每一行内，左侧（下标小的列）为 Table1 的数据，右侧为 Table2 的数据。如，Table1 有5列，Table2 有 4 列，则保证原来位于 Table1 首列的数据被存放到新表首列（下标0），Table2 首列位于新表中的第6列（下标5）。


<span id = 'dis4'>[4] </span>
这里在SQL执行器发送完插入指令后，还需遍历每一 Tuple 内的所有 Ceil，根据 TYPE == CHAR 进行 free 操作


<span id = 'dis5'>[5] </span>
如果此处为一个空指针，或字符串为空串，代表使用当前活动的数据库进行操作。