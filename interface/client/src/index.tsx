import * as React from 'react';
import * as ReactDOM from 'react-dom';
import * as redux from 'redux';
import { Provider } from 'react-redux';
import { BrowserRouter } from 'react-router-dom';

import { configureStore, Store } from './store';
import { AppContainer } from './App';

import { ServerRoot } from './utils/server';

const store: redux.Store<Store> = configureStore();
const server: ServerRoot = new ServerRoot(process.env.SERVER_API_PATH, store);
server.Start();

ReactDOM.render(
    <Provider store={store}>
        <BrowserRouter>
            <AppContainer />
        </BrowserRouter>
    </Provider>,
    document.getElementById('content'),
);
