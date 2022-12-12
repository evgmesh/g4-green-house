/*
 * Counter.h
 *
 *  Created on: Sep 1, 2022
 *      Author: tylen
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include "chip.h"

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
  CounterDataType _step;
};


template <class CounterDataType>
void
Counter<CounterDataType>::inc ()
{
  if ((init + _step) <= up_lim)
    {
      init += _step;
    }
}

template <class CounterDataType>
void
Counter<CounterDataType>::dec ()
{
  if (init - _step >= down_lim)
    {
      init -= _step;
    }
}

template <class CounterDataType>
CounterDataType
Counter<CounterDataType>::getCurrent ()
{
  return this->init;
}

template <class CounterDataType>
Counter<CounterDataType>::Counter (CounterDataType lower_boundary,
                                   CounterDataType upper_boundary,
                                   CounterDataType step)
{
  up_lim = upper_boundary;
  down_lim = lower_boundary;
  _step = step;
  if (down_lim > up_lim)
    {
      init = up_lim;
    }
  else if (down_lim < 0)
    {
      init = 0;
    }
  else
    {
      init = down_lim;
    }
}

template <class CounterDataType>
void
Counter<CounterDataType>::setCurrent (CounterDataType num)
{
  if (num > up_lim)
    {
      init = up_lim;
    }
  else if (num < down_lim)
    {
      init = down_lim;
    }
  else
    {
      init = num;
    }
}



#endif /* COUNTER_H_ */
