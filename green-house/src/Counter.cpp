#include "Counter.h"

template <class CounterDataType>
void
Counter<CounterDataType>::inc ()
{
  if (init + step <= up_lim)
    {
      init += step;
    }
}

template <class CounterDataType>
void
Counter<CounterDataType>::dec ()
{
  if (init - step >= down_lim)
    {
      init -= step;
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
  step = step;
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
