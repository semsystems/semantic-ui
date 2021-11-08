import * as React from 'react';
import ReactDOM from 'react-dom';
import 'antd/dist/antd.css';
import * as store from '../store';
import { connect } from 'react-redux';
import { ScAddr } from '@ostis/sc-core';
import { useEffect, useState } from 'react';
import { nanoid } from 'nanoid';
import { Link } from 'react-router-dom';
import { Layout, Select, Button as Btn, Menu, Switch, Radio, Input, Checkbox, Slider, Image, Progress } from 'antd';
import { GooglePlusOutlined, PrinterOutlined, ArrowsAltOutlined, LinkOutlined } from '@ant-design/icons';

const { Option } = Select;
const { Header, Footer, Sider, Content } = Layout;
const { Item } = Menu;
const { Group, Button } = Radio;

export interface KnowledgeBaseProps {
    services: store.serv.Services;
}

export const Library = {
    header: Header,
    sider: Sider,
    footer: Footer,
    select: Select,
    option: Option,
    Button: Btn,
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
    Link: 'a',
    TextField: Input,
    RadioButton: Radio,
    CheckBox: Checkbox,
    Slider: Slider,
    Image: Image,
    Progress: Progress,
    Label: 'label',
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
            disabled: 'disabled',
            checked: 'checked',
            font: 'fontFamily',
            min: 'min',
            max: 'max',
            value: 'percent',
            width: 'width',
            height: 'height',
            x: 'left',
            y: 'top',
            src: 'src',
            href: 'href',
            alt: 'alt',
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
            // if (prop['disabled'] == 'false') {
            //     //                @ts-ignore
            //     prop['disabled'] = false;
            // }
            CSS['fontSize'] = CSS['fontSize'] + 'pt';
            // CSS['width'] = CSS['width'] + 'dp';
            // CSS['height'] = CSS['height'] + 'dp';
            return React.createElement(lib[component], { style: CSS, key: nanoid(), ...prop }, text);
        });
    };

    const Tree = iterate(model);
    return <div>{Tree}</div>;
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
    console.log(JSON.parse(jsonLinkContent));
    return <div>{jsonLinkContent != '[]' && <Interface model={JSON.parse(jsonLinkContent)} lib={Library} />} </div>;
};
export const UiModule = connect(mapStateToProps)(UiModuleImpl);
