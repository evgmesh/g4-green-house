/*
 * MenuObj.h
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#ifndef MENU_MENUOBJ_H_
#define MENU_MENUOBJ_H_

#include "Counter.h"
#include "LiquidCrystal.h"
#include "Menu/MenuObjEvent.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

enum
{
  CO2_FOCUS,
  CO2_UNFOCUS,
  SET_PPM_FOCUS,
  SET_PPM_UNFOCUS,
  CHANGE_PPM,
  BACK_UNFOCUS_SAVE_UNFOCUS,
  BACK_FOCUS_SAVE_UNFOCUS,
  BACK_UNFOCUS_SAVE_FOCUS
};

const char *MENU_OBJ_LINES[]
    = { "[SET CO2 LVL]",   " SET CO2 LVL ",  "SET [%4d] PPM",
        "SET  %4d  PPM",   "SET <%4d> PPM",  " BACK     SAVE ",
        "[BACK]    SAVE ", " BACK    [SAVE]" };

class MenuObj;
typedef void (MenuObj::*obj_pointer) (const MenuObjEvent &);

class MenuObj
{
public:
  MenuObj (LiquidCrystal *lcd, Counter<uint16_t> *ppm);
  virtual ~MenuObj ();

  /** Handle the given event of the current MenuObj
   *
   * @param event event to be handled in the MenuObj
   */
  void HandleObj (const MenuObjEvent &event);


private:
  /* Variables and objects */
  obj_pointer current;
  LiquidCrystal *_lcd;
  char lcd_line[2][16] = { { 0 }, { 0 } };
  Counter<uint16_t> *_ppm;

  /* Methods */
  void SetEvent (obj_pointer newevent);
  void SetLineToConst (uint8_t line, const char *to);
  void SetLineToFMT (uint8_t line, const char *fmt, ...);

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

  /** Sensors
   *
   * @param event event of the state
   */
  void ObjSensors (const MenuObjEvent &event);
};

#endif /* MENU_MENUOBJ_H_ */
