/*
 * MenuObj.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#include <Menu/MenuObj.h>

MenuObj::MenuObj (LiquidCrystal *lcd)
{
  _lcd = lcd;
  current = &MenuObj::ObjInit;
  HandleObj (MenuObjEvent (MenuObjEvent::eFocus));
}

MenuObj::~MenuObj ()
{
  // TODO Auto-generated destructor stub
}

void
MenuObj::HandleObj (const MenuObjEvent &event)
{
  (this->current) (event);
}

void
MenuObj::SetEvent (obj_pointer newevent)
{
  (this->*current) (MenuObjEvent (MenuObjEvent::eUnFocus));
  current = newevent;
  (this->*current) (MenuObjEvent (MenuObjEvent::eFocus));
}

void
MenuObj::ObjSetCOLevel (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      lcd_line_1 = "[SET CO2 LVL]";
      break;
    case MenuObjEvent::eUnFocus:
      lcd_line_1 = " SET CO2 LVL ";
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjSetPPM);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjSensors);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjSensors);
      break;
    default:
      break;
    }
}

void
MenuObj::ObjSetPPM (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      lcd_line_1 = " SET [%4d] PPM ";
      lcd_line_2 = " BACK     SAVE ";
      break;
    case MenuObjEvent::eUnFocus:
      lcd_line_1 = " SET  %4d  PPM ";
      break;
    case MenuObjEvent::eClick:
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjBack);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjSave);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjBack (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      lcd_line_2 = "[BACK]    SAVE ";
      break;
    case MenuObjEvent::eUnFocus:
      lcd_line_2 = " BACK     SAVE ";
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjSetCOLevel);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjSave);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjSetPPM);
      break;
    default:
      break;
    }
}

void
MenuObj::ObjSave (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      lcd_line_2 = " BACK    [SAVE]";
      break;
    case MenuObjEvent::eUnFocus:
      lcd_line_2 = " BACK     SAVE ";
      break;
    case MenuObjEvent::eClick:
	  // TODO Save to EEPROM
      SetEvent (&MenuObj::ObjSetCOLevel);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjSetPPM);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjBack);
      break;
    default:
      break;
    }
}

void
MenuObj::ObjSensors (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      break;
    case MenuObjEvent::eUnFocus:
      /* code */
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::) break;
    case MenuObjEvent::eRollClockWise:

      break;
    case MenuObjEvent::eRollCClockWise:

      break;

    default:
      break;
    }
}
