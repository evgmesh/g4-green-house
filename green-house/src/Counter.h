/*
 * Counter.h
 *
 *  Created on: Sep 1, 2022
 *      Author: tylen
 */

#ifndef COUNTER_H_
#define COUNTER_H_

template <class CounterDataType> class Counter
{

public:
  Counter (CounterDataType lower_boundary, CounterDataType upper_boundary,
           CounterDataType step);
  void inc ();
  void dec ();
  CounterDataType getCurrent ();
  void setCurrent (CounterDataType num);
  ~Counter () = default;

private:
  CounterDataType init;
  CounterDataType up_lim;
  CounterDataType down_lim;
  CounterDataType step;
};
#endif /* COUNTER_H_ */
