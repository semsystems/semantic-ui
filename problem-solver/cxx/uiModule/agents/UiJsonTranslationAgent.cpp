/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "UiJsonTranslationAgent.hpp"
#include <sc-memory/sc_link.hpp>
#include <sc-memory/sc_memory.hpp>

#include <sc-agents-common/utils/GenerationUtils.hpp>

#include <sc-ui/uiPrecompiled.h>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include "keynodes/keynodes.hpp"

using namespace std;
using namespace utils;

namespace uiModule {
    StringStream ss;

    SC_AGENT_IMPLEMENTATION(UiJsonTranslationAgent) {
        class ITranslator {

        public:
            virtual ScAddr getTranslatedUiJsonLink(ScAddr const &param) = 0;

        };
        class Translator : public ITranslator {
        public:

            ScTemplateSearchResult searchPropertyValue(const ScTemplateItemValue &concrete_component)  {

                ScTemplate templ;

                templ.Triple(
                        ScType::NodeVarClass >> "_property",
                        ScType::EdgeAccessVarPosPerm,
                        concrete_component);
                templ.Triple(
                        ScType::NodeVarClass >> "_property_class",
                        ScType::EdgeAccessVarPosPerm,
                        "_property");
                ScTemplateSearchResult searchResult;
                templ.TripleWithRelation(
                        "_property",
                        ScType::EdgeDCommonVar,
                        ScType::NodeVar >> "_value",
                        ScType::EdgeAccessVarPosPerm,
                        ScType::NodeVarNoRole >> "_nrel");
                templ.TripleWithRelation(
                        "_nrel",
                        ScType::EdgeDCommonVar,
                        ScType::LinkVar >> "_name_nrel",
                        ScType::EdgeAccessVarPosPerm,
                        Keynodes::keynode_nrel_json_idtf);
                templ.TripleWithRelation(
                        "_property_class",
                        ScType::EdgeDCommonVar,
                        ScType::LinkVar >> "_property_class_name",
                        ScType::EdgeAccessVarPosPerm,
                        Keynodes::keynode_nrel_json_idtf);

                ms_context->HelperSearchTemplate(templ, searchResult);
                return searchResult;
            };

            void printPropertyValue(ScTemplateSearchResult searchResult)  {

                for (size_t i = 0; i < searchResult.Size(); i++) {
                    ss << "\"" << CommonUtils::readString(ms_context.get(), searchResult[i]["_property_class_name"])
                       << "\":\"" <<
                       ms_context->HelperGetSystemIdtf(searchResult[i]["_value"])
                       << CommonUtils::readString(ms_context.get(), searchResult[i]["_name_nrel"]) << "\",";
                }
            };

            String  formattingJson(ScTemplateSearchResult searchResult,const ScAddr &param){
                ss.str("");
                ss << "{" << "\"component\":\""
                   << CommonUtils::readString(ms_context.get(), searchResult[0]["_name_component"]) << "\",\"text\":\""
                   << ms_context->HelperGetSystemIdtf(param) << "\",";

                printPropertyValue(searchPropertyValue(param));
                ss << "}";
                String stringJson;
                stringJson = ss.str();
                stringJson.erase(stringJson.end() - 2, stringJson.end() - 1);
                return stringJson;
            }
            ScAddr getTranslatedUiJsonLink(ScAddr const &param) override {

                ScTemplate templ;
                templ.TripleWithRelation(
                        Keynodes::concept_component,
                        ScType::EdgeDCommonVar,
                        ScType::NodeVarClass >> "_component",
                        ScType::EdgeAccessVarPosPerm,
                        Keynodes::nrel_inclusion);
                templ.Triple(
                        "_component",
                        ScType::EdgeAccessVarPosPerm,
                        param
                );

                templ.TripleWithRelation(
                        "_component",
                        ScType::EdgeDCommon,
                        ScType::LinkVar >> "_name_component",
                        ScType::EdgeAccessVarPosPerm,
                        Keynodes::keynode_nrel_json_idtf);

                templ.Triple(
                        ScType::NodeVarClass >> "_property",
                        ScType::EdgeAccessVarPosPerm,
                        param
                );
                templ.Triple(
                        ScType::NodeVarClass >> "_property_class",
                        ScType::EdgeAccessVarPosPerm,
                        "_property");

                templ.TripleWithRelation(
                        "_property",
                        ScType::EdgeDCommonVar,
                        ScType::LinkVar >> "_property_name",
                        ScType::EdgeAccessVarPosPerm,
                        Keynodes::keynode_nrel_json_idtf);
                templ.TripleWithRelation(
                        "_property_class",
                        ScType::EdgeDCommonVar,
                        ScType::LinkVar >> "_property_class_name",
                        ScType::EdgeAccessVarPosPerm,
                        Keynodes::keynode_nrel_json_idtf);

                ScTemplateParams templateParams;
                ScTemplateGenResult templateGenResult;
                ScTemplateSearchResult searchResult;
                ms_context->HelperSearchTemplate(templ, searchResult);
                String stringJson=formattingJson(searchResult,param);
                ScAddr answer;
                answer = ms_context->CreateNode(ScType::LinkConst);
                ms_context->SetLinkContent(answer, ScStreamMakeRead(stringJson));

                return answer;
            }
        };
        if (!edgeAddr.IsValid())
            return SC_RESULT_ERROR;
        ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
        ScAddr param = IteratorUtils::getFirstFromSet(ms_context.get(), questionNode);
        if (!param.IsValid())
            return SC_RESULT_ERROR_INVALID_PARAMS;
        if (!isComponentValid(param)) {
            SC_LOG_ERROR("Component not found.")
            utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
            return SC_RESULT_ERROR;
        }

        Translator translator;
        ScAddr translationLinkAddr;
        try {
            translationLinkAddr = translator.getTranslatedUiJsonLink(param);
            SC_LOG_INFO( CommonUtils::readString(ms_context.get(), translationLinkAddr));
        }
        catch (std::runtime_error &ex) {
            utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
            SC_LOG_ERROR(ex.what())
            return SC_RESULT_ERROR;
        }
        ScAddr answer;
        try {
            answer = generateAnswer(param, translationLinkAddr);
        }
        catch (std::runtime_error &ex) {
            SC_LOG_ERROR(ex.what())
            utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
            return SC_RESULT_ERROR;
        }
        SC_LOG_DEBUG("Agent finished")
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
                Keynodes::nrel_sc_text_translation);
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
        if (!ms_context->HelperGenTemplate(scTemplate, templateGenResult, templateParams)) {
            throw std::runtime_error("Unable to generate message");
        }
        ms_context->CreateNode(ScType::NodeConstStruct);
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
                Keynodes::nrel_sc_text_translation);
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
        if (!ms_context->HelperGenTemplate(answerGenerationTemplate, templateResult, templateParam)) {
            throw std::runtime_error("Unable to generate answer.");
        }
        return templateResult["_answer"];
    }

    bool UiJsonTranslationAgent::isComponentValid(ScAddr const &param) {
        bool isValid = false;
        if (utils::CommonUtils::checkType(ms_context.get(), param, ScType::NodeConst)) {
            ScTemplate templ;

            templ.TripleWithRelation(
                    Keynodes::concept_component,
                    ScType::EdgeDCommonVar,
                    ScType::NodeVarClass >> "_component",
                    ScType::EdgeAccessVarPosPerm,
                    Keynodes::nrel_inclusion);
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


}
