import { ScAddr, ScConstruction, ScNet, ScTemplate, ScTemplateSearchResult, ScType } from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';
import { ServerBase } from './ServerBase';

export class ServerUtils extends ServerBase {
    constructor(client: ScNet, keynodes: ServerKeynodes) {
        super(client, keynodes);
    }

    public async DoSearch(scsTempl: string): Promise<ScTemplateSearchResult> {
        const searchResult: ScTemplateSearchResult = await this.client.TemplateSearch(scsTempl);

        return new Promise<ScTemplateSearchResult>(function (resolve) {
            resolve(searchResult);
        });
    }

    public async SearchComponentNode(actionNode: ScAddr): Promise<ScAddr> {
        const template: ScTemplate = new ScTemplate();
        template.Triple(actionNode, [ScType.EdgeAccessVarPosPerm, 'edge'], [ScType.NodeVar, 'component']);

        template.Triple(this.keynodes.kRrel1, ScType.EdgeAccessVarPosPerm, 'edge');
        const searchResult: ScTemplateSearchResult = await this.client.TemplateSearch(template);
        const componentAddr: ScAddr = searchResult[0].Get('component');
        return new Promise((resolve) => {
            resolve(componentAddr);
        });
    }

    public async CallUiJsonTranslationAgent(): Promise<ScAddr[]> {
        const questionNodeAlias = 'questionNode';
        const construction: ScConstruction = new ScConstruction();
        construction.CreateNode(ScType.NodeConst, questionNodeAlias);

        const concreteConceptComponentNodeAlias = 'concreteConceptComponentNode';
        construction.CreateNode(ScType.NodeConst, concreteConceptComponentNodeAlias);

        const rrel1EdgeAlias = 'rrel1Edge';

        construction.CreateEdge(ScType.EdgeAccessConstPosPerm, this.keynodes.kQuestion, questionNodeAlias);
        construction.CreateEdge(
            ScType.EdgeAccessConstPosPerm,
            this.keynodes.kActionTranslateToUiJson,
            questionNodeAlias,
        );
        construction.CreateEdge(
            ScType.EdgeAccessConstPosPerm,
            questionNodeAlias,
            this.keynodes.kButton1,
            rrel1EdgeAlias,
        );

        construction.CreateEdge(ScType.EdgeAccessConstPosPerm, this.keynodes.kRrel1, rrel1EdgeAlias);

        construction.CreateEdge(ScType.EdgeAccessConstPosPerm, this.keynodes.kQuestionInitiated, questionNodeAlias);

        const result: ScAddr[] = await this.client.CreateElements(construction);

        return new Promise<ScAddr[]>(function (resolve) {
            resolve(result);
        });
    }
}
