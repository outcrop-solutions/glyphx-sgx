import React from 'react';
// import { connect } from 'react-redux';
// import { withRouter } from 'react-router-dom';
// import { makeServerCall } from './ServerCallHelper.js';
// import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
// import SearchBox from './SearchBox.js';
// import Collapsible from 'react-collapsible';
// import Flexbox from 'flexbox-react';
import './css/General.css';


class Tutorials extends React.Component {
    
    render() {
        const videoArr = [
            "videos/01%20Opening%20and%20Logging%20Into%20GlyphViewer.mp4",
            "videos/02%20Opening%20A%20Viz%20From%20All%20Views.mp4",
            "videos/03%20Opening%20a%20Viz%20with%20Front%20End%20Filters.mp4",
            "videos/04%20Rotation%20and%20Zoom%20Using%20Mouse%20Controls.mp4",
            "videos/05%20Rotation%20and%20Zoom%20Using%20In%20Scene%20Click%20Controls.mp4",
            "videos/06%20Accessing%20the%20Legend.mp4",
        ];
        let id = 0;
        const hostName = 'https://s3.amazonaws.com/synglyphx/tutorials/';
        const hostNameThumbnail = 'https://s3.amazonaws.com/synglyphx/tutorials/videos/thumbnails/video';
        const srcVid = hostName + videoArr[id]; 
        const posterStr =  hostNameThumbnail + id +'.png';

        return (
            <div className='tutorial-div' height='85%'>
               <video 
               width='100%' height='100%' poster={posterStr} 
                onEnded={(e) => {               
                        id++;
                        if(id < videoArr.length){
                            e.target.src = hostName + videoArr[id];
                            e.target.poster = hostNameThumbnail + id + '.png';
                        }
                }} controls
                >
                <source id='tutorial-video' src={srcVid} type='video/mp4' />
            </video>
            </div>
        );
    }
}

export default (Tutorials);