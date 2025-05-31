# Hercode Compiler 
能想出来写这个的，家里请啥都没用了吧

感觉太适合拿来当成编译作业了 

拿C语言写的，就这样 

HerCode语法：
```
Hello! Her World
function say_hello:
	say "Hello! Her World" #注释
	say "编程很美,也属于你" #注释2
end
start:
	say_hello
end
```

编译器使用：
```
./hercode_compiler.exe her.hercode hercode.exe
```
第一个参数是hercode的代码，第二个是输出文件


## 20250531更新

一个trick,可以在stdio.h的库文件下和C代码兼容

```
char a[20];
scanf("%s",a);
for(int i=0;a[i]!='\0';++i)
{
    printf("%c",a[i]);
}
printf("\n");
Hello! Her World
start:
    say "Hello! Her World"
    say "编程很美,也属于你"
end
```

在Hello! Her World之前，代码都是C代码，直接放到main函数下，注释和C语言一样用//，在这之后就得是HerCode的写法了，注释就必须得用#