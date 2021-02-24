import { ScNet, ScTemplateSearchResult } from '@ostis/sc-core';
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
}
