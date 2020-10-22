1. TEMU工程目录介绍
本目录用于保存测试程序，并对其进行编译，包含build和src文件夹，convert.c，default.ld， Makefile文件。
目前测试程序仅支持汇编形式。

目录结构：
   +--mips_sc/        		: 测试程序目录
   |        
   |--temu/			: temu源程序目录
   |
   |--Makefile			: make脚本

2. TEMU的使用步骤

(1). 在终端进入目录”mips_sc“，输入“make”，在TEMU工程根目录下生成可加载的二进制文件“entry”。
(2). 在终端退回TEMU工程根目录，输入“make run”，编译temu指令集仿真器并启动。
(3). 如果需要重新编译和运行temu仿真器源代码，请在TEMU工程根目录下输入“make clean”，然后重复前两步。

3. 罗立伟
(1). 添加add，考虑溢出异常？
(2). subu, slt已添加
(3). mult, sll, mfhi, mflo
(4). addi(overflow?), addiu(no overflow), simm应该不要写吧？(union?), sltiu, lb, lw(地址不是4的倍数触发错误;已经读了四个字节还怎么符号扩展), offset可以是负数吗？(如果是的话，itype定义offset要改)
(5). div rs , rt 是rs/rt?
(6). J型怎么写？

