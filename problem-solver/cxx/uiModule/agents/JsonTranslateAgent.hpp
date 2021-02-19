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

class JsonTranslateAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::action_translate_to_ui_json, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
  ScAddr getTranslateJson(ScAddr const &param);
  void searchPropertyValue(const ScTemplateItemValue &par);
  bool componentIsValid(ScAddr const & jsonLink);
  ScAddr generateMessage( ScAddr const & jsonLinkAddr,ScAddr const &componentAddr);
  ScAddr generateAnswer(ScAddr const & messageAddr,ScAddr const &linkAddr);

};

} // namespace uiModule
