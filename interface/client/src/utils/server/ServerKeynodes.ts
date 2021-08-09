import { ResolveIdtfMap, ScAddr, ScIdtfResolveParams, ScNet, ScType } from '@ostis/sc-core';

const sNrelSystemIdentifier = 'nrel_system_identifier';
const sNrelScTextTranslation = 'nrel_sc_text_translation';
const sQuestion = 'question';
const sQuestionInitiated = 'question_initiated';
const sQuestionFinished = 'question_finished';
const sLangEn = 'lang_en';
const sNrelInclusion = 'nrel_inclusion';
const sRrel1 = 'rrel_1';
const sNrelAnswer = 'nrel_answer';
const sConceptComponent = 'concept_atomic_component_of_user_interface';
const sActionTranslateToUiJson = 'action_translate_to_ui_json';
const sFormatUiJson = 'format_ui_json';
const sButton1 = 'button1';

// Test
const sTestUser = 'test_user';

export class ServerKeynodes {
    private _client: ScNet = null;

    private _kNrelSystemIdentifier: ScAddr = null;
    private _kNrelScTextTranslation: ScAddr = null;
    private _kQuestion: ScAddr = null;
    private _kQuestionInitiated: ScAddr = null;
    private _kQuestionFinished: ScAddr = null;
    private _kActionTranslateToUiJson: ScAddr = null;
    private _kConceptComponent: ScAddr = null;
    private _kLangEn: ScAddr = null;
    private _kNrelInclusion: ScAddr = null;
    private _kRrel1: ScAddr = null;
    private _kNrelAnswers: ScAddr = null;
    private _kFormatUiJson: ScAddr = null;
    private _kButton1: ScAddr = null;
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
                { idtf: sQuestionFinished, type: ScType.Unknown },
                { idtf: sNrelScTextTranslation, type: ScType.Unknown },
                { idtf: sLangEn, type: ScType.Unknown },
                { idtf: sNrelInclusion, type: ScType.Unknown },
                { idtf: sRrel1, type: ScType.Unknown },
                { idtf: sNrelAnswer, type: ScType.Unknown },
                { idtf: sTestUser, type: ScType.NodeConst },
                { idtf: sConceptComponent, type: ScType.NodeConst },
                { idtf: sActionTranslateToUiJson, type: ScType.Unknown },
                { idtf: sFormatUiJson, type: ScType.Unknown },
                { idtf: sButton1, type: ScType.Unknown },
            ];

            self._client.ResolveKeynodes(keynodesList).then(function (res: ResolveIdtfMap) {
                self._kNrelSystemIdentifier = res[sNrelSystemIdentifier];
                self._kQuestion = res[sQuestion];
                self._kQuestionInitiated = res[sQuestionInitiated];
                self._kQuestionFinished = res[sQuestionFinished];
                self._kLangEn = res[sLangEn];
                self._kNrelInclusion = res[sNrelInclusion];
                self._kRrel1 = res[sRrel1];
                self._kNrelAnswers = res[sNrelAnswer];
                self._kTestUser = res[sTestUser];
                self._kConceptComponent = res[sConceptComponent];
                self._kActionTranslateToUiJson = res[sActionTranslateToUiJson];
                self._kNrelScTextTranslation = res[sNrelScTextTranslation];
                self._kFormatUiJson = res[sFormatUiJson];
                self._kButton1 = res[sButton1];

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

    public get kQuestionFinished(): ScAddr {
        return this._kQuestionFinished;
    }

    get kLangEn(): ScAddr {
        return this._kLangEn;
    }

    get kNrelInclusion(): ScAddr {
        return this._kNrelInclusion;
    }

    get kRrel1(): ScAddr {
        return this._kRrel1;
    }

    public get kNrelAnswer(): ScAddr {
        return this._kNrelAnswers;
    }

    public get kTestUser(): ScAddr {
        return this._kTestUser;
    }

    public get kConceptComponent(): ScAddr {
        return this._kConceptComponent;
    }

    public get kActionTranslateToUiJson(): ScAddr {
        return this._kActionTranslateToUiJson;
    }

    public get kNrelScTextTranslation(): ScAddr {
        return this._kNrelScTextTranslation;
    }

    public get kFormatUiJson(): ScAddr {
        return this._kFormatUiJson;
    }

    public get kButton1(): ScAddr {
        return this._kButton1;
    }
}
