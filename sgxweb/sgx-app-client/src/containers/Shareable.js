import React, { useState, useEffect } from "react";
import { API } from "aws-amplify";
import { useHistory } from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
import { ArcRotateCamera, Vector3, Color3, DirectionalLight } from '@babylonjs/core';
import { ProcessSubset } from '../libs/utils.js';
import SceneComponent from '../components/Scene';
import EmptyGlyph from "../images/empty-glyph-pane.png";
import CircularProgress from '@material-ui/core/CircularProgress';
import "./NotFound.css"

const useStyles = makeStyles((theme) => ({ 
    scene: {
        width: '100%',
        height: window.innerHeight-5,
    }
}));

let glyphs = [];
let subset = {};
let sorted = {};
let needsMap = true;
let isSceneLoaded = false;
let fields = {'xAxis': '','yAxis': '','zAxis': '','gColor': '','gSize': '','gType': ''};
let xInt = 35, yInt = 15, zInt = 10; //Default interpolation values
let identity;
let query;
let field_data = {};

const onSceneReady = (scene, data) => {

    data.then(function(result){
        let contents = JSON.parse(result.body);
        identity = contents.identity;
        let name = contents.name;
        let prps = contents.properties;
        console.log(identity);
        console.log(name);
        console.log(prps);
        fields = {'xAxis': prps.xaxis.fieldname,'yAxis': prps.yaxis.fieldname,'zAxis': prps.zaxis.fieldname,'gColor': prps.gcolor.fieldname,'gSize': prps.gsize.fieldname,'gType': prps.gtype.fieldname};
        getShareableMetadata(identity, name).then(function(result) {
            let data = JSON.parse(result.body);
            console.log(data);
            for(let i in data){
                let type = data[i]["type"];
                let min = data[i]["min"];
                let max = data[i]["max"];
                field_data[data[i]["fieldname"]] = {"type": type, "min": min, "max": max};
            }
            let tablename = name.toLowerCase().split(" ").join("_");
            query = "SELECT * FROM "+tablename+" LIMIT 500";

            updateScene();
        });
    });

    // This creates and positions a free camera (non-mesh)
    var camera = new ArcRotateCamera("Camera", 3 * Math.PI / 2, Math.PI / 3, 50, Vector3.Zero(), scene);
    camera.lowerRadiusLimit = 3;

    // This targets the camera to scene origin
    camera.setTarget(Vector3.Zero());

    const canvas = scene.getEngine().getRenderingCanvas();
    
    // This attaches the camera to the canvas
    camera.attachControl(canvas, true);

    var light = new DirectionalLight("Dir0", new Vector3(1, -1, 1), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light.specular = new Color3(0, 0, 0);
    light.intensity = 0.6;
    light.parent = camera;

    scene.clearColor = Color3.White();

    async function updateScene() {

        if(Object.keys(subset).length === 0){
            let sub = new Promise((resolve, reject) => {
                resolve(fetchSubset(identity, query));
            });
            sub.then(results => {
                subset = results;
                ProcessSubset(scene, subset, fields, field_data, sorted, glyphs, xInt, yInt, zInt, needsMap)
                isSceneLoaded = true;
            })
        }
        else{
            ProcessSubset(scene, subset, fields, field_data, sorted, glyphs, xInt, yInt, zInt, needsMap)
            isSceneLoaded = true;
        }
    }

    async function fetchSubset(identity, query){
        return getQueryResults(identity, query).then(result => {
            return result['body'];
        });
    }

    function getQueryResults(identityId, query) {
        return API.post("sgx", "/get-query-results", {
          body: "{\"identity\":\""+identityId+"\", \"query\":\""+query+"\"}"
        });
    }

}
/**
 * Will run on every frame render.  We are spinning the box on y-axis.
 */

const onRender = scene => {
    /*if (glyphs[0] !== undefined) {
      var deltaTimeInMillis = scene.getEngine().getDeltaTime();
  
      const rpm = 10;
      glyphs[0].rotation.y += ((rpm / 60) * Math.PI * 2 * (deltaTimeInMillis / 1000));
    }*/
}

function getShareableMetadata(identity, name) {
    return API.post("sgx", "/get-shareable-metadata", {
      body: "{\"identity\":\""+identity+"\", \"name\":\""+name+"\"}"
    });
}


export default function Shareable(props) {
    const classes = useStyles();
    const history = useHistory();
    const [isActive, setIsActive] = useState(false);
    const [vizId, setVizId] = useState(0);
    const [isLoading, isLoadingScene] = useState(true);

    useEffect(() => {
        if(history.location.data !== 0){
            setIsActive(true);
            if(vizId != history.location.data){
                //setLoaded(true);
                for(let i in glyphs){
                    glyphs[i].dispose();
                }
                glyphs = [];
                subset = {};
                sorted = {};
                needsMap = true;
            
                console.log("needsMap =", needsMap);
                console.log("numGlyphs =", glyphs.length);

                //let tablename = props.tablename;
                //identity = props.identity;
                //query = "SELECT * FROM "+tablename+" LIMIT 500";
                //let query = "SELECT * FROM "+tablename+" ORDER BY RAND() LIMIT 100";
        
            }

            //fields = props.fields; 
            //console.log(fields);

            setVizId(history.location.data);

        }
    });

    async function getShareableById(id) {
        return API.post("sgx", "/get-shareable-by-id", {
          body: "{\"id\":\""+id+"\"}"
        });
    }

    useEffect(() => {

        let loading_interval = setInterval(function(){
            isLoadingScene(false);
            clearInterval(loading_interval);
        }, 3000);

    });

    return (
        <div>
            {isActive
                ?  
                [(isLoading
                ? 
                <div key='0' style={{textAlign: 'center', position: 'absolute', left: '49%', top: '49%'}}>
                <   CircularProgress />
                </div>
                :
                null
                ),
                <SceneComponent key='1' antialias onSceneReady={onSceneReady} onRender={onRender} data={getShareableById(history.location.data)} id='my-canvas' className={classes.scene}/>
                ]
                :
                <div style={{textAlign: 'center'}}>
                    <img src={EmptyGlyph} alt="EmptyGlyph" style={{maxWidth: 225, marginTop: '15%'}}/>
                    <div style={{color: 'gray', maxWidth: 250, margin: 'auto', fontSize: '1rem'}}>
                        This link is invalid.
                    </div>
                </div>
            }
        </div>
    );
}