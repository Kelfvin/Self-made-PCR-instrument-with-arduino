/*程序开始后按照提示输入：
  变性的温度下限tp1min，温度上限tp1max，时间time1。
  退火的温度下限tp2min，温度上限tp2max，时间time2。
  延伸的温度下限tp3min，温度上限tp3max，时间time3。
（以上数据温度单位为℃,时间单位为s）

循环次数times。*/
#include<Keypad.h> // 键盘度库函数
#include<math.h> // pow要用
#include<DallasTemperature.h> // 温度库函数
#include<OneWire.h> // 温度库函数
#include<Adafruit_GFX.h> // 显示器库函数
#include<Adafruit_SSD1306.h> // 显示器库函数

int time1, time2, time3; // 变性，退火，延伸所需要的时间 
unsigned long timestand; // 储存程序运行的时间（作为标准） 

int tp1min, tp1max; // 变性温度限制 
int tp2min, tp2max; // 退火温度限制 
int tp3min, tp3max; // 延伸温度限制
float tp; // 储存实时温度

int cycle_index; // 程序设定的循环次数
int cycle = 1; // 储存程序已经运行的次数

int steptemp;
int step;

char key; // 字符中转 
int temp; // 计算位数（栈顶）
char a[10]; // 储存字符
int num; // 储存数值

const int  tp_pin = A3; // 定义3号口为温度传感器的数据口
const int  OLED_pin = 4; // 定义4号口为OLED屏的数据输出口
Adafruit_SSD1306 display(OLED_pin);

const int sign_warm = 10; // 定义10号口为继电器（加热）
const int  sign_cold = 11; // 定义11号口为继电器（制冷片）
const int sign_coldfan= 12; // 定义13号口继电器（制冷片散热风扇）
const int sign_fan = 13; // 定义12号为继电器（风扇）

OneWire oneWire(tp_pin); // （温度）声明
DallasTemperature sensors(&oneWire); // （温度）声明

const byte ROWS = 4; // 建立数组
const byte COLS = 4;
char keys[ROWS][COLS] =
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};
byte rowPins[ROWS] = { 2,3,4,5 }; // 引脚号码
byte colPins[COLS] = { 6,7,8,9 }; // 引脚号码

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // 初始化键盘

void UpdateDisplay()
{
  display.clearDisplay(); // 清空缓存
  display.setCursor(0, 0); // 设置光标

  switch (step)
  {
  case 1:
  {
    display.println("Warm");

    if (steptemp == 3)
    {
      display.println("before Denaturing");
      display.print("tp1min:");
      display.print(tp1min);
      display.println(" C");
    }

    if (steptemp == 4)
    {
      display.println("before Annealing");
      display.print("tp2min:");
      display.print(tp2min);
      display.println(" C");
    }

    if (steptemp == 5)
    {
      display.println("before Extending");
      display.print("tp3min:");
      display.print(tp3min);
      display.println(" C");
    }
  }
  break;

  case 2:
  {
    display.println("Cold");

    if (steptemp == 3)
    {
      display.println("before Denaturing");
      display.print("tp1max:");
      display.print(tp1max);
      display.println(" C");
    }

    if (steptemp == 4)
    {
      display.println("before Annealing");
      display.print("tp2max:");
      display.print(tp2max);
      display.println(" C");
    }

    if (steptemp == 5)
    {
      display.println("before Extending");
      display.print("tp3max:");
      display.print(tp3max);
      display.println(" C");
    }
  }
  break;


  case 3:
  {
    display.println("Denaturing");
    display.print("tp1min:");
    display.print(tp1min);
    display.println(" C");
    display.print("tp1max:");
    display.print(tp1max);
    display.println(" C");
    display.print("time1:");
    display.print(time1);
    display.println(" s");
  }
  break;


  case 4:
  {
    display.println("Annealing");
    display.print("tp2min:");
    display.print(tp2min);
    display.println(" C");
    display.print("tp2max:");
    display.print(tp2max);
    display.println(" C");
    display.print("time2:");
    display.print(time2);
    display.println(" s");
  }
  break;

  case 5:
  {
    display.println("Extending");
    display.print("tp3min:");
    display.print(tp3min);
    display.println(" C");
    display.print("tp3max:");
    display.print(tp3max);
    display.println(" C");
    display.print("time3:");
    display.print(time3);
    display.println(" s");
  }
  break;

  case 6:
  {
    display.println("Done!");
    display.print("Work time:");
    display.print((millis() - timestand) / 1000);
    display.println(" s");
    display.print("Temperature:");
    display.print(tp);
    display.println(" C");
    display.display();

    return;
  }
  break;
  }

  display.print("Cycle_index:");
  display.println(cycle_index);
  display.print("Work time:");
  display.print((millis() - timestand) / 1000);
  display.println(" s");
  display.print("Temperature:");
  display.print(tp);
  display.println(" C");

  if (cycle <= cycle_index)
  {
    display.print("Cycle:");
    display.println(cycle);
  }

  display.display(); // 将缓存显示到屏幕上

  return;
}

