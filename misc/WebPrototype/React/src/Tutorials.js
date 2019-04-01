import React from 'react';
// import { connect } from 'react-redux';
// import { withRouter } from 'react-router-dom';
// import { makeServerCall } from './ServerCallHelper.js';
// import { Table, TableBody, TableHeader, TableHeaderColumn, TableRow, TableRowColumn } from 'material-ui/Table';
// import SearchBox from './SearchBox.js';
// import Collapsible from 'react-collapsible';
// import Flexbox from 'flexbox-react';
import './General.css';


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
        const id = Math.floor(Math.random()*videoArr.length); 
        let srcVid = videoArr[id];
        console.log(id);
              

        let posterStr = 'https://s3.amazonaws.com/synglyphx/tutorials/videos/thumbnails/video' + id +'.png';
        console.log(id, posterStr, srcVid);

        return (
            <div height='85%'>
               <video id={id} width='100%' height='100%' poster={posterStr} controls>
                <source src={'https://s3.amazonaws.com/synglyphx/tutorials/'+ srcVid} type='video/mp4'>
                </source>
            </video>
            </div>
        );
    }
}

/**
 * Maps portions of the store to props of your choosing
 * @param state: passed down through react-redux's 'connect'
 **/
// const mapStateToProps = function(state) {
//  return {
//    settings: state.filterState.Settings,
//    storedViews: state.filterState.StoredViews,
//    funnelData: state.filterState.FunnelData
//  }
// }


/**
 * Connects the redux store to get access to global states.
 **/
export default Tutorials;