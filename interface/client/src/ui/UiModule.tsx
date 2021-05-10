import * as React from 'react';
import * as store from '../store';
import { connect } from 'react-redux';
import { ScAddr } from '@ostis/sc-core';
import { useEffect, useState } from 'react';
import { nanoid } from 'nanoid';

import { Layout, Select, Button as Btn, Menu, Switch, Radio, Input } from 'antd';
import { GooglePlusOutlined, PrinterOutlined, ArrowsAltOutlined, LinkOutlined } from '@ant-design/icons';

const { Option } = Select;
const { Header, Footer, Sider, Content } = Layout;
const { Item } = Menu;
const { Group, Button } = Radio;

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
    block: 'div',
    switch: Switch,
    p: 'p',
    google: GooglePlusOutlined,
    printerIcon: PrinterOutlined,
    arrowsIcon: ArrowsAltOutlined,
    linkIcon: LinkOutlined,
    group: Group,
    btn: Button,
    input: Input,
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
    const iterateProps = (style: unknown): any => {
        const semanticProps = {
            textColor: 'color',
            textSize: 'fontSize',
            color: 'backgroundColor',
            font: 'fontFamily',
            width: 'width',
            height: 'height',
        };

        const renameKeys = (keysMap: unknown, obj: unknown) =>
            Object.keys(obj).reduce(
                (acc, key) => ({
                    ...acc,
                    ...{ [keysMap[key] || key]: obj[key] },
                }),
                {},
            );

        return renameKeys(semanticProps, style);
    };

    const iterate = (model: UIComponent[]) => {
        return model.map((e) => {
            const { component, text, ...prop } = e;
            const CSS = iterateProps({ ...prop });
            return React.createElement(lib[component], { style: CSS, key: nanoid(), ...prop }, text);
        });
    };

    const Tree = iterate(model);
    return <div key={nanoid()}>{Tree}</div>;
}

export const UiModuleImpl: React.FC<KnowledgeBaseProps> = (props: KnowledgeBaseProps) => {
    const { services } = props;
    const [jsonLinkContent, setJsonLinkContent] = useState('[]');
    const loadUiJson = async () => {
        const addrs: ScAddr[] = await services.sc.utils.CallUiJsonTranslationAgent();
        const actionNode: ScAddr = addrs[0];

        const componentNode: ScAddr = await services.sc.utils.SearchComponentNode(actionNode);

        await services.sc.utils.waitFinishedArc(componentNode);
        const json = await services.sc.utils.getUiJsonLinkContent(componentNode);
        console.log(json);
        return json;
    };
    useEffect(() => {
        loadUiJson().then((json) => {
            setJsonLinkContent('[' + json + ']');
        });
    });
    return <div>{jsonLinkContent != '[]' && <Interface model={JSON.parse(jsonLinkContent)} lib={Library} />} </div>;
};
export const UiModule = connect(mapStateToProps)(UiModuleImpl);
