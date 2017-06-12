import React from 'react';
import ReactDOM from 'react-dom';
import TopNav from './topNav';
import FilterBar from './filterBar';
import registerServiceWorker from './registerServiceWorker';
import './index.css';

ReactDOM.render(<TopNav />, document.getElementById('root'));
registerServiceWorker();
