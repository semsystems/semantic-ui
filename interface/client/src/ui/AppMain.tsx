import * as React from 'react';
import { connect } from 'react-redux';
import { UiModule } from './UiModule';

class MainImpl extends React.Component {
    render() {
        return <UiModule />;
    }
}

export const AppMain = connect()(MainImpl);
