/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "UiJsonTranslationAgent.hpp"
#include "UiJsonTranslator.hpp"
#include "keynodes/Keynodes.hpp"
#include <sc-memory/sc_memory.hpp>
#include <sc-ui/uiPrecompiled.h>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

using namespace std;
using namespace utils;
using namespace uiModule;


UiJsonTranslator::UiJsonTranslator(ScMemoryContext *m_context) {
    ms_context = m_context;

};

ScTemplateSearchResult UiJsonTranslator::searchPropertyNodeValue(const ScTemplateItemValue &concrete_component) {

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
            "_property_class",
            ScType::EdgeDCommonVar,
            ScType::LinkVar >> "_property_class_name",
            ScType::EdgeAccessVarPosPerm,
            Keynodes::keynode_nrel_json_idtf);

    ms_context->HelperSearchTemplate(templ, searchResult);
    return searchResult;
};

ScTemplateSearchResult UiJsonTranslator::searchPropertyLinkValue(const ScTemplateItemValue &concrete_component) {

    ScTemplate scTemplate;

    scTemplate.Triple(
            ScType::NodeVar >> "_prop",
            ScType::EdgeAccessVarPosPerm,
            concrete_component);
    scTemplate.Triple(
            ScType::NodeVar >> "_prop_class",
            ScType::EdgeAccessVarPosPerm,
            "_prop");
    scTemplate.Triple(
            Keynodes::concept_properties_set,
            ScType::EdgeAccessVarPosPerm,
            "_prop_class");

    scTemplate.TripleWithRelation(
            "_prop",
            ScType::EdgeDCommonVar,
            ScType::LinkVar >> "_link_value",
            ScType::EdgeAccessVarPosPerm,
            Keynodes::keynode_nrel_json_idtf);
    scTemplate.TripleWithRelation(
            "_prop_class",
            ScType::EdgeDCommonVar,
            ScType::LinkVar >> "_prop_class_name",
            ScType::EdgeAccessVarPosPerm,
            Keynodes::keynode_nrel_json_idtf);
    ScTemplateSearchResult result;

    ms_context->HelperSearchTemplate(scTemplate, result);
    return result;
};

void UiJsonTranslator::setPropertyValue(ScTemplateSearchResult propertyNodeValueSearchResult,
                                        ScTemplateSearchResult propertyLinkValueSearchResult,
                                        ScMemoryContext *get_context) {
    for (size_t i = 0; i < propertyNodeValueSearchResult.Size(); i++) {
        json << "\"" <<
             CommonUtils::readString(get_context, propertyNodeValueSearchResult[i]["_property_class_name"])
             << "\":\"" <<
             ms_context->HelperGetSystemIdtf(propertyNodeValueSearchResult[i]["_value"])
             << "\",";
    }
    for (size_t a = 0; a < propertyLinkValueSearchResult.Size(); a++) {
        json << "\"" <<
             CommonUtils::readString(get_context, propertyLinkValueSearchResult[a]["_prop_class_name"])
             << "\":\"" <<
             CommonUtils::readString(get_context, propertyLinkValueSearchResult[a]["_link_value"])
             << "\",";
    }
};

String UiJsonTranslator::getJson(ScTemplateSearchResult searchResult, const ScAddr &param,
                                 ScMemoryContext *get_context) {
    json.str("");
    json << "{" << "\"component\":\"" <<
         CommonUtils::readString(get_context, searchResult[0]["_name_component"])
         << "\",";

    ScTemplateSearchResult searchPropertyNodeValueResult = searchPropertyNodeValue(param);
    ScTemplateSearchResult searchPropertyLinkValueResult = searchPropertyLinkValue(param);
    if (!searchPropertyNodeValueResult.IsEmpty() || !searchPropertyLinkValueResult.IsEmpty())
    {
        setPropertyValue(searchPropertyNodeValueResult, searchPropertyLinkValueResult, get_context);
    }

    json.seekp(-1, StringStream::cur);

    ScTemplateSearchResult searchActionValueResult = searchActionValue(param);
    if (!searchActionValueResult.IsEmpty())
    {
        json << "," << "\"actions\":" << "[";
        setActionValue(searchActionValueResult, get_context);
        json.seekp(-1, StringStream::cur);
        json << "]";
    }

    json << "}";
    return json.str();
};

ScAddr UiJsonTranslator::getTranslatedUiJsonLink(ScAddr const &param, ScMemoryContext *get_context) {
    ScTemplate templ;

    templ.Triple(
            ScType::NodeVarClass >> "_component",
            ScType::EdgeAccessVarPosPerm,
            param
    );

    templ.TripleWithRelation(
            ScType::NodeVarClass >> "_component_class",
            ScType::EdgeDCommonVar,
            "_component",
            ScType::EdgeAccessVarPosPerm,
            scAgentsCommon::CoreKeynodes::nrel_inclusion);

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
    templ.Triple(
            Keynodes::concept_properties_set,
            ScType::EdgeAccessVarPosPerm,
            "_property_class");

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
    String stringJson = getJson(searchResult, param, get_context);
    ScAddr answer;
    answer = ms_context->CreateNode(ScType::LinkConst);
    ms_context->SetLinkContent(answer, ScStreamMakeRead(stringJson));

    return answer;
}

void UiJsonTranslator::setActionValue(ScTemplateSearchResult actionValueSearchResult, ScMemoryContext *get_context) {
    for (size_t a = 0; a < actionValueSearchResult.Size(); a++) {
        json << "\""
             << CommonUtils::readString(get_context, actionValueSearchResult[a]["_action_class_name"])
             << "\",";
    }
}

ScTemplateSearchResult UiJsonTranslator::searchActionValue(const ScTemplateItemValue &concrete_component) {
    ScTemplate scTemplate;

    scTemplate.TripleWithRelation(
            ScType::NodeVar >> "_tuple",
            ScType::EdgeAccessVarPosPerm,
            concrete_component,
            ScType::EdgeAccessVarPosPerm,
            scAgentsCommon::CoreKeynodes::rrel_1);

    scTemplate.TripleWithRelation(
            "_tuple",
            ScType::EdgeAccessVarPosPerm,
            ScType::NodeVarClass >> "_action_class",
            ScType::EdgeAccessVarPosPerm,
            scAgentsCommon::CoreKeynodes::rrel_2);
    scTemplate.TripleWithRelation(
            "_action_class",
            ScType::EdgeDCommonVar,
            ScType::LinkVar >> "_action_class_name",
            ScType::EdgeAccessVarPosPerm,
            Keynodes::keynode_nrel_json_idtf);
    /*
    scTemplate.TripleWithRelation(
            Keynodes::concept_interface_user_action,
            ScType::EdgeDCommonVar,
            "_action_class",
            ScType::EdgeAccessVarPosPerm,
            scAgentsCommon::CoreKeynodes::nrel_inclusion);
            */

    scTemplate.TripleWithRelation(
            "_tuple",
            ScType::EdgeDCommonVar,
            ScType::NodeVarClass >> "_sys_internal_action_class",
            ScType::EdgeAccessVarPosPerm,
            Keynodes::nrel_action_initiated_by_ui);
    /*
    scTemplate.TripleWithRelation(
            Keynodes::action_in_sc_memory,
            ScType::EdgeDCommonVar,
            "_sys_internal_action_class",
            ScType::EdgeAccessVarPosPerm,
            scAgentsCommon::CoreKeynodes::nrel_inclusion);
            */

    ScTemplateSearchResult result;

    ms_context->HelperSearchTemplate(scTemplate, result);
    return result;
};

