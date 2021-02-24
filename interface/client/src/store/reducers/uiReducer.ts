import * as ui from '../interfaces/ui';
import { Action } from '../actions/baseAction';
import * as uiAction from '../actions/uiActions';

export function reducer(state: ui.State = ui._initState, action: Action<any>): any {
    switch (action.type) {
        case uiAction.Type.ChangeMode:
            return { ...state, mode: action.payload };

        case uiAction.Type.ChangeInitMessage:
            return { ...state, initMessage: action.payload };

        default:
            return state;
    }
}
