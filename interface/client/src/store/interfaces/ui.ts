export enum Mode {
    Initializing,
    MainUI,
}

export interface State {
    mode: Mode;
    initMessage: string;
}

export const _initState: State = {
    mode: Mode.Initializing,
    initMessage: '',
};
