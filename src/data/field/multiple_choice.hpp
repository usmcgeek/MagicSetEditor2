//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) Twan van Laarhoven and the other MSE developers          |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#pragma once

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <data/field/choice.hpp>

// ----------------------------------------------------------------------------- : MultipleChoiceField

DECLARE_POINTER_TYPE(MultipleChoiceField);
DECLARE_POINTER_TYPE(MultipleChoiceStyle);
DECLARE_POINTER_TYPE(MultipleChoiceValue);

/// A ChoiceField where multiple choices can be selected simultaniously
class MultipleChoiceField : public ChoiceField {
public:
  MultipleChoiceField();
  DECLARE_FIELD_TYPE(MultipleChoiceField);
  
  UInt minimum_selection, maximum_selection; ///< How many choices can be selected simultaniously?
  String empty_choice; ///< Name to use when nothing is selected
};

// ----------------------------------------------------------------------------- : MultipleChoiceStyle

/// The Style for a MultipleChoiceField
class MultipleChoiceStyle : public ChoiceStyle {
public:
  MultipleChoiceStyle(const MultipleChoiceFieldP& field);
  DECLARE_STYLE_TYPE(MultipleChoice);
  
  Scriptable<Direction> direction;  ///< In what direction are choices layed out?
  Scriptable<double> spacing;      ///< Spacing between choices (images) in pixels
  
  int update(Context&) override;
};

// ----------------------------------------------------------------------------- : MultipleChoiceValue

/// The Value in a MultipleChoiceField
/** The value is stored as "<choice>, <choice>, <choice>"
 *  The choices must be ordered by id
 */
class MultipleChoiceValue : public ChoiceValue {
public:
  inline MultipleChoiceValue(const MultipleChoiceFieldP& field) : ChoiceValue(field, false) {}
  DECLARE_HAS_FIELD(MultipleChoice);
  ValueP clone() const override;
  
  String last_change; ///< Which of the choices was selected/deselected last?
  
  // for SimpleValueAction
  struct ValueType {
    ChoiceValue::ValueType value;
    String                 last_change;
  };
  
  /// Splits the value, stores the selected choices in the out parameter
  void get(vector<String>& out) const;
  
  bool update(Context&) override;
  
private:
  DECLARE_REFLECTION();
  
  /// Put the value in normal form (all choices ordered, empty_name
  void normalForm();
};

// ----------------------------------------------------------------------------- : Utilities

/// Is the given choice selected in the value?
bool chosen(const String& multiple_choice_value, const String& chioce);

