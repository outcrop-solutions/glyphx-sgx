import React from 'react';
import ReactDOM from 'react-dom';
import Navbar from './Navbar';
import registerServiceWorker from './registerServiceWorker';
import './index.css';

ReactDOM.render(<Navbar />, document.getElementById('root'));
registerServiceWorker();
