/*
 * MenuObj.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#include "MenuObj.h"

const char *MENU_OBJ_LINES[] = {
  "[SET CO2 LVL]",   " SET CO2 LVL ",   "[SHOW VALUES]",  " SHOW VALUES ",
  "[SHOW ALL]",      " SHOW ALL",       "[HARD RESET]",   " HARD RESET ",
  "SET [%4d] PPM",   "SET  %4d  PPM",   "SET <%4d> PPM",  " BACK     SAVE ",
  "[BACK]    SAVE ", " BACK    [SAVE]", "  CO2:%4d PPM ", "> CO2:%4d PPM",
  "  RH:%2d% ",      "> RH:%2d%",       "  TEMP: %2d ",   "> TEMP: %d",
  "  SP:%4d PPM ",   "> SP:%4d PPM",    "  VALVE: %s ",   "> VALVE: %s",
  " BACK TO MENU ",  "[BACK TO MENU]",  "CO2:%4d S:%4d",  "H:%2d T:%2d V:%s"
};

MenuObj::MenuObj (LiquidCrystal *lcd, Counter<uint16_t> *ppm,
                  EEPROM_Wrapper *eeprom)
{
  _eeprom = eeprom;
  _lcd = lcd;
  _ppm = ppm;
  current = &MenuObj::ObjSetCOLevel;
  readSetPointFromEEPROM ();
  HandleObj (MenuObjEvent (MenuObjEvent::eFocus));
}

MenuObj::~MenuObj ()
{
  // TODO Auto-generated destructor stub
}

void
MenuObj::readSetPointFromEEPROM (void)
{
  uint16_t *data
      = (uint16_t *)_eeprom->read_from (EEPROM_ADDRESS, sizeof (uint16_t));
  if ((*data) > 150 && (*data) < 1024)
    {
      _ppm->setCurrent (*data);
    }
}

void
MenuObj::eraseSetPointFromEEPROM (void)
{
  _ppm->setCurrent (0);
  saveSetPointToEEPROM ();
}
void
MenuObj::saveSetPointToEEPROM (void)
{
  uint16_t data = _ppm->getCurrent ();
  _eeprom->write_to (EEPROM_ADDRESS, &data, sizeof (uint16_t));
}

void
MenuObj::HandleRotaryAction (RotaryAction action)
{
  switch (action)
    {
    case ROTARY_ACTION::ROTARY_CLOCKWISE:
      HandleObj (MenuObjEvent (MenuObjEvent::eRollClockWise));
      break;
    case ROTARY_ACTION::ROTARY_CCLOCKWISE:
      HandleObj (MenuObjEvent (MenuObjEvent::eRollCClockWise));
      break;
    case ROTARY_ACTION::ROTARY_PRESS:
      HandleObj (MenuObjEvent (MenuObjEvent::eClick));
      break;
    default:
      break;
    }
}

void
MenuObj::HandleObj (const MenuObjEvent &event)
{
  (this->*current) (event);
  updateLCD ();
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
MenuObj::updateLCD (void)
{
  _lcd->clear ();
  _lcd->setCursor (0, 0);
  _lcd->print (lcd_line[0]);
  _lcd->setCursor (0, 1);
  _lcd->print (lcd_line[1]);
}

void
MenuObj::ObjSetCOLevel (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[CO2_FOCUS]);
      SetLineToConst (2, MENU_OBJ_LINES[SHOW_VALUES_UNFOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (1, MENU_OBJ_LINES[CO2_UNFOCUS]);
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjSetPPM);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjShowValues);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjReset);
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
      SetEvent (&MenuObj::ObjChangePPMValue);
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
      saveSetPointToEEPROM();
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
MenuObj::ObjShowValues (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[CO2_UNFOCUS]);
      SetLineToConst (2, MENU_OBJ_LINES[SHOW_VALUES_FOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (2, MENU_OBJ_LINES[SHOW_VALUES_UNFOCUS]);
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjValuesCO);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjShowValuesMax);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjSetCOLevel);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjShowValuesMax (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[SHOW_ALL_FOCUS]);
      SetLineToConst (2, MENU_OBJ_LINES[RESET_UNFOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (2, MENU_OBJ_LINES[SHOW_ALL_UNFOCUS]);
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjShowALL);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjReset);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjShowValues);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjShowALL (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[ALL_L_1_4d_4d], 500,
                    _ppm->getCurrent ());
      SetLineToFMT (2, MENU_OBJ_LINES[ALL_L_2_2d_2d_s], 99, 22, "OFF");
      break;
    case MenuObjEvent::eUnFocus:
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjSetCOLevel);
      break;
    case MenuObjEvent::eRollClockWise:
      break;
    case MenuObjEvent::eRollCClockWise:
      break;

    default:
      break;
    }
}

void
MenuObj::ObjReset (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[SHOW_ALL_UNFOCUS]);
      SetLineToConst (2, MENU_OBJ_LINES[RESET_FOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (2, MENU_OBJ_LINES[SHOW_ALL_UNFOCUS]);
      break;
    case MenuObjEvent::eClick:
      eraseSetPointFromEEPROM();
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjSetCOLevel);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjShowValuesMax);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjValuesCO (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_CO2_FOCUS], 500);
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_HUM_UNFOCUS], 99);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_CO2_UNFOCUS], 500);
      break;
    case MenuObjEvent::eClick:
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjValuesRH);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjValuesBackToMenu);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjValuesRH (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_CO2_UNFOCUS], 500);
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_HUM_FOCUS], 99);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_HUM_UNFOCUS], 99);
      break;
    case MenuObjEvent::eClick:
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjValuesTEMP);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjValuesCO);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjValuesTEMP (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_TEMP_FOCUS], 99);
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_SP_UNFOCUS], _ppm->getCurrent ());
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_TEMP_UNFOCUS], 99);
      break;
    case MenuObjEvent::eClick:
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjValuesSP);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjValuesRH);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjValuesSP (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_TEMP_UNFOCUS], 99);
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_SP_FOCUS], _ppm->getCurrent ());
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_SP_UNFOCUS], _ppm->getCurrent ());
      break;
    case MenuObjEvent::eClick:
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjValuesVALVE);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjValuesTEMP);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjValuesVALVE (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[sCURR_VALVE_FOCUS], "OFF");
      SetLineToConst (2, MENU_OBJ_LINES[BACK_TO_MENU_UNFOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[sCURR_VALVE_UNFOCUS], "OFF");
      break;
    case MenuObjEvent::eClick:
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjValuesBackToMenu);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjValuesSP);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjValuesBackToMenu (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[sCURR_VALVE_UNFOCUS], "OFF");
      SetLineToConst (2, MENU_OBJ_LINES[BACK_TO_MENU_FOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (2, MENU_OBJ_LINES[BACK_TO_MENU_UNFOCUS]);
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjSetCOLevel);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjValuesCO);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjValuesVALVE);
      break;

    default:
      break;
    }
}
