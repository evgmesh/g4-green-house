/*
 * MenuObj.h
 *
 *  Created on: Dec 11, 2022
 *      Author: tylen
 */

#ifndef MENU_MENUOBJ_H_
#define MENU_MENUOBJ_H_

#include "LiquidCrystal.h"
#include "Menu/MenuObjEvent.h"

class MenuObj;
typedef void (MenuObj::*obj_pointer) (const MenuObjEvent &);

class MenuObj
{
public:
  MenuObj (LiquidCrystal *lcd);
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
  char lcd_line_1[16] = { 0 };
  char lcd_line_2[16] = { 0 };

  /* Methods */
  void SetEvent (obj_pointer newevent);

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
