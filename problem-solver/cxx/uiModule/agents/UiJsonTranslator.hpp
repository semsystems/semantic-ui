/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#ifndef SC_MACHINE_UIJSONTRANSLATOR_HPP
#define SC_MACHINE_UIJSONTRANSLATOR_HPP


#include <sc-ui/uiTranslators.h>
#include "UiJsonTranslationAgent.hpp"
#include "keynodes/Keynodes.hpp"

class ITranslator
{

public:
  virtual ScAddr getTranslatedUiJsonLink(ScAddr const & param, ScMemoryContext * context) = 0;

};


class UiJsonTranslator : ITranslator
{
public:
  UiJsonTranslator(ScMemoryContext * ms_context);

  ScMemoryContext * ms_context;
  StringStream json;

  ScTemplateSearchResult searchPropertyNodeValue(const ScTemplateItemValue & concrete_component);
  ScTemplateSearchResult searchPropertyLinkValue(const ScTemplateItemValue & concrete_component);
  ScTemplateSearchResult searchActionValue(const ScTemplateItemValue & concrete_component);

  void setPropertyValue(ScTemplateSearchResult propertyNodeValueSearchResult, ScTemplateSearchResult propertyLinkValueSearchResult, ScMemoryContext * context);
  void setActionValue(ScTemplateSearchResult actionValueSearchResult, ScMemoryContext * context);

  String getJson(ScTemplateSearchResult searchResult, const ScAddr & param, ScMemoryContext * context);

  ScAddr getTranslatedUiJsonLink(ScAddr const & param, ScMemoryContext * context) override;
};

#endif //SC_MACHINE_UIJSONTRANSLATOR_HPP
