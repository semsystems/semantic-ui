import * as React from 'react';
import * as store from '../store';
import { connect } from 'react-redux';
import { ScAddr } from '@ostis/sc-core';
import { useEffect } from 'react';

export interface KnowledgeBaseProps {
    services: store.serv.Services;
}

function mapStateToProps(state: store.Store): KnowledgeBaseProps {
    return {
        services: state.services,
    };
}

export const UiModuleImpl: React.FC<KnowledgeBaseProps> = (props: KnowledgeBaseProps) => {
    const { services } = props;

    const loadUiJson = async () => {
        const addrs: ScAddr[] = await services.sc.utils.CallUiJsonTranslationAgent();
        const actionNode: ScAddr = addrs[0];

        const componentNode: ScAddr = await services.sc.utils.SearchComponentNode(actionNode);

        await services.sc.utils.waitFinishedArc(componentNode);
        const jsonLinkContent = await services.sc.utils.getUiJsonLinkContent(componentNode);
        console.log(jsonLinkContent);
    };
    useEffect(() => {
        loadUiJson().then(() => console.log('Agent finished'));
    });

    return <div></div>;
};
export const UiModule = connect(mapStateToProps)(UiModuleImpl);
