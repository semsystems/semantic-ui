import { ScNet } from '@ostis/sc-core';

import { ServerKeynodes } from './ServerKeynodes';
import { ServerUtils } from './ServerUtils';

import * as store from '../../store';
import { net } from '../../store';

import * as redux from 'redux';

export class ServerRoot {
    private _client: ScNet = null;
    private _url = '';
    private _store: redux.Store<store.Store> = null;

    private _serverKeynodes: ServerKeynodes = null;
    private _serverUtils: ServerUtils = null;

    /** Constructor
     * @param url URL to websocket service
     * @param store Redux store instance
     */
    constructor(url: string, store: redux.Store<store.Store>) {
        this._url = url;
        this._store = store;
    }

    public Start(): void {
        this.NotifyChangeInitState('Connecting');
        this.NotifyChangeNetworkState(store.net.State.Connecting);
        this._client = new ScNet(
            this._url,
            this.OnConnected.bind(this),
            this.OnDisconnected.bind(this),
            this.OnError.bind(this),
        );

        this._serverKeynodes = new ServerKeynodes(this._client);
        this._serverUtils = new ServerUtils(this._client, this._serverKeynodes);

        this._store.dispatch(
            store.actions.serv.Init({
                sc: this,
            }),
        );
    }

    private NotifyChangeInitState(text: string) {
        this._store.dispatch(store.actions.ui.ChangeInitMessage(text));
    }

    private NotifyChangeNetworkState(newState: net.State) {
        this._store.dispatch(store.actions.net.changeNetState(newState));
    }

    private OnConnected() {
        this.NotifyChangeNetworkState(net.State.Connected);
        this.Initialize();
    }

    private OnDisconnected() {
        this.NotifyChangeNetworkState(net.State.Disconnected);
    }

    private OnError() {}

    private async Initialize(): Promise<boolean> {
        const self = this;

        async function delay() {
            return new Promise<void>((resolve) => {
                setTimeout(resolve, 1000);
            });
        }

        this.NotifyChangeInitState('Initialize keynodes');
        const result: boolean = await self._serverKeynodes.Initialize();
        await delay();

        this.NotifyChangeInitState('Preparing demo');
        await delay();

        return new Promise<boolean>(function (resolve) {
            self._store.dispatch(store.actions.ui.ChangeUIMode(store.ui.Mode.MainUI));
            resolve(result);
        });
    }

    public get keynodes(): ServerKeynodes {
        return this._serverKeynodes;
    }

    public get utils(): ServerUtils {
        return this._serverUtils;
    }

    public get client(): ScNet {
        return this._client;
    }
}