void Keep(int tpmin, int tpmax, int time)
{

  timestand = millis(); // 储存当前的运行时间
  while (tp <= tpmin) // 当温度低于要求的最低温度
  {
    step = 1; // 当前步骤为第一步（升温）
    UpdateDisplay(); // 显示相关

    digitalWrite(sign_warm, HIGH); // 开始加热

    sensors.requestTemperatures();  // 请求温度
    tp = sensors.getTempCByIndex(0);  // 储存温度

    if (tp >= tpmin)
      break;
  }

  timestand = millis(); // 储存当前的运行时间
  while (tp > tpmax) // 当温度大于要求的最高温度
  {
    step = 2; // 当前步骤为第二步（降温）
    UpdateDisplay(); // 显示相关

    digitalWrite(sign_warm, LOW); // 停止加热
    digitalWrite(sign_cold, HIGH); // 开始制冷

    sensors.requestTemperatures();  // 请求温度
    tp = sensors.getTempCByIndex(0);  // 储存温度
  }
  digitalWrite(sign_cold, LOW); // 关闭制冷

  timestand = millis(); // 储存当前运行时间
  while ((millis() - timestand) / 1000 < time) // 如果时间还没到进行操作
  {
    sensors.requestTemperatures(); // 请求温度
    tp = sensors.getTempCByIndex(0); // 获取温度

    step = steptemp;
    UpdateDisplay();  // 显示相关

    if (tp < tpmin) // 温度低于要求温度开始加热
      digitalWrite(sign_warm, HIGH);

    else if (tp >= tpmin && tp <= tpmax) // 若温度处于合适的温度不操作
      continue;

    else // 若温度高于要求温度停止加热
      digitalWrite(sign_warm, LOW);
  }

  return;
}



void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 初始化屏幕地址
  display.setTextColor(WHITE);

  pinMode(sign_warm, OUTPUT); // 定义继电器（加热）为输出口
  digitalWrite(sign_warm, LOW); // 初始关闭加热

  pinMode(sign_cold, OUTPUT); // 定义继电器（制冷片）为输出口
  digitalWrite(sign_cold, LOW); // 初始关闭制冷片

  pinMode(sign_fan, OUTPUT); // 定义继电器（风扇）为输出口
  digitalWrite(sign_fan, HIGH); // 初始开启风扇

  pinMode(sign_coldfan, OUTPUT); // 定义13号口继电器（制冷片散热风扇）为输出口
  digitalWrite(sign_coldfan, HIGH);  // 初始开启风扇
}

void inputupdatedisplay(int i)
{
  display.clearDisplay(); // 清空屏幕;
  display.setCursor(0, 0); // 设置光标
  display.print("Input ");

  switch (i) // 判断显示出当前输入的是哪个数据
  {
  case 1:display.println("tp1min:");
    break;

  case 2:display.println("tp1max:");
    break;

  case 3:display.println("time1:");
    break;

  case 4:display.println("tp2min:");
    break;

  case 5:display.println("tp2max:");
    break;

  case 6:display.println("time2:");
    break;

  case 7:display.println("tp3min:");
    break;

  case 8:display.println("tp3max:");
    break;

  case 9:display.println("time3:");
    break;

  case 10:display.println("Cycle_index:");
    break;
  }
  display.display(); // 将缓存打印到屏幕
}

void loop()
{

  if (cycle == 1) // 第一次循环时输入数据
  {
    display.display(); // 初始化

    for (int i = 1;i <= 10;i++) // 总共需要输入10个变量的值
    {
      key = '0'; // 还原中转站
      temp = 0; // 初始化记位器

      inputupdatedisplay(i); // 屏幕打印相关

      while (key != "B")
      {
        key = customKeypad.getKey(); // 输入一个字符

        if (key != NO_KEY && key != 'B') // 如果不是无输入和确定键B
        {
          if (key == 'A') // 若输入了删除键A
          {
            if (temp != 0) // 防止栈空
            {
              temp--; // 按下A（删除键）将上一个数据出栈

              inputupdatedisplay(i);
              for (int w = 1;w <= temp;w++)
                display.print(a[w]);
              display.display();
            }
          }

          else if (key == 'C' || key == 'D' || key == '*' || key == '#') // 按这些键无效
            continue;

          else
          {
            if (temp != 9) // 防止栈溢出
            {
              a[++temp] = key; // 将字符储存起来

              inputupdatedisplay(i);
              for (int w = 1;w <= temp;w++)
                display.print(a[w]);
              display.display();
            }
          }

        }

        else if (key == 'B') // 若输入确定B则跳出输入
          break;

        else continue; // 若无输入则进入下一此循环
      }

      num = 0; // 初始化数值储存器

      for (int j = 1;j <= temp;j++) // 将储存的一个个字符转换成数值
      {
        num += pow(10, temp - j) * (a[j] - '0'); // 因为a[]为char型所以转换成对于的int'0'
      }

      switch (i) // 将数值赋给对应的变量
      {
      case 1:tp1min = num;
        break;

      case 2:tp1max = num;
        break;

      case 3:time1 = num;
        break;

      case 4:tp2min = num;
        break;

      case 5:tp2max = num;
        break;

      case 6:time2 = num;
        break;

      case 7:tp3min = num;
        break;

      case 8:tp3max = num;
        break;

      case 9:time3 = num;
        break;

      case 10:cycle_index = num;
        break;
      }
    }
  }

  if (cycle <= cycle_index)
  {
    /*************************变性***************************/
    steptemp = 3; // 储存当前步骤
    sensors.requestTemperatures(); // 请求温度
    tp = sensors.getTempCByIndex(0); // 获取温度
    Keep(tp1min, tp1max, time1); // 进行温度操作

    /*************************退火***************************/
    steptemp = 4; // 储存当前步骤
    Keep(tp2min, tp2max, time2); // 进行温度操作

    /**************************退火**************************/
    steptemp = 5; // 储存当前步骤
    Keep(tp3min, tp3max, time3); // 进行温度操作

    cycle++; // 循环次数加一

    timestand = millis(); // 储存当前的运行时间    
  }

  else
  {
    digitalWrite(sign_warm, LOW); // 停止加热
    digitalWrite(sign_fan, LOW); // 关闭风扇
    digitalWrite(sign_coldfan, LOW); // 关闭制冷片的风扇

    sensors.requestTemperatures(); // 请求温度
    tp = sensors.getTempCByIndex(0); // 获取温度

    step = 6;
    UpdateDisplay();
  }

}
