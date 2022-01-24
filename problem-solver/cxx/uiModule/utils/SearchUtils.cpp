// This source file is part of an OSTIS project. For the latest info, see http://ostis.net
// Distributed under the MIT License
// (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)

#include "SearchUtils.hpp"
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

namespace uiModule
{
    ScAddr SearchUtils::getFirstParamFromSet(ScMemoryContext * ms_context, const ScAddr &set)
    {
        return getParamFromSet(ms_context, set, scAgentsCommon::CoreKeynodes::rrel_1);
    }

    ScAddr SearchUtils::getSecondParamFromSet(ScMemoryContext * ms_context, const ScAddr &set)
    {
        return getParamFromSet(ms_context, set, scAgentsCommon::CoreKeynodes::rrel_2);
    }

    ScAddr SearchUtils::getParamFromSet(ScMemoryContext * ms_context, const ScAddr &set, const ScAddr &rrel_param)
    {
        ScTemplate scTemplate;

        scTemplate.TripleWithRelation(
                set,
                ScType::EdgeAccessVarPosPerm,
                ScType::NodeVar >> "param",
                ScType::EdgeAccessVarPosPerm,
                rrel_param);

        ScTemplateSearchResult result;

        ms_context->HelperSearchTemplate(scTemplate, result);

        return result[0]["param"];
    }
}