/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_addr.hpp>

#include "Keynodes.generated.hpp"

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

  SC_PROPERTY(Keynode("concept_user_interface_component"), ForceCreate)
  static ScAddr concept_component;

  SC_PROPERTY(Keynode("concept_component_class"), ForceCreate)
  static ScAddr concept_component_class;

  SC_PROPERTY(Keynode("nrel_subdividing"), ForceCreate)
  static ScAddr nrel_subdividing;

  SC_PROPERTY(Keynode("action_translate_to_ui_json"), ForceCreate)
  static ScAddr action_translate_to_ui_json;

  SC_PROPERTY(Keynode("concept_properties_set"), ForceCreate)
  static ScAddr concept_properties_set;

  SC_PROPERTY(Keynode("concept_atomic_component_of_user_interface"), ForceCreate)
  static ScAddr concept_atomic_component;

  SC_PROPERTY(Keynode("concept_non_atomic_component_of_user_interface"), ForceCreate)
  static ScAddr concept_non_atomic_component;

  SC_PROPERTY(Keynode("nrel_action_initiated_by_ui"), ForceCreate)
  static ScAddr nrel_action_initiated_by_ui;

  SC_PROPERTY(Keynode("concept_interface_user_action"), ForceCreate)
  static ScAddr concept_interface_user_action;

  SC_PROPERTY(Keynode("action_in_sc_memory"), ForceCreate)
  static ScAddr action_in_sc_memory;

  SC_PROPERTY(Keynode("action_process_ui_action"), ForceCreate)
  static ScAddr action_process_ui_action;

  SC_PROPERTY(Keynode("nrel_action_parameters"), ForceCreate)
  static ScAddr nrel_action_parameters;

  SC_PROPERTY(Keynode("concept_action_parameters_set"), ForceCreate)
  static ScAddr concept_action_parameters_set;
};

} // namespace uiModule
