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


UiJsonTranslator::UiJsonTranslator(ScMemoryContext * m_context)
{
  ms_context = m_context;

};

ScTemplateSearchResult UiJsonTranslator::searchPropertyValue(const ScTemplateItemValue & concrete_component)
{

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

void UiJsonTranslator::setPropertyValue(ScTemplateSearchResult searchResult, ScMemoryContext * get_context)
{

  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    json << "\"" << CommonUtils::readString(get_context, searchResult[i]["_property_class_name"])
         << "\":\"" <<
         ms_context->HelperGetSystemIdtf(searchResult[i]["_value"])
         << CommonUtils::readString(get_context, searchResult[i]["_name_nrel"]) << "\",";
  }
};

String UiJsonTranslator::getJson(ScTemplateSearchResult searchResult, const ScAddr & param,
                                 ScMemoryContext * get_context)
{
  json.str("");
  json << "{" << "\"component\":\""
       << CommonUtils::readString(get_context, searchResult[0]["_name_component"]) << "\",\"text\":\""
       << ms_context->HelperGetSystemIdtf(param) << "\",";

  setPropertyValue(searchPropertyValue(param), get_context);
  json << "}";
  String stringJson;
  stringJson = json.str();
  stringJson.erase(stringJson.end() - 2, stringJson.end() - 1);
  return stringJson;
};

ScAddr UiJsonTranslator::getTranslatedUiJsonLink(ScAddr const & param, ScMemoryContext * get_context)
{

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
  String stringJson = getJson(searchResult, param, get_context);
  ScAddr answer;
  answer = ms_context->CreateNode(ScType::LinkConst);
  ms_context->SetLinkContent(answer, ScStreamMakeRead(stringJson));

  return answer;
};

