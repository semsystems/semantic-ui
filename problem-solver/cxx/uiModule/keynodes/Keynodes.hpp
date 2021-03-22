/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_addr.hpp>

#include "keynodes.generated.hpp"

namespace uiModule
{

class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:

  SC_PROPERTY(Keynode("nrel_json_idtf"), ForceCreate)
  static ScAddr keynode_nrel_json_idtf;

  SC_PROPERTY(Keynode("format_ui_json"), ForceCreate)
  static ScAddr format_ui_json;

  SC_PROPERTY(Keynode("concept_button"), ForceCreate)
  static ScAddr concept_button;

  SC_PROPERTY(Keynode("concept_component"), ForceCreate)
  static ScAddr concept_component;

  SC_PROPERTY(Keynode("concept_component_class"), ForceCreate)
  static ScAddr concept_component_class;

  SC_PROPERTY(Keynode("nrel_inclusion"), ForceCreate)
  static ScAddr nrel_inclusion;

  SC_PROPERTY(Keynode("action_translate_to_ui_json"), ForceCreate)
  static ScAddr action_translate_to_ui_json;

  SC_PROPERTY(Keynode("nrel_sc_text_translation"), ForceCreate)
  static ScAddr nrel_sc_text_translation;

  SC_PROPERTY(Keynode("question_finished_successfully"), ForceCreate)
  static ScAddr question_finished_successfully;
};

} // namespace uiModule
