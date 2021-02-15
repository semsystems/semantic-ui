import { ScAddr, ScLinkContent, ScLinkContentType, ScNet, ScTemplate, ScTemplateResult, ScType } from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';

type CallbackIdentifierFunction = (addr: ScAddr, idtf: string) => void;

export class ServerBase {
    private readonly _client: ScNet = null;
    private readonly _keynodes: ServerKeynodes = null;

    constructor(client: ScNet, keynodes: ServerKeynodes) {
        this._client = client;
        this._keynodes = keynodes;
    }

    public get client(): ScNet {
        return this._client;
    }

    public get keynodes(): ServerKeynodes {
        return this._keynodes;
    }

    public get host(): string {
        return location.protocol + '//' + location.hostname + (location.port ? ':' + location.port : '');
    }

    public blobToBase64(blob: Blob): Promise<string> {
        const reader = new FileReader();
        reader.readAsDataURL(blob);
        return new Promise<string>((resolve) => {
            reader.onloadend = () => {
                if (typeof reader.result === 'string') {
                    resolve(reader.result.split(',')[1]);
                } else resolve('');
            };
        });
    }

    public GetContentURL(addr: ScAddr): string {
        return `http://${this.host}/content/${addr.value}`;
    }

    public async GetSystemIdentifier(
        addrs: ScAddr[],
        callback: CallbackIdentifierFunction = null,
    ): Promise<{ [key: number]: string }> {
        const result: { [key: number]: string } = {};

        for (const a of addrs) {
            const templ: ScTemplate = new ScTemplate();

            templ.TripleWithRelation(
                a,
                ScType.EdgeDCommonVar,
                [ScType.LinkVar, '_link'],
                ScType.EdgeAccessVarPosPerm,
                this.keynodes.kNrelSystemIdentifier,
            );

            const searchResult: ScTemplateResult[] = await this.client.TemplateSearch(templ);
            if (searchResult.length > 0) {
                const linkAddr: ScAddr = searchResult[0].Get('_link');
                const content: ScLinkContent[] = await this.client.GetLinkContents([linkAddr]);

                if (content[0].type != ScLinkContentType.String) {
                    throw 'System identifier should be a string';
                }

                const idtf: string = content[0].data as string;
                result[a.value] = idtf;
                if (callback) {
                    callback(linkAddr, idtf);
                }
            }
        }

        return new Promise<{ [key: number]: string }>((resolve) => {
            resolve(result);
        });
    }
}
