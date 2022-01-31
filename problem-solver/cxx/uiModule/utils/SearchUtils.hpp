// This source file is part of an OSTIS project. For the latest info, see http://ostis.net
// Distributed under the MIT License
// (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)

#include <sc-memory/kpm/sc_agent.hpp>

#pragma once

namespace uiModule {

class SearchUtils {
public:
    static ScAddr getFirstParamFromSet(ScMemoryContext * ms_context, const ScAddr &set);
    static ScAddr getSecondParamFromSet(ScMemoryContext * ms_context, const ScAddr &set);
    static ScAddr getParamFromSet(ScMemoryContext * ms_context, const ScAddr &set, const ScAddr &rrel_param);
};

}