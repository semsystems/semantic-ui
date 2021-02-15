import { combineReducers } from 'redux';

import * as net from './netReducer';
import * as services from './serviceReducer';
import * as ui from './uiReducer';

export const rootReducer = combineReducers({
    net: net.reducer,
    services: services.reducer,
    ui: ui.reducer,
});
