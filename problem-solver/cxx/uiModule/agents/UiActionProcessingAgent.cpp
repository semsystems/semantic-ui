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
#include <algorithm>

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

        ScAddr actionSubclass = findInternalActionSubclass(componentParam, uiActionClassParam);
        ScAddr actionMainClass = findInternalActionMainClass(actionSubclass);
        ScTemplateSearchResult actionParams = findActionParameters(actionSubclass);
        ScAddr answer = startInternalAction(actionMainClass, actionParams);

        SC_LOG_DEBUG("UiActionProcessingAgent finished")
        utils::AgentUtils::finishAgentWork(ms_context.get(), actionNode, answer, true);
        return SC_RESULT_OK;
    }

    bool UiActionProcessingAgent::checkActionClass(const ScAddr &action)
    {
        return ms_context->HelperCheckEdge(Keynodes::action_process_ui_action, action,
                                           ScType::EdgeAccessConstPosPerm);
    }

    ScAddr UiActionProcessingAgent::findInternalActionSubclass(const ScAddr &component, const ScAddr &uiAction)
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
                ScType::NodeVarClass >> "_sys_internal_action_subclass",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::nrel_action_initiated_by_ui);

        ScTemplateSearchResult result;

        ms_context->HelperSearchTemplate(scTemplate, result);
        return result[0]["_sys_internal_action_subclass"];
    }

    ScAddr UiActionProcessingAgent::findInternalActionMainClass(const ScAddr &actionSubclass)
    {
        ScTemplate scTemplate;

        scTemplate.TripleWithRelation(
                ScType::NodeVarClass >> "_sys_internal_action_class",
                ScType::EdgeDCommonVar,
                actionSubclass,
                ScType::EdgeAccessVarPosPerm,
                scAgentsCommon::CoreKeynodes::nrel_inclusion);

        ScTemplateSearchResult result;

        ms_context->HelperSearchTemplate(scTemplate, result);
        return result[0]["_sys_internal_action_class"];
    }

    ScTemplateSearchResult UiActionProcessingAgent::findActionParameters(const ScAddr &actionSubclass)
    {
        ScTemplate scTemplate1;

        scTemplate1.TripleWithRelation(
                actionSubclass,
                ScType::EdgeDCommonVar,
                ScType::NodeVar >> "_action_parameters_set",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::nrel_action_parameters);
        scTemplate1.Triple(
                Keynodes::concept_action_parameters_set,
                ScType::EdgeAccessVarPosPerm,
                "_action_parameters_set");

        ScTemplateSearchResult result1;

        ms_context->HelperSearchTemplate(scTemplate1, result1);
        ScAddr parameters = result1[0]["_action_parameters_set"];

        ScTemplate scTemplate2;

        scTemplate2.TripleWithRelation(
                parameters,
                ScType::EdgeAccessVarPosPerm,
                ScType::NodeVar >> "_value",
                ScType::EdgeAccessVarPosPerm,
                ScType::NodeVarRole >> "_role");

        ScTemplateSearchResult result2;

        ms_context->HelperSearchTemplate(scTemplate2, result2);

        if (!result2.IsEmpty())
        {
            return result2;
        }

        ScTemplate scTemplate3;

        scTemplate3.Triple(
                parameters,
                ScType::EdgeAccessVarPosPerm,
                ScType::NodeVar >> "_value");

        ScTemplateSearchResult result3;

        ms_context->HelperSearchTemplate(scTemplate3, result3);

        return result3;
    }

    ScAddr UiActionProcessingAgent::startInternalAction(const ScAddr &actionMainClass,
                                                        const ScTemplateSearchResult &constParameters)
    {
        vector<pair<int, ScAddr>> parametersMap;
        if (!constParameters.IsEmpty())
        {
            for (size_t i = 0; i < constParameters.Size(); i++)
            {
                ScAddr value = constParameters[i]["_value"];

                if (constParameters[i].Has("_role"))
                {
                    ScAddr role = constParameters[i]["_role"];
                    string roleIdtf = (ms_context->HelperGetSystemIdtf(role));
                    int roleNumber = stoi(roleIdtf.substr(roleIdtf.find('_') + 1));
                    parametersMap.emplace_back(roleNumber, value);
                }
                else
                {
                    parametersMap.emplace_back(1, value);
                }
            }

            sort(parametersMap.begin(), parametersMap.end(), cmp);
        }

        vector<ScAddr> parametersVec;
        parametersVec.reserve(parametersMap.size());
        for (auto elem : parametersMap)
        {
            parametersVec.push_back(elem.second);
        }

        ScAddr question = AgentUtils::initAgent(ms_context.get(),actionMainClass,parametersVec);

        SC_LOG_INFO("Start internal action " + ms_context->HelperGetSystemIdtf(actionMainClass))

        return question;
    }

    bool UiActionProcessingAgent::cmp(pair<int, ScAddr> &a, pair<int, ScAddr> &b)
    {
        return a.first < b.first;
    }
}