/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "UiActionProcessingAgent.hpp"
#include <sc-memory/sc_memory.hpp>

#include <sc-ui/uiPrecompiled.h>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include "keynodes/Keynodes.hpp"
#include "utils/SearchUtils.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

using namespace std;
using namespace utils;

namespace uiModule
{

    SC_AGENT_IMPLEMENTATION(UiActionProcessingAgent)
    {
        ScAddr actionNode = otherAddr;

        if (!checkActionClass(actionNode))
        {
            SC_LOG_DEBUG("UiActionProcessingAgent: action class check failed")
            return SC_RESULT_OK;
        }

        ScAddr componentParam = SearchUtils::getFirstParamFromSet(ms_context.get(), actionNode);
        if (!componentParam.IsValid()
            && CommonUtils::checkType(ms_context.get(), componentParam, ScType::NodeConst))
        {
            SC_LOG_ERROR("Parameter is not valid.")
            utils::AgentUtils::finishAgentWork(ms_context.get(), componentParam, false);
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }

        ScAddr uiActionClassParam = SearchUtils::getSecondParamFromSet(ms_context.get(), actionNode);
        if (!uiActionClassParam.IsValid()
            && CommonUtils::checkType(ms_context.get(), componentParam, ScType::NodeConstClass))
        {
            SC_LOG_ERROR("Parameter is not valid.")
            utils::AgentUtils::finishAgentWork(ms_context.get(), uiActionClassParam, false);
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }

        ScAddr internalAction = findInternalAction(componentParam, uiActionClassParam);
        ScAddr answer = startInternalActionAndGenerateAnswer(internalAction);

        SC_LOG_DEBUG("UiActionProcessingAgent finished")
        utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, answer, true);
        return SC_RESULT_OK;
    }

    bool UiActionProcessingAgent::checkActionClass(const ScAddr &action)
    {
        return ms_context->HelperCheckEdge(Keynodes::action_process_ui_action, action,
                                           ScType::EdgeAccessConstPosPerm);
    }

    ScAddr UiActionProcessingAgent::findInternalAction(const ScAddr &component, const ScAddr &uiAction)
    {
        ScTemplate scTemplate;

        scTemplate.TripleWithRelation(
                ScType::NodeVar >> "_tuple",
                ScType::EdgeAccessVarPosPerm,
                component,
                ScType::EdgeAccessVarPosPerm,
                scAgentsCommon::CoreKeynodes::rrel_1);
        scTemplate.TripleWithRelation(
                "_tuple",
                ScType::EdgeAccessVarPosPerm,
                uiAction,
                ScType::EdgeAccessVarPosPerm,
                scAgentsCommon::CoreKeynodes::rrel_2);
        scTemplate.TripleWithRelation(
                "_tuple",
                ScType::EdgeDCommonVar,
                ScType::NodeVarClass >> "_sys_internal_action_class",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::nrel_action_initiated_by_ui);

        ScTemplateSearchResult result;

        ms_context->HelperSearchTemplate(scTemplate, result);
        return result[0]["_sys_internal_action_class"];
    }

    ScAddr UiActionProcessingAgent::startInternalActionAndGenerateAnswer(const ScAddr &internalAction)
    {
        ScTemplate startInternalActionGenTemplate;
        startInternalActionGenTemplate.Triple(
                internalAction,
                ScType::EdgeAccessVarPosPerm >> "_arc1",
                ScType::NodeVar >> "_question_node"
        );
        startInternalActionGenTemplate.Triple(
                scAgentsCommon::CoreKeynodes::question,
                ScType::EdgeAccessVarPosPerm >> "_arc2",
                "_question_node"
        );
        startInternalActionGenTemplate.Triple(
                scAgentsCommon::CoreKeynodes::question_initiated,
                ScType::EdgeAccessVarPosPerm >> "_arc3",
                "_question_node"
        );
        ScTemplateParams templateParams;
        ScTemplateGenResult startInternalActionGenTemplateResult;
        ms_context->HelperGenTemplate(startInternalActionGenTemplate, startInternalActionGenTemplateResult,
                                      templateParams);

        SC_LOG_DEBUG("Start internal action " + ms_context->HelperGetSystemIdtf(internalAction))

        ScTemplate answerGenTemplate;
        answerGenTemplate.Triple(
                ScType::NodeVarStruct >> "_answer",
                ScType::EdgeAccessVarPosPerm,
                internalAction);
        answerGenTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                startInternalActionGenTemplateResult["_question_node"]);
        answerGenTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                startInternalActionGenTemplateResult["_arc1"]);
        answerGenTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                scAgentsCommon::CoreKeynodes::question);
        answerGenTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                startInternalActionGenTemplateResult["_arc2"]);
        answerGenTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                scAgentsCommon::CoreKeynodes::question_initiated);
        answerGenTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                startInternalActionGenTemplateResult["_arc3"]);
        ScTemplateGenResult answerGenTemplateResult;
        ms_context->HelperGenTemplate(answerGenTemplate, answerGenTemplateResult, templateParams);

        return answerGenTemplateResult["_answer"];
    }
}