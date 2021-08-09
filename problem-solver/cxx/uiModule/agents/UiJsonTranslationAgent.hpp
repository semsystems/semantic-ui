/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>
#include "UiJsonTranslator.hpp"
#include "keynodes/Keynodes.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "UiJsonTranslationAgent.generated.hpp"

namespace uiModule
{

class UiJsonTranslationAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  ScAddr findStructure();

  bool isComponentValid(ScAddr const & jsonLink);
  bool isSimpleComponentValid(ScAddr const & jsonLink);

  ScAddr generateAnswer(ScAddr const & uiComponentAddr, ScAddr const & linkAddr);

  bool checkActionClass(ScAddr const & actionAddr);

};

} // namespace uiModule
