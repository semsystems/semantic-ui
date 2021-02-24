import { ResolveIdtfMap, ScAddr, ScIdtfResolveParams, ScNet, ScType } from '@ostis/sc-core';

const sNrelSystemIdentifier = 'nrel_system_identifier';
const sQuestion = 'question';
const sQuestionInitiated = 'question_initiated';
const sLangEn = 'lang_en';
const sNrelAuthors = 'nrel_authors';
const sRrel1 = 'rrel_1';

// Test
const sTestUser = 'test_user';

export class ServerKeynodes {
    private _client: ScNet = null;

    private _kNrelSystemIdentifier: ScAddr = null;
    private _kQuestion: ScAddr = null;
    private _kQuestionInitiated: ScAddr = null;
    private _kActionReplyToMessage: ScAddr = null;
    private _kLangEn: ScAddr = null;
    private _kNrelAuthors: ScAddr = null;
    private _kRrel1: ScAddr = null;

    // test
    private _kTestUser: ScAddr = null;

    constructor(client: ScNet) {
        this._client = client;
    }

    public async Initialize(): Promise<boolean> {
        const self = this;
        return new Promise<boolean>(function (resolve) {
            const keynodesList: ScIdtfResolveParams[] = [
                { idtf: sNrelSystemIdentifier, type: ScType.Unknown },
                { idtf: sQuestion, type: ScType.Unknown },
                { idtf: sQuestionInitiated, type: ScType.Unknown },
                { idtf: sLangEn, type: ScType.Unknown },
                { idtf: sNrelAuthors, type: ScType.Unknown },
                { idtf: sRrel1, type: ScType.Unknown },
                { idtf: sTestUser, type: ScType.NodeConst },
            ];

            self._client.ResolveKeynodes(keynodesList).then(function (res: ResolveIdtfMap) {
                self._kNrelSystemIdentifier = res[sNrelSystemIdentifier];
                self._kQuestion = res[sQuestion];
                self._kQuestionInitiated = res[sQuestionInitiated];
                self._kLangEn = res[sLangEn];
                self._kNrelAuthors = res[sNrelAuthors];
                self._kRrel1 = res[sRrel1];
                self._kTestUser = res[sTestUser];

                let resValue = true;
                for (let i = 0; i < keynodesList.length; ++i) {
                    const idtf: string = keynodesList[i].idtf;
                    const addr: ScAddr = res[idtf];
                    console.log(`Resolve keynode ${idtf} = ${addr.value}`);

                    resValue = resValue && addr.isValid();
                }

                resolve(resValue);
            });
        });
    }

    get kNrelSystemIdentifier(): ScAddr {
        return this._kNrelSystemIdentifier;
    }

    public get kQuestion(): ScAddr {
        return this._kQuestion;
    }

    public get kQuestionInitiated(): ScAddr {
        return this._kQuestionInitiated;
    }

    get kLangEn(): ScAddr {
        return this._kLangEn;
    }

    get kNrelAuthors(): ScAddr {
        return this._kNrelAuthors;
    }

    get kRrel1(): ScAddr {
        return this._kRrel1;
    }

    public get kTestUser(): ScAddr {
        return this._kTestUser;
    }
}
