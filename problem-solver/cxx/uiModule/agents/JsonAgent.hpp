/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "JsonAgent.generated.hpp"

namespace uiModule
{

class JsonAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_find_ui_json, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
  void getJson(ScAddr const &param);
  void searchPropertyValue(const ScTemplateItemValue &par);
  void searchProperty(const ScTemplateItemValue &concrete_component);
};

} // namespace uiModule
