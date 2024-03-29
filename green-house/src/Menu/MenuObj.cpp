/*
 * MenuObj.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#include "MenuObj.h"

const char *MENU_OBJ_LINES[]
    = { "[SET CO2 LVL]",        " SET CO2 LVL ",    "[SHOW VALUES]",
        " SHOW VALUES ",        "[SHOW ALL]",       " SHOW ALL",
        "[HARD RESET]",         " HARD RESET ",     "SET [%4d] PPM",
        "SET  %4d  PPM",        "SET <%4d> PPM",    " BACK     SAVE ",
        "[BACK]    SAVE ",      " BACK    [SAVE]",  "  CO2:%4.0f PPM ",
        "> CO2:%4.0f PPM",      "  RH:%4.1f%\%",    "> RH:%4.1f%\%",
        "  TEMP: %4.1f C ",     "> TEMP: %4.1f C",  "  SP:%4d PPM ",
        "> SP:%4d PPM",         "  VALVE:%s ",      "> VALVE:%s",
        " BACK TO MENU ",       "[BACK TO MENU]",   "CO2:%4.0f SP:%4d",
        "H:%3.0f T:%3.0f V:%d", "   HARD RESET  ",  " YES      [NO]",
        "[YES]      NO ",       "Launching ...",    " Moment please ",
        " SET NETWORK ",        "[SET NETWORK]",    "ENTER SSID \"...\"",
        "ENTER PASS \"...\"",   "ENTER IP:X.X.X.X", "%d.%d.%d.%d",
        "%3d.%3d.%3d.%3d" };

MenuObj::MenuObj (LiquidCrystal *lcd, Counter<uint16_t> *ppm,
                  EEPROM_Wrapper *eeprom, GH_DATA *gh_display,
                  SemaphoreHandle_t *sp_sig, ND *network)
    : symbols (33, 122, 1), ip_digit (0, 255, 1)
{
  timestamp = 0;
  _eeprom = eeprom;
  _lcd = lcd;
  _ppm = ppm;
  _gh_display = gh_display;
  _set_point_sig = sp_sig;
  _network = network;
  memset (_network->ssid, 0x00, ND_SSID_MAX_LENGTH);
  memset (_network->password, 0x00, ND_SSID_MAX_LENGTH);
  char_counter = 0;
  current = &MenuObj::ObjWait;
  _ppm->setCurrent(750);
  readSetPointFromEEPROM ();
  if (!readNetworkDataFromEEPROM ())
    {

      current = &MenuObj::ObjSetNetwork;
    }
  else
    {
      xQueueSend (network_q, (void *)_network, 0);
    }
  HandleObj (MenuObjEvent (MenuObjEvent::eFocus));
}

MenuObj::~MenuObj ()
{
  // TODO Auto-generated destructor stub
}

void
MenuObj::readSetPointFromEEPROM (void)
{
  uint16_t *data = (uint16_t *)_eeprom->read_from (SETPOINT_EEPROM_ADDRESS,
                                                   SETPOINT_SIZE);
  if ((*data) > 200 && (*data) < 9999)
    {
      _ppm->setCurrent (*data);
      _gh_display->set_point = _ppm->getCurrent ();
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
  _eeprom->write_to (SETPOINT_EEPROM_ADDRESS, &data, SETPOINT_SIZE);
}

bool
MenuObj::readNetworkDataFromEEPROM (void)
{
  ND *data = (ND *)_eeprom->read_from (ND_EEPROM_ADDRESS, ND_SIZE);
  if (data->ssid[0] && data->password[0] && data->ip[0])
    {
      memcpy (_network->ssid, data->ssid, ND_SSID_MAX_LENGTH);
      memcpy (_network->password, data->password, ND_PASSWORD_MAX_LENGTH);
      memcpy (_network->ip, data->ip, ND_IP_MAX_LENGTH);
      return true;
    }
  return false;
}

void
MenuObj::eraseNetworkDatFromEEPROM (void)
{
  memset (_network->ssid, 0x00, ND_SSID_MAX_LENGTH);
  memset (_network->password, 0x00, ND_SSID_MAX_LENGTH);
  memset (_network->ip, 0x00, ND_IP_MAX_LENGTH);
  saveNetworkDatToEEPROM ();
}

void
MenuObj::saveNetworkDatToEEPROM (void)
{
  _eeprom->write_to (ND_EEPROM_ADDRESS, _network, ND_SIZE);
}

void
MenuObj::HandleRotaryAction (RotaryAction action)
{
  TickType_t current_timestamp = xTaskGetTickCount ();
  switch (action)
    {
    case ROTARY_ACTION::ROTARY_CLOCKWISE:
      HandleObj (MenuObjEvent (MenuObjEvent::eRollClockWise));
      break;
    case ROTARY_ACTION::ROTARY_CCLOCKWISE:
      HandleObj (MenuObjEvent (MenuObjEvent::eRollCClockWise));
      break;
    case ROTARY_ACTION::ROTARY_PRESS:
      if (current_timestamp - timestamp > 200)
        {
          HandleObj (MenuObjEvent (MenuObjEvent::eClick));
          timestamp = current_timestamp;
        }
    case ROTARY_ACTION::ROTARY_IDLE:
      if (current == &MenuObj::ObjWait)
        {
          SetEvent (&MenuObj::ObjShowALL);
        }
      if (current == &MenuObj::ObjShowALL || current == &MenuObj::ObjValuesCO
          || current == &MenuObj::ObjValuesRH
          || current == &MenuObj::ObjValuesSP
          || current == &MenuObj::ObjValuesTEMP
          || current == &MenuObj::ObjValuesVALVE)
        {
          HandleObj (MenuObjEvent (MenuObjEvent::eFocus));
        }
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
  memcpy (lcd_line[line - 1], to, 17);
}

void
MenuObj::SetLineToFMT (uint8_t line, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vsnprintf (lcd_line[line - 1], 17, fmt, args);
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
      SetEvent (&MenuObj::ObjSetNetwork);
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
      _gh_display->set_point = _ppm->getCurrent ();
      xSemaphoreGive (*_set_point_sig);
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
      saveSetPointToEEPROM ();
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
      SetLineToFMT (1, MENU_OBJ_LINES[ALL_L_1_4f_4d], _gh_display->co2_val,
                    _ppm->getCurrent ());
      SetLineToFMT (2, MENU_OBJ_LINES[ALL_L_2_2f_2f_s], _gh_display->rhum_val,
                    _gh_display->temp_val, (_gh_display->valve_open) ? 1 : 0);
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
      SetEvent (&MenuObj::ObjHARDResetNo);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjSetNetwork);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjShowValuesMax);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjSetNetwork (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[SET_NETWORK_FOCUS]);
      SetLineToConst (2, "");
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (1, MENU_OBJ_LINES[SET_NETWORK_UNFOCUS]);
      break;
    case MenuObjEvent::eClick:
      eraseNetworkDatFromEEPROM ();
      ip_numbers[0] = 0;
      ip_numbers[1] = 0;
      ip_numbers[2] = 0;
      ip_numbers[3] = 0;
      SetEvent (&MenuObj::ObjSetSSID);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjSetCOLevel);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjReset);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjSetSSID (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[ND_SSID]);
      _network->ssid[char_counter] = '"';
      symbols.setCurrent ('"');
      char_counter = 1;
      _network->ssid[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->ssid);
      break;
    case MenuObjEvent::eUnFocus:
      char_counter = 0;
      break;
    case MenuObjEvent::eClick:
      if (char_counter > 15
          || (_network->ssid[char_counter] == '"' && char_counter != 0))
        {
          SetEvent (&MenuObj::ObjSetPASSWD);
          break;
        }
      char_counter++;
      _network->ssid[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->ssid);
      break;
    case MenuObjEvent::eRollClockWise:
      symbols.inc ();
      _network->ssid[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->ssid);
      break;
    case MenuObjEvent::eRollCClockWise:
      symbols.dec ();
      _network->ssid[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->ssid);
      break;
    default:
      break;
    }
}

void
MenuObj::ObjSetPASSWD (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[ND_PASSWORD]);
      _network->password[char_counter] = '"';
      symbols.setCurrent ('"');
      char_counter = 1;
      _network->password[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->password);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (1, "");
      SetLineToConst (2, "");
      char_counter = 0;
      break;
    case MenuObjEvent::eClick:
      if (char_counter > 15
          || (_network->password[char_counter] == '"' && char_counter != 0))
        {
          SetEvent (&MenuObj::ObjSetIP);
          break;
        }
      char_counter++;
      _network->password[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->password);
      break;
    case MenuObjEvent::eRollClockWise:
      symbols.inc ();
      _network->password[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->password);
      break;
    case MenuObjEvent::eRollCClockWise:
      symbols.dec ();
      _network->password[char_counter] = symbols.getCurrent ();
      SetLineToFMT (2, "%s", _network->password);
      break;
    default:
      break;
    }
}

void
MenuObj::ObjSetIP (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[ND_IP]);
      SetLineToFMT (2, MENU_OBJ_LINES[ND_IP_FMT_STRICT], ip_numbers[0],
                    ip_numbers[1], ip_numbers[2], ip_numbers[3]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToConst (1, "");
      SetLineToConst (2, "");
      char_counter = 0;
      break;
    case MenuObjEvent::eClick:
      if (char_counter == 3)
        {
          snprintf (_network->ip, ND_IP_MAX_LENGTH, MENU_OBJ_LINES[ND_IP_FMT],
                    ip_numbers[0], ip_numbers[1], ip_numbers[2],
                    ip_numbers[3]);
          saveNetworkDatToEEPROM ();
          xQueueSend (network_q, (void *)_network, 0);
          SetEvent (&MenuObj::ObjSetCOLevel);
          break;
        }
      char_counter++;
      ip_numbers[char_counter] = ip_digit.getCurrent ();
      SetLineToFMT (2, MENU_OBJ_LINES[ND_IP_FMT_STRICT], ip_numbers[0],
                    ip_numbers[1], ip_numbers[2], ip_numbers[3]);
      break;
    case MenuObjEvent::eRollClockWise:
      ip_digit.inc ();
      ip_numbers[char_counter] = ip_digit.getCurrent ();
      SetLineToFMT (2, MENU_OBJ_LINES[ND_IP_FMT_STRICT], ip_numbers[0],
                    ip_numbers[1], ip_numbers[2], ip_numbers[3]);
      break;
    case MenuObjEvent::eRollCClockWise:
      ip_digit.dec ();
      ip_numbers[char_counter] = ip_digit.getCurrent ();
      SetLineToFMT (2, MENU_OBJ_LINES[ND_IP_FMT_STRICT], ip_numbers[0],
                    ip_numbers[1], ip_numbers[2], ip_numbers[3]);
      break;
    default:
      break;
    }
}

void
MenuObj::ObjHARDResetYes (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[HARD_RESET]);
      SetLineToConst (2, MENU_OBJ_LINES[HR_YES_F_NO_UF]);
      break;
    case MenuObjEvent::eUnFocus:
      break;
    case MenuObjEvent::eClick:
      eraseSetPointFromEEPROM ();
      eraseNetworkDatFromEEPROM ();
      SetEvent (&MenuObj::ObjReset);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjHARDResetNo);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjHARDResetNo);
      break;

    default:
      break;
    }
}

void
MenuObj::ObjHARDResetNo (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[HARD_RESET]);
      SetLineToConst (2, MENU_OBJ_LINES[HR_YES_UF_NO_F]);
      break;
    case MenuObjEvent::eUnFocus:
      break;
    case MenuObjEvent::eClick:
      SetEvent (&MenuObj::ObjReset);
      break;
    case MenuObjEvent::eRollClockWise:
      SetEvent (&MenuObj::ObjHARDResetYes);
      break;
    case MenuObjEvent::eRollCClockWise:
      SetEvent (&MenuObj::ObjHARDResetYes);
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
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_CO2_FOCUS], _gh_display->co2_val);
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_HUM_UNFOCUS],
                    _gh_display->rhum_val);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_CO2_UNFOCUS], _gh_display->co2_val);
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
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_CO2_UNFOCUS], _gh_display->co2_val);
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_HUM_FOCUS], _gh_display->rhum_val);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_HUM_UNFOCUS],
                    _gh_display->rhum_val);
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
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_TEMP_FOCUS], _gh_display->temp_val);
      SetLineToFMT (2, MENU_OBJ_LINES[CURR_SP_UNFOCUS], _ppm->getCurrent ());
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_TEMP_UNFOCUS],
                    _gh_display->temp_val);
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
      SetLineToFMT (1, MENU_OBJ_LINES[CURR_TEMP_UNFOCUS],
                    _gh_display->temp_val);
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
      SetLineToFMT (1, MENU_OBJ_LINES[sCURR_VALVE_FOCUS],
                    (_gh_display->valve_open) ? "ON" : "OFF");
      SetLineToConst (2, MENU_OBJ_LINES[BACK_TO_MENU_UNFOCUS]);
      break;
    case MenuObjEvent::eUnFocus:
      SetLineToFMT (1, MENU_OBJ_LINES[sCURR_VALVE_UNFOCUS],
                    (_gh_display->valve_open) ? "ON" : "OFF");
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
      SetLineToFMT (1, MENU_OBJ_LINES[sCURR_VALVE_UNFOCUS],
                    (_gh_display->valve_open) ? "ON" : "OFF");
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

void
MenuObj::ObjWait (const MenuObjEvent &event)
{
  switch (event.type)
    {
    case MenuObjEvent::eFocus:
      SetLineToConst (1, MENU_OBJ_LINES[WAIT_1]);
      SetLineToConst (2, MENU_OBJ_LINES[WAIT_2]);
      break;
    case MenuObjEvent::eUnFocus:
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
