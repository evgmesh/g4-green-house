/*
 * MenuObj.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#include "MenuObj.h"

const char *MENU_OBJ_LINES[]
    = { "[SET CO2 LVL]",   " SET CO2 LVL ",  "SET [%4d] PPM",
        "SET  %4d  PPM",   "SET <%4d> PPM",  " BACK     SAVE ",
        "[BACK]    SAVE ", " BACK    [SAVE]" };


MenuObj::MenuObj (LiquidCrystal *lcd, Counter<uint16_t> *ppm)
{
  _lcd = lcd;
  _ppm = ppm;
  current = &MenuObj::ObjSetCOLevel;
  HandleObj (MenuObjEvent (MenuObjEvent::eFocus));
}

MenuObj::~MenuObj ()
{
  // TODO Auto-generated destructor stub
}

void
MenuObj::HandleObj (const MenuObjEvent &event)
{
  (this->*current) (event);
}

void
MenuObj::SetEvent (obj_pointer newevent)
{
  (this->*current) (MenuObjEvent (MenuObjEvent::eUnFocus));
  current = newevent;
  (this->*current) (MenuObjEvent (MenuObjEvent::eFocus));
}

inline void
MenuObj::SetLineToConst (uint8_t line, const char *to)
{
  memcpy (lcd_line[line - 1], to, 16);
}

void
MenuObj::SetLineToFMT (uint8_t line, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vsnprintf (lcd_line[line - 1], 16, fmt, args);
  va_end (args);
}

void
MenuObj::ObjSetCOLevel (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[CO2_FOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (1, MENU_OBJ_LINES[CO2_UNFOCUS]);
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
      SetLineToFMT (1, MENU_OBJ_LINES[SET_PPM_FOCUS], _ppm->getCurrent ());
      SetLineToConst (2, MENU_OBJ_LINES[BACK_UNFOCUS_SAVE_UNFOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[SET_PPM_UNFOCUS], _ppm->getCurrent ());
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
MenuObj::ObjChangePPMValue (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CHANGE_PPM], _ppm->getCurrent ());
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[SET_PPM_FOCUS], _ppm->getCurrent ());
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjSetPPM);
      break;
    case MenuObjEvent::eRollClockWise:
      _ppm->inc ();
      SetLineToFMT (1, MENU_OBJ_LINES[CHANGE_PPM], _ppm->getCurrent ());
      break;
    case MenuObjEvent::eRollCClockWise:
      _ppm->dec ();
      SetLineToFMT (1, MENU_OBJ_LINES[CHANGE_PPM], _ppm->getCurrent ());
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
      SetLineToConst (2, MENU_OBJ_LINES[BACK_FOCUS_SAVE_UNFOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (2, MENU_OBJ_LINES[BACK_UNFOCUS_SAVE_UNFOCUS]);
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
      SetLineToConst (2, MENU_OBJ_LINES[BACK_UNFOCUS_SAVE_FOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (2, MENU_OBJ_LINES[BACK_UNFOCUS_SAVE_UNFOCUS]);
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
      break;
    case MenuObjEvent::eRollClockWise:

      break;
    case MenuObjEvent::eRollCClockWise:

      break;

    default:
      break;
    }
}
