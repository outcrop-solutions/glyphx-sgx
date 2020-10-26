import React, { useEffect }  from 'react';
import { API } from "aws-amplify";
import { makeStyles } from '@material-ui/core/styles';
import { ArcRotateCamera, Vector3, Color3, HemisphericLight, MeshBuilder, StandardMaterial } from '@babylonjs/core';
import SceneComponent from '../components/Scene'; // ^^ point to file we created above or 'babylonjs-hook' NPM.
//import './App.css';

const useStyles = makeStyles((theme) => ({ 
    scene: {
        width: '100%',
        height: window.innerHeight-135
    }
  }));

let glyphs = [];
let subset = {};
let interval;
let fields = {'xAxis': '','yAxis': '','zAxis': '','gColor': '','gSize': '','gType': ''};

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

    var light = new HemisphericLight("light", new Vector3(0, 1, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light.intensity = 0.7;

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

    //x1 min, x3, max, y1 min, y3 max, x2 data value
    function linearInterpolation(x1, x3, y1, y3, x2){
		if(x3-x1 === 0){
			if(x2 >= y1 && x2 <= y3){
				return x2;
			}else{
				return y3;
			}
		}
		return (((x2-x1)*(y3-y1))/(x3-x1))+y1;
    }

    let sorted = {};
    function textInterpolation(field, y1, y3, x2_str, sub){
        let dataCpy;
        if(field in sorted){
            dataCpy = sorted[field];
        }
        else{
            dataCpy = [...sub[field]];
            dataCpy.sort();
            sorted[field] = dataCpy;
        }

        let x1 = 0;
        let x3 = dataCpy.length;
        let x2 = dataCpy.indexOf(x2_str);
        return linearInterpolation(x1, x3, y1, y3, x2);
    }

    function updateScene() {
        //console.log(subset);
        if(subset === null || subset === undefined || subset === {}){
            //console.log("Subset is still null.");
            let timer = setInterval(function(){
                //console.log("Subset is still null. inner");
                console.log(subset);
                if(subset !== null && subset !== undefined && subset !== {}){
                    processSubset();
                    console.log("Processed subset.");
                    clearInterval(timer);
                }
            },1000);
        }
        else{
            processSubset();
            console.log("Processed subset.");
        }
    }

    let last_fields = {'xAxis': '','yAxis': '','zAxis': '','gColor': '','gSize': '','gType': ''};
    console.log("Running...");
    interval = setInterval(function(){
        if(testObjEquality(fields, last_fields)){
            try{
                updateScene();
                last_fields = Object.assign({}, fields);
                console.log("Scene updated.")
            }
            catch(err){
                console.log(err.message);
            }
            //console.log(fields);
            //console.log(last_fields);
            //console.log(subset);
        }
    },1000);

    function processSubset() {
        let sub = JSON.parse(subset);
        if(fields['xAxis'] !== "" || fields['yAxis'] !== ""){
            let total_count = sub[Object.keys(sub)[0]].length;
            console.log(glyphs.length, total_count);
            for(let i = 0; i < total_count; i++){
                let glyph;
                // Our built-in 'box' shape.
                if(glyphs.length !== total_count){
                    glyph = MeshBuilder.CreateBox("box", {size: 2}, scene);
                    glyphs.push(glyph);
                }
                else{
                    glyph = glyphs[i];
                }

                if(fields['gType'] !== ""){
                    let toChange = Math.floor(Math.random() * Math.floor(2));
                    if(toChange === 1){
                        let mat = glyph.material;
                        glyph.dispose();
                        glyph = MeshBuilder.CreateSphere("Sphere", {segments: 16, diameter: 2}, scene);
                        glyphs[i] = glyph;
                        glyph.material = mat;
                    }
                }
                if(fields['xAxis'] !== ""){
                    let f = field_data[fields['xAxis']];
                    if(field_data[fields['xAxis']]["type"] !== "string"){
                        glyph.position.x = linearInterpolation(f["min"], f["max"], -20, 20, sub[fields['xAxis']][i]);
                    }
                    else{
                        glyph.position.x = textInterpolation(fields['xAxis'], -20, 20, sub[fields['xAxis']][i], sub);
                    }
                }
                if(fields['yAxis'] !== ""){
                    let f = field_data[fields['yAxis']];
                    if(field_data[fields['yAxis']]["type"] !== "string"){
                        glyph.position.z = linearInterpolation(f["min"], f["max"], -10, 10, sub[fields['yAxis']][i]);
                    }
                    else{
                        glyph.position.z = textInterpolation(fields['yAxis'], -10, 10, sub[fields['yAxis']][i], sub);
                    }
                }
                if(fields['zAxis'] !== ""){
                    let f = field_data[fields['zAxis']];
                    if(field_data[fields['zAxis']]["type"] !== "string"){
                        glyph.position.y = linearInterpolation(f["min"], f["max"], -10, 10, sub[fields['zAxis']][i]);
                    }
                    else{
                        glyph.position.y = textInterpolation(fields['zAxis'], -10, 10, sub[fields['zAxis']][i], sub);
                    }
                }
                if(fields['gColor'] !== ""){
                    let f = field_data[fields['gColor']];
                    let mat = new StandardMaterial("sm", scene);
                    let r, g, b;
                    if(field_data[fields['gColor']]["type"] !== "string"){
                        r = linearInterpolation(f["min"], f["max"], 1, 0, sub[fields['gColor']][i]);
                        g = linearInterpolation(f["min"], f["max"], 0, 0.5, sub[fields['gColor']][i]);
                        b = linearInterpolation(f["min"], f["max"], 0, 0, sub[fields['gColor']][i]);
                    }
                    else{
                        r = textInterpolation(fields['gColor'], 1, 0, sub[fields['gColor']][i], sub);
                        g = textInterpolation(fields['gColor'], 0, 0.5, sub[fields['gColor']][i], sub);
                        b = textInterpolation(fields['gColor'], 0, 0, sub[fields['gColor']][i], sub);
                    }
                    mat.emissiveColor = new Color3(r, g, b);
                    mat.diffuseColor = new Color3(r, g, b);
                    glyph.material = mat;
                }
                if(fields['gSize'] !== ""){
                    let f = field_data[fields['gSize']];
                    let value;
                    if(field_data[fields['gSize']]["type"] !== "string"){
                        value = linearInterpolation(f["min"], f["max"], 0.5, 1, sub[fields['gSize']][i]);
                    }
                    else{
                        value = textInterpolation(fields['gSize'], 0.5, 1, sub[fields['gSize']][i], sub);
                    }
                    glyph.scaling = new Vector3(value, value, value);
                }
                //console.log(glyph.position);
            }
        }else{
            //let box;
            // Our built-in 'box' shape.
            //box = MeshBuilder.CreateBox("box", {size: 2}, scene);
            //MeshBuilder.CreateSphere("Sphere"+color, 16, 1, scene);

            // Move the box upward 1/2 its height
            //box.position.y = 1;

            // Our built-in 'ground' shape.
            //MeshBuilder.CreateGround("ground", {width: 6, height: 6}, scene);

            //glyphs.push(box);
        }
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

    useEffect(() => {
        //console.log("prop.fields",props.fields);
        //console.log("fields", fields);
        fields = props.fields; 

        let tablename = props.tablename;
        let identity = props.identity;
        let query = "SELECT * FROM "+tablename+" LIMIT 500";
        //let query = "SELECT * FROM "+tablename+" ORDER BY RAND() LIMIT 100";
        fetchSubset(identity, query);
        
        return function cleanup() {
            for(let i in glyphs){
                glyphs[i].dispose();
            }
            glyphs = [];
            subset = {};
            clearInterval(interval);
        };
    });

    async function fetchSubset(identity, query){
        await getQueryResults(identity, query).then(result => {
          //console.log(result['body']);
          subset = result['body'];
          //console.log(subset);
          console.log("Subset fetched.");
        });
    }

    function getQueryResults(identityId, query) {
        return API.post("sgx", "/get-query-results", {
          body: "{\"identity\":\""+identityId+"\", \"query\":\""+query+"\"}"
        });
    }

    return (
        <div>
            <SceneComponent antialias onSceneReady={onSceneReady} onRender={onRender} data={props.data} id='my-canvas' className={classes.scene}/>
        </div>
    );
}