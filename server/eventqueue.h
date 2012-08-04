#include <functional>
#include <boost/heap/priority_queue.hpp>

typedef std::function<void()> FunctionType;

class EventQueue{  //Наш класс очереди
  struct Event{          //Класс объектов, которыми управляет класс очереди, состоит
    FunctionType efunc;  //из функции, которую надо выполнить и количества шагов,
    unsigned int etick;  //через которое ее надо выполнить
    friend bool operator>(const Event a,const Event b){  //Для boost::heap::priority_queue
      if (a.etick>b.etick) {return true;}
      return false;
    }
  };
  //Дальше идет особая магия. Создаем бустовскую очередь с типом Event, подсовываем
  //ему в качестве оператора сравнения >, который мы определили выше.
  typedef boost::heap::priority_queue<Event,boost::heap::compare<std::greater<Event>>> QueueType; 
  QueueType main_queue;
public:
  EventQueue();
  void tick();  //Тикаем один шаг и выкидываем функции на выполнение, если надо.
  void store_function(FunctionType,int);  //Записываем функцию на выполнение
};                                        //через int ticks шагов.

EventQueue::EventQueue(){  //Ну тут все понятно, просто конструктор
};

void EventQueue::store_function(FunctionType func_to_store,int ticks){
  Event temp;
  temp.efunc=func_to_store;
  temp.etick=ticks;
  main_queue.push(std::move(temp));  //А вот тут мы не копируем, а переносим
                                     //ибо нефиг
};

void EventQueue::tick(){     //Выполняем функции, чей тик равен 0 и минусуем всем
  if (!main_queue.empty()){  //функциям %%в кармочку%% этот самый тик
    while (main_queue.top().etick==0){
      main_queue.top().efunc();            //Выполняем функцию...
      main_queue.pop();                    //...и выбрасываем ее
    }
    for (auto temp_event:main_queue)
      --temp_event.etick;
  }
};
