import React, { useState, useEffect }  from 'react';
import { API } from "aws-amplify";
import { makeStyles } from '@material-ui/core/styles';
import { ArcRotateCamera, Vector3, Color3, DirectionalLight } from '@babylonjs/core';
import SceneComponent from '../components/Scene'; // ^^ point to file we created above or 'babylonjs-hook' NPM.
import { ProcessSubset } from '../libs/utils.js';
import CircularProgress from '@material-ui/core/CircularProgress';
//import './App.css';

const useStyles = makeStyles((theme) => ({ 
    scene: {
        width: '100%',
        height: window.innerHeight-135,
    }
  }));

let glyphs = [];
let subset = {};
let sorted = {};
let interval_to_run = null;
let isSceneLoaded = false;
let fields = {'xAxis': '','yAxis': '','zAxis': '','gColor': '','gSize': '','gType': ''};
let last_fields = {'xAxis': '','yAxis': '','zAxis': '','gColor': '','gSize': '','gType': ''};
let needsMap = true;
let xInt = 35, yInt = 15, zInt = 10; //Default interpolation values
let identity;
let query;

const onSceneReady = (scene, data) => {

    let field_data = {};
    for(let i in data){
        let type = data[i]["type"];
        let min = data[i]["min"];
        let max = data[i]["max"];
        field_data[data[i]["fieldname"]] = {"type": type, "min": min, "max": max};
    }
    //console.log(field_data);

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

    function testObjEquality(a, b){
        if(a['xAxis'] !== b['xAxis']) return true;
        if(a['yAxis'] !== b['yAxis']) return true;
        if(a['zAxis'] !== b['zAxis']) return true;
        if(a['gColor'] !== b['gColor']) return true;
        if(a['gSize'] !== b['gSize']) return true;
        if(a['gType'] !== b['gType']) return true;
        return false;
    }

    console.log("Running...");
    clearInterval(interval_to_run);
    interval_to_run = setInterval(function(){
        if(testObjEquality(fields, last_fields)){
            try{
                updateScene();
                last_fields = Object.assign({}, fields);
                console.log("Scene updated.");
            }
            catch(err){
                console.log(err.message);
            }
        }
    }, 1000);

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

export default function Preview(props) {
    const classes = useStyles();
    const [isLoading, isLoadingScene] = useState(true);
    //const [loaded, setLoaded] = useState(false);
    const [vizId, setVizId] = useState(0);

    useEffect(() => {

        if(vizId != props.vizId){
            //setLoaded(true);
            for(let i in glyphs){
                glyphs[i].dispose();
            }
            glyphs = [];
            subset = {};
            sorted = {};
            needsMap = true;
        
            //console.log("needsMap =", needsMap);
            //console.log("numGlyphs =", glyphs.length);

            let tablename = props.tablename;
            identity = props.identity;
            query = "SELECT * FROM "+tablename+" LIMIT 500";
            //let query = "SELECT * FROM "+tablename+" ORDER BY RAND() LIMIT 100";
     
        }

        fields = props.fields; 
        console.log(fields);

        setVizId(props.vizId);
        //console.log("props.data:", props.data);

    });

    useEffect(() => {

        let loading_interval = setInterval(function(){
            if(isSceneLoaded){
                props.setIsDisabled(false);
                isLoadingScene(false);
                clearInterval(loading_interval);
            }
        }, 1000);

    });

    return (
        <div>
            {isLoading
            ? 
            <div style={{textAlign: 'center', position: 'absolute', left: '62%', top: '45%'}}>
            <   CircularProgress />
            </div>
            :
            <></>
            }
            <SceneComponent antialias onSceneReady={onSceneReady} onRender={onRender} data={props.data} id='my-canvas' className={classes.scene}/>
        </div>
    );
}