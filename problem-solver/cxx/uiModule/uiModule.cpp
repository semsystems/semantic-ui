/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "uiModule.hpp"
#include "keynodes/keynodes.hpp"
#include "agents/JsonAgent.hpp"

using namespace uiModule;

SC_IMPLEMENT_MODULE(UiModule)

sc_result UiModule::InitializeImpl()
{
  if (!uiModule::Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(JsonAgent)

  return SC_RESULT_OK;
}

sc_result UiModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(JsonAgent)

  return SC_RESULT_OK;
}
