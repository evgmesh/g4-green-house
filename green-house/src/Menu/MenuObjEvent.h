/*
 * MenuObjEvent.h
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#ifndef MENU_MENUOBJEVENT_H_
#define MENU_MENUOBJEVENT_H_

class MenuObjEvent
{
public:
  virtual ~MenuObjEvent (){};

  enum eventType
  {
    /** Start of the event */
    eFocus,
    /** End of the event*/
    eUnFocus,
    /** Button toggle event type */
    eClick,
    /** Rotary clockwise */
    eRollClockWise,
    /** Rotary counter clockwise */
    eRollCClockWise,
  };
  MenuObjEvent (eventType e = eFocus) : type (e){};
  eventType type;
};



#endif /* MENU_MENUOBJEVENT_H_ */
