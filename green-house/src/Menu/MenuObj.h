/*
 * MenuObj.h
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#include "Counter.h"
#include "EEPROMWrapper.h"
#include "LiquidCrystal.h"
#include "MenuObjEvent.h"
#include "common_values.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef MENU_MENUOBJ_H_
#define MENU_MENUOBJ_H_

#define EEPROM_ADDRESS 0x00000100

enum
{
  CO2_FOCUS,
  CO2_UNFOCUS,
  SHOW_VALUES_FOCUS,
  SHOW_VALUES_UNFOCUS,
  SHOW_ALL_FOCUS,
  SHOW_ALL_UNFOCUS,
  RESET_FOCUS,
  RESET_UNFOCUS,
  SET_PPM_FOCUS,
  SET_PPM_UNFOCUS,
  CHANGE_PPM,
  BACK_UNFOCUS_SAVE_UNFOCUS,
  BACK_FOCUS_SAVE_UNFOCUS,
  BACK_UNFOCUS_SAVE_FOCUS,
  CURR_CO2_UNFOCUS,
  CURR_CO2_FOCUS,
  CURR_HUM_UNFOCUS,
  CURR_HUM_FOCUS,
  CURR_TEMP_UNFOCUS,
  CURR_TEMP_FOCUS,
  CURR_SP_UNFOCUS,
  CURR_SP_FOCUS,
  sCURR_VALVE_UNFOCUS,
  sCURR_VALVE_FOCUS,
  BACK_TO_MENU_UNFOCUS,
  BACK_TO_MENU_FOCUS,
  ALL_L_1_4f_4d,
  ALL_L_2_2f_2f_s,
  HARD_RESET,
  HR_YES_UF_NO_F,
  HR_YES_F_NO_UF,
  WAIT_1,
  WAIT_2

};

class MenuObj;
typedef void (MenuObj::*obj_pointer) (const MenuObjEvent &);

class MenuObj
{
public:
  MenuObj (LiquidCrystal *lcd, Counter<uint16_t> *ppm, EEPROM_Wrapper *eeprom,
           GH_DATA *gh_display, QueueHandle_t *gh_d_q);
  virtual ~MenuObj ();

  /**
   * @brief Handle the action of rotary
   *
   * @param action press / rotate
   */
  void HandleRotaryAction (RotaryAction action);

private:
  /* Variables and objects */
  TickType_t timestamp;
  obj_pointer current;
  LiquidCrystal *_lcd;
  char lcd_line[2][18] = { { 0 }, { 0 } };
  Counter<uint16_t> *_ppm;
  EEPROM_Wrapper *_eeprom;
  GH_DATA *_gh_display;
  QueueHandle_t *_display_gh_q;

  /* Methods */
  void SetEvent (obj_pointer newevent);
  void SetLineToConst (uint8_t line, const char *to);
  void SetLineToFMT (uint8_t line, const char *fmt, ...);
  void updateLCD (void);
  void readSetPointFromEEPROM (void);
  void eraseSetPointFromEEPROM (void);
  void saveSetPointToEEPROM (void);

  /** Handle the given event of the current MenuObj
   *
   * @param event event to be handled in the MenuObj
   */
  void HandleObj (const MenuObjEvent &event);

  /** Set CO2 level
   *
   * @param event event of the state
   */
  void ObjSetCOLevel (const MenuObjEvent &event);

  /** Set PPM
   *
   * @param event event of the state
   */
  void ObjSetPPM (const MenuObjEvent &event);

  /** Change PPM Value
   *
   * @param event event of the state
   */
  void ObjChangePPMValue (const MenuObjEvent &event);

  /** Back
   *
   * @param event event of the state
   */
  void ObjBack (const MenuObjEvent &event);

  /** Save
   *
   * @param event event of the state
   */
  void ObjSave (const MenuObjEvent &event);

  /** Values
   *
   * @param event event of the state
   */
  void ObjShowValues (const MenuObjEvent &event);

  /** All values minimalistic
   *
   * @param event event of the state
   */
  void ObjShowValuesMax (const MenuObjEvent &event);

  /** SHOW all values
   *
   * @param event event of the state
   */
  void ObjShowALL (const MenuObjEvent &event);

  /** Erase EEPROM
   *
   * @param event event of the state
   */
  void ObjReset (const MenuObjEvent &event);

  /** CO2
   *
   * @param event event of the state
   */
  void ObjValuesCO (const MenuObjEvent &event);

  /** RH
   *
   * @param event event of the state
   */
  void ObjValuesRH (const MenuObjEvent &event);

  /** TEMP
   *
   * @param event event of the state
   */
  void ObjValuesTEMP (const MenuObjEvent &event);

  /** SP
   *
   * @param event event of the state
   */
  void ObjValuesSP (const MenuObjEvent &event);

  /** VALVE
   *
   * @param event event of the state
   */
  void ObjValuesVALVE (const MenuObjEvent &event);

  /** Back to Menu
   *
   * @param event event of the state
   */
  void ObjValuesBackToMenu (const MenuObjEvent &event);

  /** Hard reset yes
   *
   * @param event event of the state
   */
  void ObjHARDResetYes (const MenuObjEvent &event);

  /** Hard reset no
   *
   * @param event event of the state
   */
  void ObjHARDResetNo (const MenuObjEvent &event);

  /** Waiting for values
   *
   * @param event event of the state
   */
  void ObjWait (const MenuObjEvent &event);
};

#endif /* MENU_MENUOBJ_H_ */
