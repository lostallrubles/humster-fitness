//Подключаем библиотеку ЖК-дисплея 
#include <LiquidCrystal.h>

//Датчик присоединен ко выводу № 2
#define pinSensor 2

/*
 Переменная счетчик срабатываний датчика. целое число без знака (больше или равно нулю)
 ключевое слово volatile нужно потому что значение переменной будет меняться в обработчике прерывания
*/
volatile unsigned long counter;

//Переменная в которй будем хранить значение времени последнего срабатывания датчика (защита от дребезга контактов)
volatile unsigned long lastTick = millis();

//константы для перевода миллисекунд в дни, часы минуты и секунды
const unsigned long msec_in_sec = 1000; // в секунде тысяча миллисекунд
const unsigned long msec_in_min  = 60 * msec_in_sec;
const unsigned long msec_in_hour = msec_in_min * 60;
const unsigned long msec_in_day  = msec_in_hour * 24;

//Две строки длиной 16 символов для вывода информации на дисплей
char row_1[16]; 
char row_2[16]; 

//номера ножек ардуино, используемых для подключения ЖК дисплея
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;

//создаем экземпляр для работы с дисплеем
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Эта функция вызывается один раз при подаче питания или после сброса контроллера
void setup() {
  //Сбрасываем значение счетчика оборотов в ноль
  counter = 0;
  
  //Настраиваем порт датчика на режим чтения с подтяжкой уровня к питанию. 
  pinMode(pinSensor, INPUT_PULLUP);
  
  //Настраиваем нулевое прерывание на нисходящий фронт сигнала от датчика.
  //Всего в контроллере есть два прерывания. Нулевое на ножке № 2, первое на ножке № 3.
  attachInterrupt(0, interruptFunction, FALLING);
  
  //Инициализируем дисплей
  lcd.begin(16, 2);
  //Ставим курсор в начало первой строки
  lcd.setCursor(0, 0);  
  //Выводим приветствие
  lcd.print("hello, world!");
  //Ставим курсор на начало второй строки
  lcd.setCursor(0, 1);     
  //выводим сообщение "хомячковый фитнес"
  lcd.print("humster fitnes");
  //Ждем одну секунду
  delay(1000);
  //Очищаем экран жисплея
  lcd.clear();
  
  //Включаем последовательный порт и настраиваем скорость порта в 115200 бит/сек
  Serial.begin(115200);
}

//Эта встроенная функция вызывается все время пока есть питание на процессоре
void loop() {
  //Формируем строку с текущим значением счетчика оборотов колеса
  sprintf(row_1,"rounds: %04d", counter);
  //Встаем в начало первой строки 
  lcd.setCursor(0, 0);     
  //Выводим строку на дисплей
  lcd.print(row_1);

  //Повторяем вывод строки в последовательный порт (если нет дисплея, то наблюдать за работой программы можно в мониторе порта <Ctrl>+<Shift>+<M> )
  Serial.println(row_1);
  
  //расчитываем время работы программы
  uptime();
  //встаем в начало второй строки
  lcd.setCursor(0, 1);     
  //выводим время работы программы
  lcd.print(row_2);
  
  //Повторяем вывод строки в последовательный порт (если нет дисплея, то наблюдать за работой программы можно в мониторе порта <Ctrl>+<Shift>+<M> )
  Serial.println(row_2);
  
  //делаем задержку в 300 миллисекунд 
  delay(300);
}

/*
 * Так как мы настроили аппаратное прерывание (attachInterrupt() )на вторую ножку процессора,
   то эта функция вызывается всякий раз, когда датчик, подключенный к ножке №2 замыкается на землю (сигнал с высокого уровня +5В переходит в низкий уровень 0В)
 */
void interruptFunction(){
  /* Так как контакты имеют дребезг, а хомячок не очень стремителен, 
     то считаем, что он крутит барабан не быстрее трех оборотов в секунду (300 миллисекунд на оборот).
     если это случается быстрее, то значит это не хомячок, а дребезг контактов. 
  */
  unsigned long currentTime = millis(); //сколько миллисекунд прошло со времени старта программы
  if ((currentTime - lastTick) > 300) {
    counter = counter + 1;   //Увеличиваем значение счетчика оборотов на единицу
    lastTick = currentTime;  // Запоминаем когда последний раз мы реагировали на датчик
  }
}

//Подсчет времени работы программы
void uptime(){
  //полных секунд с момента старта
  unsigned long mseconds = millis();
  int days = 0;
  int hours = 0;
  int mins = 0;
  int sec = 0;
  //прошло дней с момента запуска
  days = mseconds / msec_in_day;
  mseconds -= (days * msec_in_day);
  //прошло часов с момента запуска
  hours = mseconds / msec_in_hour;
  mseconds -= (hours * msec_in_hour);
  //прошло минут с момента запуска
  mins = mseconds / msec_in_min;
  mseconds -= (mins * msec_in_min);
  //прошло секунд с момента запуска
  sec = mseconds / msec_in_sec;
  sprintf(row_2,"uptime: %02d %02d %02d", hours, mins, sec );
}
