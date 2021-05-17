import React, { useState, useEffect } from "react";
//import { API } from "aws-amplify";
import { useHistory } from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
import { ArcRotateCamera, Vector3, Color3, HemisphericLight, MeshBuilder, Mesh, StandardMaterial, PointerDragBehavior } from '@babylonjs/core';
import * as GUI from 'babylonjs-gui';
import { LinearInterpolation } from '../libs/utils.js';
import SceneComponent from '../components/Scene';
import "./NotFound.css"

const useStyles = makeStyles((theme) => ({ 
    scene: {
        width: '100%',
        height: window.innerHeight-60,
    }
}));

const leftBound = 6, rightBound = -6;
const xMin = 0, xMax = 45;
const zMin = 0, zMax = 45;
var nodes = [];

const onSceneReady = (scene) => {

    //var plane_x = null, plane_z = null;
    //var plane_x_two = null, plane_z_two = null;
    // This creates and positions a free camera (non-mesh)
    var camera = new ArcRotateCamera("Camera", 3 * Math.PI / 2, Math.PI / 3, 25, Vector3.Zero(), scene);
    camera.lowerRadiusLimit = 3;

    // This targets the camera to scene origin
    camera.setTarget(Vector3.Zero());

    const canvas = scene.getEngine().getRenderingCanvas();
    
    // This attaches the camera to the canvas
    camera.attachControl(canvas, true);

    var light = new HemisphericLight("light1", new Vector3(0, 1, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    //light.specular = new Color3(0, 0, 0);
    light.intensity = 0.7;
    //light.parent = camera;

    const cubeSize = 0.125;
    const wallHeight = 5;
    const distance = leftBound-rightBound;

    const plane_x = MeshBuilder.CreatePlane("plane_x", {height: wallHeight, width: leftBound*2, sideOrientation: Mesh.DOUBLESIDE});
    plane_x.rotation.y = Math.PI/2;
    plane_x.position.y = wallHeight/2;
    plane_x.position.x = leftBound;
    var mat_x = new StandardMaterial("mat_x", scene);
    mat_x.emissiveColor = new Color3.Red();
    mat_x.diffuseColor = new Color3.Red();
    mat_x.alpha = 0.1;
    plane_x.material = mat_x;

    const plane_z = MeshBuilder.CreatePlane("plane_z", {height: wallHeight, width: leftBound*2, sideOrientation: Mesh.DOUBLESIDE});
    plane_z.position.y = wallHeight/2;
    plane_z.position.z = leftBound;
    var mat_z = new StandardMaterial("mat_z", scene);
    mat_z.emissiveColor = new Color3.Blue();
    mat_z.diffuseColor = new Color3.Blue();
    mat_z.alpha = 0.1;
    plane_z.material = mat_z;

    const plane_x_two = MeshBuilder.CreatePlane("plane_x_two", {height: 0.5, width: 1.5, sideOrientation: Mesh.DOUBLESIDE});
    plane_x_two.rotation.x = Math.PI/2;
    plane_x_two.rotation.y = Math.PI/2;
    plane_x_two.position.z = rightBound-0.75;
    plane_x_two.position.x = rightBound;
    var mat_x_two = new StandardMaterial("mat_x_two", scene);
    mat_x_two.alpha = 0;
    plane_x_two.material = mat_x_two;

    const plane_z_two = MeshBuilder.CreatePlane("plane_z_two", {height: 0.5, width: 1.5, sideOrientation: Mesh.DOUBLESIDE});
    plane_z_two.rotation.x = Math.PI/2;
    plane_z_two.position.x = rightBound-0.75;
    plane_z_two.position.z = rightBound;
    var mat_z_two = new StandardMaterial("mat_z_two", scene);
    mat_z_two.alpha = 0;
    plane_z_two.material = mat_z_two;

    var pointerDragBehavior_x = new PointerDragBehavior({dragAxis: new Vector3(1,0,0)});
    var pointerDragBehavior_z = new PointerDragBehavior({dragAxis: new Vector3(0,0,1)});
    var pointerDragBehavior_x_two = new PointerDragBehavior({dragAxis: new Vector3(1,0,0)});
    var pointerDragBehavior_z_two = new PointerDragBehavior({dragAxis: new Vector3(0,0,1)});

    pointerDragBehavior_x.validateDrag = (targetPosition)=>{
        let closest = plane_x_two.position.x+(distance/(xMax+1));
        if(targetPosition.x > leftBound){
            plane_x.position.x = leftBound
        }
        else if(targetPosition.x < closest){
            plane_x.position.x = closest;
        }
        else{
            plane_x.position.x = targetPosition.x;
        }
    }

    pointerDragBehavior_z.validateDrag = (targetPosition)=>{
        let closest = plane_z_two.position.z+(distance/(zMax+1));
        if(targetPosition.z > leftBound){
            plane_z.position.z = leftBound;
        }
        else if(targetPosition.z < closest){
            plane_z.position.z = closest;
        }
        else{
            plane_z.position.z = targetPosition.z;
        }
    }
    
    pointerDragBehavior_x_two.validateDrag = (targetPosition)=>{
        let closest = plane_x.position.x-(distance/(xMax+1));
        if(targetPosition.x > closest){
            plane_x_two.position.x = closest;
        }
        else if(targetPosition.x < rightBound){
            plane_x_two.position.x = rightBound;
        }
        else{
            plane_x_two.position.x = targetPosition.x;
        }
    }

    pointerDragBehavior_z_two.validateDrag = (targetPosition)=>{
        let closest = plane_z.position.z-(distance/(zMax+1));
        if(targetPosition.z > closest){
            plane_z_two.position.z = closest;
        }
        else if(targetPosition.z < rightBound){
            plane_z_two.position.z = rightBound;
        }
        else{
            plane_z_two.position.z = targetPosition.z;
        }
    }

    // Use drag plane in world space
    pointerDragBehavior_x.useObjectOrientationForDragging = false;
    pointerDragBehavior_z.useObjectOrientationForDragging = false;
    pointerDragBehavior_x_two.useObjectOrientationForDragging = false;
    pointerDragBehavior_z_two.useObjectOrientationForDragging = false;

    //var x_sign = false;
    //var z_sign = false;

    function filterNodes() {
        //console.log(camera.position);
        for(let i = 0; i < nodes.length; i++){
            if(!evaluateX_two(i) || !evaluateZ_two(i)){
                nodes[i].setEnabled(false);
            }
            else{
                nodes[i].setEnabled(true);
            }
        }
    }

    /*function evaluateX(i) {
        if(x_sign && plane_x.position.x != leftBound)
            return nodes[i].position.x < plane_x.position.x;
        return nodes[i].position.x > plane_x.position.x;
    }

    function evaluateZ(i) {
        if(z_sign && plane_z.position.z != leftBound)
            return nodes[i].position.z < plane_z.position.z;
        return nodes[i].position.z > plane_z.position.z;
    }*/

    function evaluateX_two(i) {
        return nodes[i].position.x < plane_x.position.x && nodes[i].position.x > plane_x_two.position.x;
    }

    function evaluateZ_two(i) {
        return nodes[i].position.z < plane_z.position.z && nodes[i].position.z > plane_z_two.position.z;
    }

    pointerDragBehavior_x.onDragObservable.add((event)=>{
        label1.text = "X: " + Math.round(LinearInterpolation(leftBound, rightBound, xMin, xMax, plane_x.position.x)).toString();
    });
    pointerDragBehavior_x.onDragEndObservable.add((event)=>{
        filterNodes();
    });
    pointerDragBehavior_z.onDragObservable.add((event)=>{
        //label2.text = "Z: " + Math.round(LinearInterpolation(leftBound, rightBound, zMin, zMax, plane_z.position.z)).toString();
    });
    pointerDragBehavior_z.onDragEndObservable.add((event)=>{
        filterNodes();
    });

    pointerDragBehavior_x_two.onDragObservable.add((event)=>{
        //label3.text = "X: " + Math.round(LinearInterpolation(leftBound, rightBound, xMin, xMax, plane_x_two.position.x)).toString();
    });
    pointerDragBehavior_x_two.onDragEndObservable.add((event)=>{
        filterNodes();
    });
    pointerDragBehavior_z_two.onDragObservable.add((event)=>{
        //label4.text = "Z: " + Math.round(LinearInterpolation(leftBound, rightBound, zMin, zMax, plane_z_two.position.z)).toString();
    });
    pointerDragBehavior_z_two.onDragEndObservable.add((event)=>{
        filterNodes();
    });

    plane_x.addBehavior(pointerDragBehavior_x);
    plane_z.addBehavior(pointerDragBehavior_z);
    plane_x_two.addBehavior(pointerDragBehavior_x_two);
    plane_z_two.addBehavior(pointerDragBehavior_z_two);

    //console.log(scene);
    var advancedTexture = GUI.AdvancedDynamicTexture.CreateFullscreenUI("UI", true, scene);

    var rect1 = new GUI.Rectangle();
    configureLabel(rect1, plane_x, -150);

    var label1 = new GUI.TextBlock();
    label1.text = "X: 0";
    rect1.addControl(label1);

    var rect2 = new GUI.Rectangle();
    configureLabel(rect2, plane_z, -150);

    var label2 = new GUI.TextBlock();
    label2.text = "Z: 0";
    rect2.addControl(label2);

    var rect3 = new GUI.Rectangle();
    configureLabel(rect3, plane_x_two, 0);

    var label3 = new GUI.TextBlock();
    label3.text = "X: "+xMax;
    rect3.addControl(label3);

    var rect4 = new GUI.Rectangle();
    configureLabel(rect4, plane_z_two, 0);

    var label4 = new GUI.TextBlock();
    label4.text = "Z: "+zMax;
    rect4.addControl(label4);

    function configureLabel(rect, linkedPlane, offset) {
        rect.width = 0.03;
        rect.height = "40px";
        rect.cornerRadius = 20;
        rect.color = "White";
        rect.alpha = 0.5;
        advancedTexture.addControl(rect);
        rect.linkWithMesh(linkedPlane);   
        rect.linkOffsetY = offset;

    }

    let offset = cubeSize/2;
    let heightMap = new Map();

    for(let i = 0; i < xMax+1; i++){
        for(let j = 0; j < xMax+1; j++){
            let height = (Math.floor(Math.random() * 4) + 1) / 4.0;
            let box = null;
            if(!heightMap.has(height)){
                box = MeshBuilder.CreateBox("box_"+i+"_"+j, {width: cubeSize, depth: cubeSize, height: height });
                heightMap[height] = nodes.length;
            }
            else
                box = nodes[heightMap[height]].clone("box_"+i+"_"+j);
            box.position.x = LinearInterpolation(xMin, xMax, rightBound+offset, leftBound-offset, i);
            box.position.y = height / 2.0;
            box.position.z = LinearInterpolation(zMin, zMax, rightBound+offset, leftBound-offset, j);
            box.freezeWorldMatrix();
            nodes.push(box);
        }
    }

    scene.clearColor = Color3.FromHexString("#121212");;

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

export default function Cross(props) {
    const classes = useStyles();
    const history = useHistory();
    //const [isActive, setIsActive] = useState(false);

    useEffect(() => {
        
    });

    return (
        <div style={{overflow: 'hidden'}}>
            <SceneComponent antialias onSceneReady={onSceneReady} onRender={onRender} id='my-canvas' className={classes.scene}/>
        </div>
    );
}