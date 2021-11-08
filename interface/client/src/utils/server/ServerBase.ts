import {
    ScAddr,
    ScLinkContent,
    ScLinkContentType,
    ScNet,
    ScTemplate,
    ScTemplateResult,
    ScTemplateSearchResult,
    ScType,
} from '@ostis/sc-core';
import { ServerKeynodes } from './ServerKeynodes';

type CallbackIdentifierFunction = (addr: ScAddr, idtf: string) => void;

export class ServerBase {
    private readonly _client: ScNet = null;
    private readonly _keynodes: ServerKeynodes = null;
    private readonly template_search_iterations_count: number = +process.env.TEMPLATE_SEARCH_ITERATIONS_COUNT;
    private readonly template_search_waiting_ms_interval: number = +process.env.TEMPLATE_SEARCH_WAITING_MS_INTERVAL;

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

    public async findFinishedArc(actionNode: ScAddr): Promise<ScTemplateSearchResult> {
        const template: ScTemplate = new ScTemplate();

        template.Triple(this.keynodes.kQuestionFinished, ScType.EdgeAccessVarPosPerm, actionNode);
        const resultFinishedArc = await this.client.TemplateSearch(template);

        return new Promise((resolve) => {
            resolve(resultFinishedArc);
        });
    }

    public async findAnswerConstruction(actionNode: ScAddr): Promise<ScTemplateSearchResult> {
        const template: ScTemplate = new ScTemplate();
        template.TripleWithRelation(
            actionNode,
            ScType.EdgeDCommonVar,
            [ScType.NodeVarStruct, 'structure'],
            ScType.EdgeAccessVarPosPerm,
            this.keynodes.kNrelAnswer,
        );
        template.TripleWithRelation(
            [ScType.NodeVar, 'node'],
            ScType.EdgeDCommonVar,
            [ScType.NodeVar, 'componentNode'],
            ScType.EdgeAccessVarPosPerm,
            this.keynodes.kNrelScTextTranslation,
        );
        template.Triple('structure', ScType.EdgeAccessVarPosPerm, 'componentNode');
        const resultAnswer: ScTemplateSearchResult = await this.client.TemplateSearch(template);

        const structAddr: ScAddr = await resultAnswer[0].Get('structure');
        const componentAddr: ScAddr = await resultAnswer[0].Get('componentNode');
        const nodeAddr: ScAddr = await resultAnswer[0].Get('node');
        const templ: ScTemplate = new ScTemplate();
        templ.Triple(structAddr, ScType.EdgeAccessVarPosPerm, [componentAddr, 'component']);

        templ.Triple(this.keynodes.kFormatUiJson, ScType.EdgeAccessVarPosPerm, [ScType.LinkVar, 'ui_json_link']);
        templ.Triple(nodeAddr, ScType.EdgeAccessVarPosPerm, 'ui_json_link');
        const result: ScTemplateSearchResult = await this.client.TemplateSearch(templ);

        return new Promise((resolve) => {
            resolve(result);
        });
    }

    public async findAnswerLink(actionNode: ScAddr): Promise<ScAddr> {
        const template: ScTemplate = new ScTemplate();
        template.TripleWithRelation(
            [ScType.NodeVar, 'translation_node'],
            ScType.EdgeDCommonVar,
            actionNode,
            ScType.EdgeAccessVarPosPerm,
            this.keynodes.kNrelScTextTranslation,
        );
        template.Triple('translation_node', ScType.EdgeAccessVarPosPerm, [ScType.LinkVar, 'link']);

        template.Triple(this.keynodes.kFormatUiJson, ScType.EdgeAccessVarPosPerm, 'link');

        const templateResult: ScTemplateSearchResult = await this.client.TemplateSearch(template);
        const linkAddr: ScAddr = await templateResult[0].Get('link');

        return new Promise((resolve) => {
            resolve(linkAddr);
        });
    }

    public async findLinkContent(link: ScAddr): Promise<string> {
        const linkContent: ScLinkContent[] = await this.client.GetLinkContents([link]);
        const contentResult: string = linkContent[0].data.toString();
        return new Promise<string>((resolve) => {
            resolve(contentResult);
        });
    }

    public async waitFinishedArc(actionNode: ScAddr): Promise<void> {
        let iterator = 0;
        const timerForFinishedArc = setInterval(async () => {
            const resultFinishedArc: ScTemplateSearchResult = await this.findFinishedArc(actionNode);
            iterator++;
            if (resultFinishedArc.length != 0 || iterator > this.template_search_iterations_count) {
                clearInterval(timerForFinishedArc);
            }
        }, this.template_search_waiting_ms_interval);
    }

    public async getUiJsonLinkContent(actionNode: ScAddr): Promise<string> {
        let linkNodeContent = '';
        let iterator = 0;
        let resolveFunction;
        const timerForAnswerConstruction = setInterval(async () => {
            const answerMessageNodeConstruction: ScTemplateSearchResult = await this.findAnswerConstruction(actionNode);
            iterator++;
            if (answerMessageNodeConstruction.length > 0) {
                clearInterval(timerForAnswerConstruction);
                const answerNode: ScAddr = await answerMessageNodeConstruction[0].Get('component');
                const linkNode: ScAddr = await this.findAnswerLink(answerNode);
                linkNodeContent = await this.findLinkContent(linkNode);
                resolveFunction(linkNodeContent);
            } else if (iterator > this.template_search_iterations_count) {
                clearInterval(timerForAnswerConstruction);
                resolveFunction(linkNodeContent);
            }
        }, this.template_search_waiting_ms_interval);
        return new Promise<string>((resolve) => {
            resolveFunction = resolve;
        });
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
                const linkAddr: ScAddr = await searchResult[0].Get('_link');
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
