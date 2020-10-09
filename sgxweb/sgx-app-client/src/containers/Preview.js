import React, { useEffect }  from 'react';
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
		if(x3-x1 == 0){
			if(x2 >= y1 && x2 <= y3){
				return x2;
			}else{
				return y3;
			}
		}
		return (((x2-x1)*(y3-y1))/(x3-x1))+y1;
	}

    function updateScene() {
        //console.log(subset);
        if(subset === null){
            //console.log("Subset is still null.");
            let timer = setInterval(function(){
                if(subset !== null){
                    processSubset();
                    clearInterval(timer);
                }
            },1000);
        }
        else{
            processSubset();
        }
    }

    let last_fields = {'xAxis': '','yAxis': '','zAxis': '','gColor': '','gSize': '','gType': ''};
    //console.log(data);
    interval = setInterval(function(){
        if(testObjEquality(fields, last_fields)){
            //console.log(fields);
            //console.log(last_fields);
            last_fields = Object.assign({}, fields);
            updateScene();
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
                    glyph.position.x = linearInterpolation(field_data[fields['xAxis']]["min"], field_data[fields['xAxis']]["max"], -20, 20, sub[fields['xAxis']][i]);
                }
                if(fields['yAxis'] !== ""){
                    glyph.position.z = linearInterpolation(field_data[fields['yAxis']]["min"], field_data[fields['yAxis']]["max"], -10, 10, sub[fields['yAxis']][i]);
                }
                if(fields['zAxis'] !== ""){
                    glyph.position.y = linearInterpolation(field_data[fields['zAxis']]["min"], field_data[fields['zAxis']]["max"], -10, 10, sub[fields['zAxis']][i]);
                }
                if(fields['gColor'] !== ""){
                    let mat = new StandardMaterial("sm", scene);
                    let r = linearInterpolation(field_data[fields['gColor']]["min"], field_data[fields['gColor']]["max"], 1, 0, sub[fields['gColor']][i]);
                    let g = linearInterpolation(field_data[fields['gColor']]["min"], field_data[fields['gColor']]["max"], 0, 0.5, sub[fields['gColor']][i]);
                    let b = linearInterpolation(field_data[fields['gColor']]["min"], field_data[fields['gColor']]["max"], 0, 0, sub[fields['gColor']][i]);
                    mat.emissiveColor = new Color3(r, g, b);
                    mat.diffuseColor = new Color3(r, g, b);
                    glyph.material = mat;
                }
                if(fields['gSize'] !== ""){
                    let value = linearInterpolation(field_data[fields['gSize']]["min"], field_data[fields['gSize']]["max"], 0.5, 1, sub[fields['gSize']][i]);
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
        subset = props.subset;
        fields = props.fields; 
        
        return function cleanup() {
            for(let i in glyphs){
                glyphs[i].dispose();
            }
            glyphs = [];
            clearInterval(interval);
        };
    });

    return (
        <div>
            <SceneComponent antialias onSceneReady={onSceneReady} onRender={onRender} data={props.data} id='my-canvas' className={classes.scene}/>
        </div>
    );
}