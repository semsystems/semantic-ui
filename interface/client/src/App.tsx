import * as React from 'react';
import 'antd/dist/antd.css';
import { connect } from 'react-redux';
import { withRouter } from 'react-router-dom';

import * as store from './store';
import { AppMain } from './ui/AppMain';


interface AppContainerProps {
    store?: store.Store;
}

function mapStateToProps(state: store.Store): any {
    return {
        store: state,
    };
}

export class AppContainerImpl extends React.Component<AppContainerProps, any> {
    private renderMainUI(): React.ReactNode {
        return <AppMain />;
    }

    private renderLoader(msg: string): React.ReactNode {
        return (
            <div className="loader-container">
                <div className="loader-vertical-center">
                    <div className="loader"></div>
                    <div className="loader-text">{msg}</div>
                </div>
            </div>
        );
    }

    private renderImpl(): React.ReactNode {
        const uiState: store.ui.State = this.props.store.ui;
        if (uiState.mode === store.ui.Mode.MainUI) return this.renderMainUI();

        return this.renderLoader(uiState.initMessage);
    }

    render(): React.ReactNode {
        return this.renderImpl();
    }
}

export const AppContainer = withRouter(connect(mapStateToProps)(AppContainerImpl) as any);
