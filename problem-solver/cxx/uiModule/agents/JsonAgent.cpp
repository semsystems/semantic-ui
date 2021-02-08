/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "JsonAgent.hpp"
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
    SC_AGENT_IMPLEMENTATION(JsonAgent) {


        if (!edgeAddr.IsValid())
            return SC_RESULT_ERROR;
        ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
        ScAddr param = IteratorUtils::getFirstFromSet(ms_context.get(), questionNode);
        if (!param.IsValid())
            return SC_RESULT_ERROR_INVALID_PARAMS;

        getJson(param);

    }

    void JsonAgent::searchProperty(const ScTemplateItemValue &concrete_component) {
        ScTemplate templ;
        ScTemplateSearchResult searchResult;
        templ.Triple(
                ScType::NodeVarClass >> "_property",
                ScType::EdgeAccessVarPosPerm,
                concrete_component);
        templ.Triple(
                ScType::NodeVarClass >> "_property_class",
                ScType::EdgeAccessVarPosPerm,
                "_property");

        templ.TripleWithRelation(
                "_property",
                ScType::EdgeDCommonVar,
                ScType::LinkVar >> "_value",
                ScType::EdgeAccessVarPosPerm,
                ScType::NodeVarNoRole >> "_nrel");

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
               CommonUtils::readString(ms_context.get(), searchResult[i]["_value"]) << "\",";

        }
    }

    void JsonAgent::searchPropertyValue(const ScTemplateItemValue &concrete_component) {

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

    void JsonAgent::getJson(ScAddr const &param) {

        ScTemplate templ;

        templ.TripleWithRelation(
                Keynodes::concept_component,
                ScType::EdgeDCommonVar,
                ScType::NodeVar >> "_component",//concept_button
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
        SC_LOG_INFO(s);

    };


}
