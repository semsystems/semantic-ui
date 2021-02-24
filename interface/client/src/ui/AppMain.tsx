import * as React from 'react';
import { connect } from 'react-redux';

class MainImpl extends React.Component {
    render() {
        return <div></div>;
    }
}

export const AppMain = connect()(MainImpl);
