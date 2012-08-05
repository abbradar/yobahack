#include "eventqueue.h"

friend bool EventQueue::Event::operator >(const Event &a, const Event &b) {
  return a.etick > b.etick;
}

void EventQueue::Push(Function &&func, unsigned int delay) {
  Event temp;
  temp.func = func;
  temp.tick = delay; 
  main_queue_.push(std::move(temp));  // А вот тут мы не копируем, а переносим, ибо нефиг
};

void EventQueue::Tick() {
  if (!main_queue_.empty()) {  // функциям %%в кармочку%% этот самый тик
    while (main_queue_.top().etick == 0) {
      // Выполняем функции, чей тик равен 0 и минусуем всем.
      main_queue_.top().func(); // Выполняем функцию...
      main_queue_.pop(); // ...и выбрасываем ее
    }

    for (auto temp_event : main_queue_)
      --temp_event->tick;
  }
};
