/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <test/FileUtils.hpp>
#include "catch2/catch.hpp"
#include "sc-test-framework/sc_test_unit.hpp"
#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-memory/sc_wait.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "keynodes/Keynodes.hpp"
#include "agents/UiJsonTranslationAgent.hpp"
#include "../../../../../ostis-web-platform/sc-machine/tools/builder/src/scs_loader.hpp"


using namespace uiModule;
using namespace utils;

namespace TextUiJsonTranslationAgent
{

ScsLoader loader;
const std::string TEST_FILES_DIR_PATH =
      "../../../problem-solver/cxx/uiModule/test/";
const std::string SC_MEMORY_INI = "sc-memory.ini";
const int WAIT_TIME = 9000;
FileUtils json;

void initialize()
{
  scAgentsCommon::CoreKeynodes::InitGlobal();
  Keynodes::InitGlobal();

  UiJsonTranslationAgent::InitGlobal();
  ScAgentInit(true);
  SC_AGENT_REGISTER(UiJsonTranslationAgent)
}

ScTemplateSearchResult getSearchResult(ScMemoryContext * context, ScAddr const & uiComponentAddr)
{
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
        ScType::LinkVar >> "_link_json"
  );
  scTemplate.Triple(
        Keynodes::format_ui_json,
        ScType::EdgeAccessVarPosPerm >> "_arc1",
        "_link_json"

  );

  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  return searchResult;

}

bool generatedAnswerIsValid(ScMemoryContext * context, ScAddr const & uiComponentAddr)
{

  return getSearchResult(context, uiComponentAddr).Size() == 1;
}


TEST_CASE("Json_without_some_properties", "[ui json translation test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkArguments");

  loader.loadScsFile(
        context,
        TEST_FILES_DIR_PATH + "testStructures/test_button_2.scs");
  initialize();

  ScAddr component = context.HelperFindBySystemIdtf("button");
  REQUIRE(component.IsValid());

  ScAddr test_action_node = context.HelperFindBySystemIdtf("test_action_node");
  REQUIRE(test_action_node.IsValid());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_action_node);

  REQUIRE(ScWaitEvent<ScEventAddOutputEdge>(context,
                                            scAgentsCommon::CoreKeynodes::question_finished_successfully).Wait(
        WAIT_TIME));

  REQUIRE(generatedAnswerIsValid(&context, component));
  string s = json.getFileContent(TEST_FILES_DIR_PATH + "json/test_2.json");
  string ss = CommonUtils::readString(&context, getSearchResult(&context, component)[0]["_link_json"]);
  REQUIRE(ss == s);
  SC_AGENT_UNREGISTER(UiJsonTranslationAgent)
  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
};

TEST_CASE("Json_without_properties", "[ui json translation test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkArguments");
  string s = json.getFileContent(TEST_FILES_DIR_PATH + "json/test_3.json");
  loader.loadScsFile(
        context,
        TEST_FILES_DIR_PATH + "testStructures/test_button_3.scs");
  initialize();

  ScAddr component = context.HelperFindBySystemIdtf("button");
  REQUIRE(component.IsValid());

  ScAddr test_action_node = context.HelperFindBySystemIdtf("test_action_node");
  REQUIRE(test_action_node.IsValid());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_action_node);

  REQUIRE(ScWaitEvent<ScEventAddOutputEdge>(context,
                                            scAgentsCommon::CoreKeynodes::question_finished_successfully).Wait(
        WAIT_TIME));

  string ss = CommonUtils::readString(&context, getSearchResult(&context, component)[0]["_link_json"]);
  REQUIRE(ss == s);
  SC_AGENT_UNREGISTER(UiJsonTranslationAgent)
  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
};

TEST_CASE("Json_with_all_properties", "[ui json translation test]")
{


  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkArguments");
  string s = json.getFileContent(TEST_FILES_DIR_PATH + "json/test_1.json");

  loader.loadScsFile(
        context,
        TEST_FILES_DIR_PATH + "testStructures/test_button_1.scs");
  initialize();

  ScAddr component = context.HelperFindBySystemIdtf("button");
  REQUIRE(component.IsValid());

  ScAddr test_action_node = context.HelperFindBySystemIdtf("test_action_node");
  REQUIRE(test_action_node.IsValid());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_action_node);

  REQUIRE(ScWaitEvent<ScEventAddOutputEdge>(context,
                                            scAgentsCommon::CoreKeynodes::question_finished_successfully).Wait(
        WAIT_TIME));
  string ss = CommonUtils::readString(&context, getSearchResult(&context, component)[0]["_link_json"]);
  REQUIRE(ss == s);
  SC_AGENT_UNREGISTER(UiJsonTranslationAgent)
  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
};

TEST_CASE("Json_with_incorrect_parameter", "[ui json translation test]")
{
  test::ScTestUnit::InitMemory(SC_MEMORY_INI, "");
  ScMemoryContext context(sc_access_lvl_make_min, "checkArguments");

  loader.loadScsFile(
        context,
        TEST_FILES_DIR_PATH + "testStructures/test_button_4.scs");
  initialize();

  ScAddr component = context.HelperFindBySystemIdtf("button");
  REQUIRE(component.IsValid());

  ScAddr test_action_node = context.HelperFindBySystemIdtf("test_action_node");
  REQUIRE(test_action_node.IsValid());

  context.CreateEdge(
        ScType::EdgeAccessConstPosPerm,
        scAgentsCommon::CoreKeynodes::question_initiated,
        test_action_node);

  REQUIRE(ScWaitEvent<ScEventAddOutputEdge>(context,
                                            scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully).Wait(
        WAIT_TIME));;

  SC_AGENT_UNREGISTER(UiJsonTranslationAgent)
  context.Destroy();
  test::ScTestUnit::ShutdownMemory(false);
};
}
