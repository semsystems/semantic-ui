import * as React from 'react';
import { useEffect, useState } from 'react';
import 'antd/dist/antd.css';
import * as store from '../store';
import { connect } from 'react-redux';
import { ScAddr } from '@ostis/sc-core';
import { nanoid } from 'nanoid';
import { Button as Btn, Checkbox, Image, Input, Layout, Menu, Progress, Radio, Select, Slider, Switch } from 'antd';
import { ArrowsAltOutlined, GooglePlusOutlined, LinkOutlined, PrinterOutlined } from '@ant-design/icons';

const { Option } = Select;
const { Header, Footer, Sider } = Layout;
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

interface UIComponentProp {
    component: string;
    [propName: string]: string | undefined;
}

type UIComponent = UIComponentProp & {
    actions: string[];
};

interface TInterface {
    model: UIComponent[];
    componentAdr: ScAddr;
    lib: any;
    kbProps: KnowledgeBaseProps;
}

function Interface({ model, componentAdr, lib, kbProps }: TInterface): JSX.Element | null {
    const { services } = kbProps;

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

    const iterateActions = (actions: string[]): any => {
        const semanticActionNames = {};

        const handleActionWithoutParams = async (actionName: string) => {
            const actionAdr = await services.sc.utils.SearchUiClassByJsonIdtf(actionName);
            await services.sc.utils.CallUiActionProcessingAgent(componentAdr, actionAdr);
            console.log(actionName);
        };

        const mapEvents = (actionNameMap: unknown, actionArray: string[]) => {
            const eventMap = {};
            for (const actionName of actionArray) {
                let event;
                switch (actionName) {
                    default:
                        event = () => handleActionWithoutParams(actionName);
                }
                eventMap[actionNameMap[actionName] || actionName] = event;
            }
            return eventMap;
        };

        return mapEvents(semanticActionNames, actions);
    };

    const iterate = (model: UIComponent[]) => {
        return model.map((e) => {
            const { component, actions, text, ...prop } = e;
            const CSS = iterateProps({ ...prop });
            const events = iterateActions(actions);
            // if (prop['disabled'] == 'false') {
            //     //                @ts-ignore
            //     prop['disabled'] = false;
            // }
            CSS['fontSize'] = CSS['fontSize'] + 'pt';
            // CSS['width'] = CSS['width'] + 'dp';
            // CSS['height'] = CSS['height'] + 'dp';
            return React.createElement(
                lib[component],
                {
                    style: CSS,
                    key: nanoid(),
                    ...prop,
                    ...events,
                },
                text,
            );
        });
    };

    const Tree = iterate(model);
    return <div>{Tree}</div>;
}

export const UiModuleImpl: React.FC<KnowledgeBaseProps> = (props: KnowledgeBaseProps) => {
    const { services } = props;
    const [jsonLinkContent, setJsonLinkContent] = useState('[]');
    const [componentAdr, setComponentAdr] = useState(new ScAddr());
    const loadUiJson = async () => {
        const addrs: ScAddr[] = await services.sc.utils.CallUiJsonTranslationAgent();
        const actionNode: ScAddr = addrs[0];

        const componentNode: ScAddr = await services.sc.utils.SearchComponentNode(actionNode);

        await services.sc.utils.waitFinishedArc(componentNode);
        const json = await services.sc.utils.getUiJsonLinkContent(componentNode);
        console.log(json);

        return { componentNode, json };
    };
    useEffect(() => {
        loadUiJson().then((value) => {
            setJsonLinkContent('[' + value.json + ']');
            setComponentAdr(value.componentNode);
        });
    }, []);
    console.log(JSON.parse(jsonLinkContent));
    return (
        <div>
            {jsonLinkContent != '[]' && (
                <Interface
                    model={JSON.parse(jsonLinkContent)}
                    componentAdr={componentAdr}
                    lib={Library}
                    kbProps={props}
                />
            )}{' '}
        </div>
    );
};
export const UiModule = connect(mapStateToProps)(UiModuleImpl);
