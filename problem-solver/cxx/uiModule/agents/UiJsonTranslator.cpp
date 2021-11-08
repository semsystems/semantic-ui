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

using namespace std;
using namespace utils;
using namespace uiModule;


UiJsonTranslator::UiJsonTranslator(ScMemoryContext *m_context) {
    ms_context = m_context;

};

ScTemplateSearchResult UiJsonTranslator::searchPropertyValue(const ScTemplateItemValue &concrete_component) {

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

ScTemplateSearchResult UiJsonTranslator::searchProperty(const ScTemplateItemValue &concrete_component) {

    ScTemplate scTemplate;

    scTemplate.Triple(
            ScType::NodeVar >> "_prop",
            ScType::EdgeAccessVarPosPerm,
            concrete_component);

    scTemplate.Triple(
            ScType::NodeVar >> "_prop_class",
            ScType::EdgeAccessVarPosPerm,
            "_prop");

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

void UiJsonTranslator::setPropertyValue(ScTemplateSearchResult searchResult, ScTemplateSearchResult result,
                                        ScMemoryContext *get_context) {
    for (size_t i = 0; i < searchResult.Size(); i++) {
        json << "\"" << CommonUtils::readString(get_context, searchResult[i]["_property_class_name"])
             << "\":\"" <<
             ms_context->HelperGetSystemIdtf(searchResult[i]["_value"])
             << "\",";

    }
        for (size_t a = 0; a < result.Size(); a++) {
            json << "\"" << CommonUtils::readString(get_context, result[a]["_prop_class_name"])
                 << "\":\"" <<
                 CommonUtils::readString(get_context, result[a]["_link_value"])

                 << "\",";
        }
};


String UiJsonTranslator::getJson(ScTemplateSearchResult searchResult, const ScAddr &param,
                                 ScMemoryContext *get_context) {
    json.str("");
    json << "{" << "\"component\":\"" <<
         CommonUtils::readString(get_context, searchResult[0]["_name_component"])
         << "\",";
    setPropertyValue(searchPropertyValue(param), searchProperty(param), get_context);


    json << "}";
    String stringJson;
    stringJson = json.str();
    stringJson.erase(stringJson.end() - 2, stringJson.end() - 1);
    return stringJson;
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
            Keynodes::nrel_inclusion);

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
    String stringJson = getJson(searchResult, param, get_context);
    ScAddr answer;
    answer = ms_context->CreateNode(ScType::LinkConst);
    ms_context->SetLinkContent(answer, ScStreamMakeRead(stringJson));

    return answer;
};

