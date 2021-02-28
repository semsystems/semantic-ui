/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "UiJsonTranslationAgent.generated.hpp"

namespace uiModule
{

class UiJsonTranslationAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::action_translate_to_ui_json, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  bool isComponentValid(ScAddr const & jsonLink);
  ScAddr generateAnswer(ScAddr const & uiComponentAddr,ScAddr const &linkAddr);

};

} // namespace uiModule
