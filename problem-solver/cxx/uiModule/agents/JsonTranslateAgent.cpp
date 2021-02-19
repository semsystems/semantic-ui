/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "JsonTranslateAgent.hpp"
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

    SC_AGENT_IMPLEMENTATION(JsonTranslateAgent) {

        if (!edgeAddr.IsValid())
            return SC_RESULT_ERROR;
        ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
        ScAddr param = IteratorUtils::getFirstFromSet(ms_context.get(), questionNode);
        if (!param.IsValid())
            return SC_RESULT_ERROR_INVALID_PARAMS;
        if (!componentIsValid(param)) {
            SC_LOG_ERROR("Message processing program, component not found.")
            utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
            return SC_RESULT_ERROR;
        }

        ScAddr messageAddr;
        try {
            messageAddr = getTranslateJson(param);
            generateMessage(messageAddr, param);
            SC_LOG_INFO( CommonUtils::readString(ms_context.get(), messageAddr));
        }
        catch (std::runtime_error &ex) {
            utils::AgentUtils::finishAgentWork(ms_context.get(), param, false);
            SC_LOG_ERROR(ex.what())
            return SC_RESULT_ERROR;
        }
        ScAddr answer;
        try {
            answer = generateAnswer(param, messageAddr);
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

    ScAddr JsonTranslateAgent::generateMessage(ScAddr const &jsonLinkAddr, ScAddr const &componentAddr) {
        ScTemplate scTemplate;

        scTemplate.TripleWithRelation(
                ScType::NodeVar >> "_translation_node",
                ScType::EdgeDCommonVar,
                componentAddr,
                ScType::EdgeAccessVarPosPerm,
                Keynodes::nrel_sc_text_translation);
        scTemplate.Triple(
                "_translation_node",
                ScType::EdgeAccessVarPosPerm,
                jsonLinkAddr);
        scTemplate.Triple(
                Keynodes::format_ui_json,
                ScType::EdgeAccessVarPosPerm,
                jsonLinkAddr);
        ScTemplateParams templateParams;
        ScTemplateGenResult templateGenResult;
        if (!ms_context->HelperGenTemplate(scTemplate, templateGenResult, templateParams)) {
            throw std::runtime_error("Unable to generate message");
        }

        return jsonLinkAddr;
    }

    ScAddr JsonTranslateAgent::generateAnswer(ScAddr const &messageAddr, ScAddr const &linkAddr) {
        ScTemplate searchTemplate;

        searchTemplate.TripleWithRelation(
                ScType::NodeVar >> "_translation_node",
                ScType::EdgeDCommonVar >> "_arc2",
                messageAddr,
                ScType::EdgeAccessVarPosPerm >> "_access_arc",
                Keynodes::nrel_sc_text_translation);
        searchTemplate.Triple(
                "_translation_node",
                ScType::EdgeAccessVarPosPerm >> "_arc",
                linkAddr
                );
        searchTemplate.Triple(
                Keynodes::format_ui_json,
                ScType::EdgeAccessVarPosPerm >> "_arc1",
               linkAddr
              );

        ScTemplateSearchResult searchResult;
        ms_context->HelperSearchTemplate(searchTemplate, searchResult);
        if (searchResult.Size() != 1) {
            throw std::runtime_error("Message not generated.");
        }
        ms_context->CreateNode(ScType::NodeConstStruct);
        ScTemplate answerGenerationTemplate;

        answerGenerationTemplate.Triple(
                ScType::NodeVarStruct >> "_answer",
                ScType::EdgeAccessVarPosPerm,
                searchResult[0]["_translation_node"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                searchResult[0]["_arc2"]);

        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                messageAddr);

        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                searchResult[0]["_arc"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                searchResult[0]["_arc1"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                searchResult[0]["_access_arc"]);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::nrel_sc_text_translation);
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
              linkAddr
              //  searchResult[0]["_link"]
                );
        answerGenerationTemplate.Triple(
                "_answer",
                ScType::EdgeAccessVarPosPerm,
                Keynodes::format_ui_json);

        ScTemplateParams templateParams;
        ScTemplateGenResult templateGenResult;
        if (!ms_context->HelperGenTemplate(answerGenerationTemplate, templateGenResult, templateParams)) {
            throw std::runtime_error("Unable to generate answer.");
        }

        return templateGenResult["_answer"];
    }

    void JsonTranslateAgent::searchPropertyValue(const ScTemplateItemValue &concrete_component) {

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

        for (size_t i = 0; i < searchResult.Size(); i++) {

            ss << "\"" << CommonUtils::readString(ms_context.get(), searchResult[i]["_property_class_name"])
               << "\":\"" <<
               ms_context->HelperGetSystemIdtf(searchResult[i]["_value"])
               << CommonUtils::readString(ms_context.get(), searchResult[i]["_name_nrel"]) << "\",";

        }

    }

    ScAddr JsonTranslateAgent::getTranslateJson(ScAddr const &param) {

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
        String s2;
        ss << "{" << "\"component\":\""
           << CommonUtils::readString(ms_context.get(), searchResult[0]["_name_component"]) << "\",\"text\":\""
           << ms_context->HelperGetSystemIdtf(param) << "\",";
        for (size_t i = 0; i < searchResult.Size(); i++) {
            ss << "\"" << CommonUtils::readString(ms_context.get(), searchResult[i]["_property_class_name"])
               << "\":\"" <<
               CommonUtils::readString(ms_context.get(), searchResult[i]["_property_name"]) << "\",";
        }

        searchPropertyValue(param);
        ss << "}";
        String s;
        s = ss.str();
        s.erase(s.end() - 2, s.end() - 1);

        ScAddr answer;
        answer = ms_context->CreateNode(ScType::LinkConst);
        ms_context->SetLinkContent(answer, ScStreamMakeRead(s));
        ss.str("");
        return answer;
    };


    bool JsonTranslateAgent::componentIsValid(ScAddr const &param) {
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
