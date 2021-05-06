import * as React from 'react';
import * as store from '../store';
import { connect } from 'react-redux';
import { ScAddr } from '@ostis/sc-core';
import { useEffect } from 'react';

import {
    Layout,
    Select,
    Button as Btn,
    Menu,
    Switch,
    Radio,
    Input
} from 'antd';
import { GooglePlusOutlined, PrinterOutlined, ArrowsAltOutlined, LinkOutlined } from '@ant-design/icons';
import {resolveSrv} from "dns";


const { Option } = Select;
const { Header, Footer, Sider, Content } = Layout;
const { Item } = Menu;
const { Group, Button} = Radio;

export interface KnowledgeBaseProps {
    services: store.serv.Services;
}

export const Library = {
    layout: Layout,
    header: Header,
    sider: Sider,
    content: Content,
    footer: Footer,
    select: Select,
    option: Option,
    button: Btn,
    menu: Menu,
    item: Item,
    block: "div",
    switch: Switch,
    p: "p",
    google: GooglePlusOutlined,
    printerIcon: PrinterOutlined,
    arrowsIcon: ArrowsAltOutlined,
    linkIcon: LinkOutlined,
    group: Group,
    btn: Button,
    input: Input
};

function mapStateToProps(state: store.Store): KnowledgeBaseProps {
    return {
        services: state.services,
    };
}

interface UIComponent {
    component: string;
    [propName: string]: string | undefined;
}

interface TInterface {
    model: UIComponent[];
    lib: any;
}

function Interface({ model, lib }: TInterface): JSX.Element | null {
    const iterateProps = (style: Object): any => {
        const semanticProps = {
            textColor: 'color',
            textSize: 'fontSize',
            color: 'backgroundColor',
            font: 'fontFamily',
            width: 'width',
            height: 'height',
        };

        const renameKeys = (keysMap: object, obj: object) =>
            Object.keys(obj).reduce(
                (acc, key) => ({
                    ...acc,
                    // @ts-ignore
                    ...{ [keysMap[key] || key]: obj[key] },
                }),
                {},
            );

        return renameKeys(semanticProps, style);
    };

    // @ts-ignore
    const iterate = (model: UIComponent[]) => {
        return model.map((e) => {
            const { component, text, ...prop } = e;
            const CSS = iterateProps({ ...prop });
            return React.createElement(lib[component], { style: CSS, ...prop }, text);
        });
    };

    const Tree = iterate(model);
    return <>{Tree}</>;
}

export const UiModuleImpl: React.FC<KnowledgeBaseProps> = (props: KnowledgeBaseProps) => {
    const { services } = props;
    let result;
    const loadUiJson = async () => {
        const addrs: ScAddr[] = await services.sc.utils.CallUiJsonTranslationAgent();
        const actionNode: ScAddr = addrs[0];

        const componentNode: ScAddr = await services.sc.utils.SearchComponentNode(actionNode);

        await services.sc.utils.waitFinishedArc(componentNode);
        const jsonLinkContent = await services.sc.utils.getUiJsonLinkContent(componentNode);
        console.log(jsonLinkContent);
        return jsonLinkContent;
    };
    useEffect(() => {
        loadUiJson().then((jsonLinkContent) => {
            console.log('Agent finished');
            result = Interface({ model: [JSON.parse(jsonLinkContent)], lib: Library });
        });
    });

    return result;
};
export const UiModule = connect(mapStateToProps)(UiModuleImpl);
