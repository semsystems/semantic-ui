/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/kpm/sc_agent.hpp>
#include "keynodes/Keynodes.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "UiActionProcessingAgent.generated.hpp"

namespace uiModule
{

class UiActionProcessingAgent : public ScAgent
{
    SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
    SC_GENERATED_BODY()

    bool checkActionClass(ScAddr const &actionAddr);
    ScAddr findInternalAction(const ScAddr &component, const ScAddr &uiAction);
    ScAddr startInternalActionAndGenerateAnswer(const ScAddr &internalAction);
};

} // namespace uiModule