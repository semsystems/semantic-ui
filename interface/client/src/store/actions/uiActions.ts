import { Action } from './baseAction';
import * as ui from '../interfaces/ui';

export namespace Type {
    export const ChangeMode = 'UI_CHANGE_MODE';
    export const ChangeInitMessage = 'UI_CHANGE_INIT_MSG';
}

export function ChangeUIMode(mode: ui.Mode): Action<ui.Mode> {
    return {
        type: Type.ChangeMode,
        payload: mode,
    };
}

export function ChangeInitMessage(msg: string): Action<string> {
    return {
        type: Type.ChangeInitMessage,
        payload: msg,
    };
}
