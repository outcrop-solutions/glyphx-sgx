import React, { useState, useEffect }  from 'react';
import { API } from "aws-amplify";
import { makeStyles } from '@material-ui/core/styles';
import { ArcRotateCamera, Vector3, Color3, DirectionalLight, MeshBuilder, StandardMaterial, Texture } from '@babylonjs/core';
import SceneComponent from '../components/Scene'; // ^^ point to file we created above or 'babylonjs-hook' NPM.
import { GetZoomLevel } from '../libs/utils.js';
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

    console.log("Running...");
    clearInterval(interval_to_run);
    interval_to_run = setInterval(function(){
        if(testObjEquality(fields, last_fields)){
            try{
                updateScene();
                last_fields = Object.assign({}, fields);
                console.log("Scene updated.")
            }
            catch(err){
                console.log(err.message);
            }
        }
    }, 1000);

    async function updateScene() {
        //console.log(subset);
        if(Object.keys(subset).length === 0){
            subset = await fetchSubset(identity, query);
        }
        processSubset();
        isSceneLoaded = true;
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

    function processSubset() {
        let sub = JSON.parse(subset);
        if(fields['xAxis'] !== "" || fields['yAxis'] !== ""){
            let total_count = sub[Object.keys(sub)[0]].length;
            console.log(glyphs.length, total_count);
            for(let i = 0; i < total_count; i++){
                let glyph;

                if(needsMap){
                    if(fields['xAxis'].toLowerCase().includes('long') &&
                        fields['yAxis'].toLowerCase().includes('lat')) {

                        let x = field_data[fields['xAxis']];
                        let y = field_data[fields['yAxis']];
                        let minX = parseFloat(x['min']), maxX = parseFloat(x['max']);
                        let minY = parseFloat(y['min']), maxY = parseFloat(y['max']);
                        let centerX = ((minX + maxX) / 2.0);
                        let centerY = ((minY + maxY) / 2.0);
                        let center = centerY + "," + centerX;
                        console.log("Zoom:", GetZoomLevel(minX, maxX, minY, maxY, centerX, centerY, true));
                        let gzl = GetZoomLevel(minX, maxX, minY, maxY, centerX, centerY, true);
                        let zoom = gzl[0];
                        let mapWidth = gzl[1], mapHeight = gzl[2];
                        //let mapWidth = 2048, mapHeight = 1024;
                        let size = mapWidth+","+mapHeight;
                        let groundWidth = mapWidth/25, groundHeight = mapHeight/25;

                        //let apiKey = "AIzaSyDPO-xd-zLjtZ4-dYkGrU-qE7NvmTMM52s";
                        //let mapUrl = "https://maps.googleapis.com/maps/api/staticmap?center="+center+"&zoom="+zoom+"&size="+size+"&key="+apiKey;
                        let map = false, satellite = false;
                        let apiKey = "pba2AzAMjfkGrvf7GUuA2nWX9RHclALU";
                        let mapUrl = "http://open.mapquestapi.com/staticmap/v4/getmap?key=" + apiKey;
                        mapUrl += "&center=" + center;
                        mapUrl += "&zoom=" + zoom + "&size=" + size;
                        mapUrl += "&type=";
                        if (map) {
                            mapUrl += "map";
                        }
                        else if (satellite) {
                            mapUrl += "sat";
                        }
                        else {
                            mapUrl += "hyb";
                        }
                        mapUrl += "&imagetype=png";
                        mapUrl += "&pois=";
                        console.log(mapUrl);

                        var base_image_light = new DirectionalLight("Dir0", new Vector3(0, -1, 0), scene);
                        base_image_light.specular = new Color3(0, 0, 0);
                        base_image_light.intensity = 1.0;

                        var baseImage = new StandardMaterial("ground", scene);
                        baseImage.diffuseTexture = new Texture(mapUrl, scene);

                        let ground = MeshBuilder.CreateGround("ground", {width: groundWidth, height: groundHeight}, scene);
                        ground.position.y = -1;
                        ground.material = baseImage;
                        var bbMax = ground.getBoundingInfo().boundingBox.maximum;
                        xInt = bbMax.x;
                        yInt = bbMax.z;
                        console.log(xInt, yInt);
                        base_image_light.includedOnlyMeshes.push(ground);

                        needsMap = false;
                    }
                }
                // Our built-in 'box' shape.
                if(glyphs.length !== total_count){
                    glyph = MeshBuilder.CreateBox("box", {size: 0.5}, scene);
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
                        glyph = MeshBuilder.CreateSphere("Sphere", {segments: 16, diameter: 1}, scene);
                        glyphs[i] = glyph;
                        glyph.material = mat;
                    }
                }
                if(fields['xAxis'] !== ""){
                    let f = field_data[fields['xAxis']];
                    if(field_data[fields['xAxis']]["type"] !== "string"){
                        glyph.position.x = linearInterpolation(f["min"], f["max"], xInt*-1, xInt, sub[fields['xAxis']][i]);
                    }
                    else{
                        glyph.position.x = textInterpolation(fields['xAxis'], xInt*-1, xInt, sub[fields['xAxis']][i], sub);
                    }
                }
                if(fields['yAxis'] !== ""){
                    let f = field_data[fields['yAxis']];
                    if(field_data[fields['yAxis']]["type"] !== "string"){
                        glyph.position.z = linearInterpolation(f["min"], f["max"], yInt*-1, yInt, sub[fields['yAxis']][i]);
                    }
                    else{
                        glyph.position.z = textInterpolation(fields['yAxis'], yInt*-1, yInt, sub[fields['yAxis']][i], sub);
                    }
                }
                if(fields['zAxis'] !== ""){
                    let f = field_data[fields['zAxis']];
                    if(field_data[fields['zAxis']]["type"] !== "string"){
                        glyph.position.y = linearInterpolation(f["min"], f["max"], 0, zInt, sub[fields['zAxis']][i]);
                    }
                    else{
                        glyph.position.y = textInterpolation(fields['zAxis'], 0, zInt, sub[fields['zAxis']][i], sub);
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
            }
            console.log(fields);
            console.log(scene.getNodes().length);
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
    const [isLoading, isLoadingScene] = useState(true);
    //const [loaded, setLoaded] = useState(false);
    const [vizId, setVizId] = useState(0);

    useEffect(() => {
        //console.log("prop.fields",props.fields);
        //console.log("fields", fields);
        //console.log("loaded =", loaded);
        //console.log('vizId =', vizId);
        //console.log('props =', props.vizId);
        if(vizId != props.vizId){
            //setLoaded(true);
            for(let i in glyphs){
                glyphs[i].dispose();
            }
            glyphs = [];
            subset = {};
            needsMap = true;
        
            console.log("needsMap =", needsMap);
            console.log("numGlyphs =", glyphs.length);

            let tablename = props.tablename;
            identity = props.identity;
            query = "SELECT * FROM "+tablename+" LIMIT 500";
            //let query = "SELECT * FROM "+tablename+" ORDER BY RAND() LIMIT 100";
     
        }

        fields = props.fields; 

        setVizId(props.vizId);

    });

    useEffect(() => {

        let loading_interval = setInterval(function(){
            props.setIsDisabled(false);
            isLoadingScene(false);
            clearInterval(loading_interval);
        }, 3000);

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