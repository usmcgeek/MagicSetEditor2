//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) Twan van Laarhoven and the other MSE developers          |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#pragma once

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <gui/value/editor.hpp>
#include <render/value/package_choice.hpp>

DECLARE_SHARED_POINTER_TYPE(DropDownList);

// ----------------------------------------------------------------------------- : PackageChoiceValueEditor

/// An editor 'control' for editing PackageChoiceValues
class PackageChoiceValueEditor : public PackageChoiceValueViewer, public ValueEditor {
public:
  DECLARE_VALUE_EDITOR(PackageChoice);
  
  void draw(RotatedDC& dc) override;
  void determineSize(bool force_fit) override;
  bool onLeftDown(const RealPoint& pos, wxMouseEvent& ev) override;
  bool onChar(wxKeyEvent& ev) override;
  void onLoseFocus() override;
  
private:
  DropDownListP drop_down;
  friend class DropDownPackageChoiceList;
  /// Change the choice
  void change(const String& c);
  /// Initialize the drop down list
  void initDropDown();
};


