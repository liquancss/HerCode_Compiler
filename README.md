# Hercode Compiler 
能想出来写这个的，家里请啥都没用了吧

感觉太适合拿来当成编译作业了 

拿C语言写的，就这样 

HerCode语法：
```
Shello! Her World
function say_shello:
	say "shello! Her World" #注释
	say "编程很美,也属于你" #注释2
end
start:
	say_shello
end
```

编译器使用：
```
./hercode_compiler.exe her.hercode hercode.exe
```
第一个参数是hercode的代码，第二个是输出文件


## 20250531更新

一个trick,可以和C代码兼容，我暴力的把所有标准库头文件都给扔到输出的C文件上

```
time_t rawtime;
struct tm *info;
#define BST (+1)
#define CCT (+8)
time(&rawtime);
/* 获取 GMT 时间 */
info = gmtime(&rawtime );

printf("当前的世界时钟：\n");
printf("伦敦：%2d:%02d\n", (info->tm_hour+BST)%24, info->tm_min);
printf("中国：%2d:%02d\n", (info->tm_hour+CCT)%24, info->tm_min);

Hello! Her World
#「你可以做到(you_can_do_this)
#这是一段“鼓励式函数”：
#-功能：向终端打印两句话，欢迎女孩们来到专属的编程世界
#－关键字说明：
#function 定义函数（像在写日记那样自然）
#say 输出一句温暖的话
# end 结束函数或代码块
function you_can_do_this:
	say "Shello Her World"
	say "看啥？Python不比这好用？"
end
start:
	you_can_do_this
end
```
在Shello! Her World之前，代码都是C代码，直接放到main函数下，注释和C语言一样用//，在这之后就得是HerCode的写法了，注释就必须得用#
