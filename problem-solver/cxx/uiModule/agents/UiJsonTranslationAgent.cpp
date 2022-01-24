/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "UiJsonTranslationAgent.hpp"
#include <sc-memory/sc_memory.hpp>

#include <sc-ui/uiPrecompiled.h>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include "keynodes/Keynodes.hpp"
#include "UiJsonTranslator.hpp"

using namespace std;
using namespace utils;

namespace uiModule {

    SC_AGENT_IMPLEMENTATION(UiJsonTranslationAgent)
            {

                    ScAddr actionNode = otherAddr;

            if (!checkActionClass(actionNode))
            {
                SC_LOG_DEBUG("UiJsonTranslationAgent: action class check failed")
                return SC_RESULT_OK;
            }
            ScAddr param = IteratorUtils::getFirstFromSet(ms_context.get(), actionNode);

            if (!param.IsValid())
            {
                SC_LOG_ERROR("Parameter is not valid.")
                utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
                return SC_RESULT_ERROR_INVALID_PARAMS;
            }
            if (!(isComponentValid(param))&&!(isSimpleComponentValid(param))) {
                SC_LOG_ERROR("Component not found.")
                utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
                return SC_RESULT_ERROR;
            }

            UiJsonTranslator translator(ms_context.get());
            ScAddr translationLinkAddr;
            try
            {
                translationLinkAddr = translator.getTranslatedUiJsonLink(param, ms_context.get());
                SC_LOG_INFO(CommonUtils::readString(ms_context.get(), translationLinkAddr));
            }
            catch (std::runtime_error & ex)
            {
                utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
                SC_LOG_ERROR(ex.what());
                return SC_RESULT_ERROR;
            }
            ScAddr answer;
            try
            {
                answer = generateAnswer(param, translationLinkAddr);
            }
            catch (std::runtime_error & ex)
            {
                SC_LOG_ERROR(ex.what())
                utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
                return SC_RESULT_ERROR;
            }
            SC_LOG_DEBUG("UiJsonTranslationAgent finished")
            utils::AgentUtils::finishAgentWork(ms_context.get(), param, answer, true);
            return SC_RESULT_OK;

            }

    ScAddr UiJsonTranslationAgent::generateAnswer(ScAddr const &uiComponentAddr, ScAddr const &linkAddr) {
        ScTemplate scTemplate;
        scTemplate.TripleWithRelation(
                ScType::NodeVar >> "_translation_node",
                ScType::EdgeDCommonVar >> "_common_arc",
                uiComponentAddr,
                ScType::EdgeAccessVarPosPerm >> "_access_arc",
                scAgentsCommon::CoreKeynodes::nrel_sc_text_translation);
        scTemplate.Triple(
                "_translation_node",
                ScType::EdgeAccessVarPosPerm >> "_arc",
                linkAddr
        );
        scTemplate.Triple(
                Keynodes::format_ui_json,
                ScType::EdgeAccessVarPosPerm >> "_arc1",
                linkAddr
        );
        ScTemplateParams templateParams;
        ScTemplateGenResult templateGenResult;
        ms_context->HelperGenTemplate(scTemplate, templateGenResult, templateParams);

        ScTemplate answerGenerationTemplate;

        answerGenerationTemplate.Triple(
                ScType::NodeVarStruct >> "_answer",
                ScType::EdgeAccessVarPosPerm,
                templateGenResult["_translation_node"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                templateGenResult["_common_arc"]);

        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                uiComponentAddr);

        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                templateGenResult["_access_arc"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                templateGenResult["_arc"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                templateGenResult["_arc1"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                scAgentsCommon::CoreKeynodes::nrel_sc_text_translation);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                linkAddr);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::format_ui_json);

        ScTemplateParams templateParam;
        ScTemplateGenResult templateResult;
        ms_context->HelperGenTemplate(answerGenerationTemplate, templateResult, templateParam);

        return templateResult["_answer"];
    }

    ScAddr UiJsonTranslationAgent::findStructure() {
        ScTemplate templat;
        templat.Triple(
                ScType::NodeVar >> "_node",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::concept_component
        );

        templat.TripleWithRelation(
                ScType::NodeVarStruct >> "_struct",
                ScType::EdgeDCommonVar,
                "_node",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::nrel_subdividing);

        ScTemplateSearchResult searchResult;

        ms_context->HelperSearchTemplate(templat, searchResult);

        return searchResult[0]["_struct"];
    }

    bool UiJsonTranslationAgent::isSimpleComponentValid(ScAddr const &param) {
        bool isValid = false;
        ScAddr structure = findStructure();

        if (utils::CommonUtils::checkType(ms_context.get(), param, ScType::NodeConst)) {
            ScTemplate templ;
            templ.Triple(
                    structure,
                    ScType::EdgeAccessVarPosPerm,
                    ScType::NodeVarClass >> "_class");
            templ.TripleWithRelation(
                    "_class",
                    ScType::EdgeDCommonVar,
                    ScType::NodeVarClass >> "_component",
                    ScType::EdgeAccessVarPosPerm,
                    scAgentsCommon::CoreKeynodes::nrel_inclusion);
            templ.Triple(
                    "_component",
                    ScType::EdgeAccessVarPosPerm,
                    param
            );

            ScTemplateSearchResult searchResult;
            ms_context->HelperSearchTemplate(templ, searchResult);
            isValid = searchResult.Size() >= 1;
        }
        return isValid;
    }


    bool UiJsonTranslationAgent::isComponentValid(ScAddr const &param) {
        bool isValid = false;
        if (utils::CommonUtils::checkType(ms_context.get(), param, ScType::NodeConst)) {
            ScTemplate templat;
            ScAddr structure = findStructure();
            templat.Triple(
                    structure,
                    ScType::EdgeAccessVarPosPerm,
                    ScType::NodeVarClass >> "_class");

            templat.TripleWithRelation(
                    "_class",
                    ScType::EdgeDCommonVar,
                    ScType::NodeVarClass >> "_comp_class",
                    ScType::EdgeAccessVarPosPerm,
                    scAgentsCommon::CoreKeynodes::nrel_inclusion);

            ScTemplateSearchResult searchResult;
            templat.TripleWithRelation(
                    "_comp_class",
                    ScType::EdgeDCommonVar,
                    ScType::NodeVarClass >> "_componen",
                    ScType::EdgeAccessVarPosPerm,
                    scAgentsCommon::CoreKeynodes::nrel_inclusion);

            templat.Triple(
                    "_componen",
                    ScType::EdgeAccessVarPosPerm,
                    param
            );

            ms_context->HelperSearchTemplate(templat, searchResult);

            isValid = searchResult.Size() >= 1;
        }
        return isValid;
    }

    bool UiJsonTranslationAgent::checkActionClass(ScAddr const &actionAddr) {
        return ms_context->HelperCheckEdge(Keynodes::action_translate_to_ui_json, actionAddr,
                                           ScType::EdgeAccessConstPosPerm);
    }

}
