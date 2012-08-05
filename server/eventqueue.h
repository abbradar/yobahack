#ifndef YOBAHACK_SERVER_EVENTQUEUE_H_
#define YOBAHACK_SERVER_EVENTQUEUE_H_

#include <functional>
#include <boost/heap/priority_queue.hpp>

/** Класс очереди событий */
class EventQueue {
 public:
  typedef std::function<void()> Function;

  /** Тикаем один шаг и выполняем функцию, если надо */
  void Tick();
 
  /** Записываем функцию на выполнение
   * \param func Функция на выполнение
   * \param delay Количество шагов
   */
  void Push(Function &&func, unsigned int delay);

 private:
  /** Класс объектов, которыми управляет класс очереди */
  struct Event {
    friend bool operator >(const Event &a, const Event &b);
    
    Function func; ///< Функция, которую надо выполнить
    unsigned int tick; ///< Кол-во шагок, через которое ее надо выполнить
  };

  typedef boost::heap::priority_queue<Event, boost::heap::compare<std::greater<Event>>> Queue; 

  Queue main_queue_;
};

#endif // YOBAHACK_SERVER_EVENTQUEUE_H_
