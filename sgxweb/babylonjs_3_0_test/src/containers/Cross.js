import React, { useState, useEffect } from "react";
//import { API } from "aws-amplify";
import { useHistory } from "react-router-dom";
import { makeStyles } from '@material-ui/core/styles';
import { ArcRotateCamera, Vector3, Color3, DirectionalLight, MeshBuilder, Mesh, StandardMaterial } from 'babylonjs';
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
const xMin = 1, xMax = 19448;
const zMin = 43983, zMax = 44196;
var nodes = [];

const onSceneReady = (scene) => {

    scene.getAnimationRatio();
    scene.ambientColor = new Color3(0.35, 0.35, 0.35);
    //var plane_x = null, plane_z = null;
    //var plane_x_two = null, plane_z_two = null;
    // This creates and positions a free camera (non-mesh)
    var camera = new ArcRotateCamera("Camera", 3 * Math.PI / 2, Math.PI / 3, 25, Vector3.Zero(), scene);
    camera.lowerRadiusLimit = 3;
    camera.inertia = 0.2;

    // This targets the camera to scene origin
    camera.setTarget(Vector3.Zero());

    const canvas = scene.getEngine().getRenderingCanvas();
    
    // This attaches the camera to the canvas
    camera.attachControl(canvas, true);

    var light = new DirectionalLight("Dir0", new Vector3(1, -1, 1), scene);
    //var light = new PointLight("Dir0", new Vector3(0, -1, 0), scene);
    light.specular = new Color3(0, 0, 0);
    // Default intensity is 1. Let's dim the light a small amount
    light.intensity = 0.9;
    light.parent = camera;

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

    //console.log(scene);
    var advancedTexture = GUI.AdvancedDynamicTexture.CreateFullscreenUI("UI", true, scene);

    var rect1 = new GUI.Rectangle();
    configureLabel(rect1, plane_x, -150);

    var label1 = new GUI.TextBlock();
    label1.text = "X: "+xMin;
    rect1.addControl(label1);

    var rect2 = new GUI.Rectangle();
    configureLabel(rect2, plane_z, -150);

    var label2 = new GUI.TextBlock();
    label2.text = "Z: "+zMin;
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
    let materialMap = new Map();

    let x_min = 43983, x_max = 44196
    let y_min = 1, y_max = 19448
    let z_min = 15, z_max = 935.25

    function shadeColor(color, percent) {   
        var f=parseInt(color.slice(1),16),t=percent<0?0:255,p=percent<0?percent*-1:percent,R=f>>16,G=f>>8&0x00FF,B=f&0x0000FF;
        return "#"+(0x1000000+(Math.round((t-R)*p)+R)*0x10000+(Math.round((t-G)*p)+G)*0x100+(Math.round((t-B)*p)+B)).toString(16).slice(1);
    }

    const HSBToRGB = (h, s, b) => {
        s /= 100;
        b /= 100;
        const k = (n) => (n + h / 60) % 6;
        const f = (n) => b * (1 - s * Math.max(0, Math.min(k(n), 4 - k(n), 1)));
        return [LinearInterpolation(0,255,0,1,255 * f(5)), LinearInterpolation(0,255,0,1,255 * f(3)), LinearInterpolation(0,255,0,1,255 * f(1))];
    };

    function interpolateColor(value) {

        let h = LinearInterpolation(z_min, z_max, 180, 0, value);
        //let s = LinearInterpolation(z_min, z_max, 100, 100, value);
        //let b = LinearInterpolation(z_min, z_max, 100, 100, value);
        return HSBToRGB(Math.ceil(h / 10) * 10, 100, 100);
    }

    function componentToHex(c) {
        var hex = c.toString(16);
        return hex.length == 1 ? "0" + hex : hex;
    }

    function rgbToHex(r, g, b) {
        return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
    }

    fetch("./data.json",{
        headers : { 
          'Content-Type': 'application/json',
          'Accept': 'application/json'
        }
    })
    .then(response => response.json())
      .then(
        (data) => {
            //console.log(data);
            for(let i = 0; i < data.length; i++){
                try {
                    let price = parseFloat(data[i]['TotalPrice']);
                    if(price >= z_min){
                        let height = LinearInterpolation(z_min, z_max, 0.25, 5, price);
                        let box = null;
                        if(!heightMap.has(height)){
                            box = MeshBuilder.CreateBox("box_"+i, {width: cubeSize, depth: cubeSize, height: height });
                            heightMap[height] = nodes.length;
                        }
                        else
                            box = nodes[heightMap[height]].createInstance("box_"+i);
                        box.position.x = LinearInterpolation(y_min, y_max, rightBound+offset, leftBound-offset, parseFloat(data[i]['Item']));
                        box.position.y = height / 2.0;
                        box.position.z = LinearInterpolation(x_min, x_max, rightBound+offset, leftBound-offset, parseFloat(data[i]['ShipDate'].replace(',','')));

                        let color = interpolateColor(price);
                        let color_index = rgbToHex(color[0], color[1], color[2]);

                        if(materialMap.get(color_index) == null){
                            materialMap.set(color_index, new StandardMaterial("sm", scene));
                        }
                        //materialMap.get(color).emissiveColor = new Color3(color[0], color[1], color[2]);
                        materialMap.get(color_index).emissiveColor = new Color3.FromHexString(shadeColor(rgbToHex(color[0], color[1], color[2]), -0.25));
                        materialMap.get(color_index).diffuseColor = new Color3(color[0], color[1], color[2]);
                        materialMap.get(color_index).ambientColor = new Color3(color[0], color[1], color[2]);
                        materialMap.get(color_index).specularColor = new Color3(color[0], color[1], color[2]);
                        box.material = materialMap.get(color_index);
                        box.material.needDepthPrePass = true;

                        box.freezeWorldMatrix();
                        box.convertToUnIndexedMesh();

                        nodes.push(box);
                    }
                }
                catch(error) {
                    console.error(error);
                }
            }
            console.log(materialMap.size);
            console.log(nodes.length);
        },
        (error) => {
            console.log(error);
        }
      )


    scene.clearColor = Color3.FromHexString("#121212");;

    var ground = Mesh.CreateGround("ground", 50, 50, 1, scene, false);
    var groundMaterial = new StandardMaterial("ground", scene);
    groundMaterial.emissiveColor = Color3.FromHexString("#121212");
    groundMaterial.diffuseColor = Color3.Black();
    groundMaterial.ambientColor = Color3.Black();
    groundMaterial.specularColor = Color3.Black();
    groundMaterial.disableLightning = true;
    ground.material = groundMaterial;
    ground.position.y = -0.05;

    var startingPoint;
    var currentMesh;
    var getGroundPosition = function () {
        // Use a predicate to get position on the ground
        var pickinfo = scene.pick(scene.pointerX, scene.pointerY, function (mesh) { return mesh == ground; });
        if (pickinfo.hit) {
            return pickinfo.pickedPoint;
        }

        return null;
    }

    var validateDragX = function (targetPosition) {
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
        label1.text = "X: " + Math.round(LinearInterpolation(leftBound, rightBound, xMin, xMax, plane_x.position.x)).toString();
    };

    var validateDragZ = function (targetPosition) {
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
        label2.text = "Z: " + Math.round(LinearInterpolation(leftBound, rightBound, zMin, zMax, plane_z.position.z)).toString();
    };

    var validateDragX_two = (targetPosition)=>{
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
        label3.text = "X: " + Math.round(LinearInterpolation(leftBound, rightBound, xMin, xMax, plane_x_two.position.x)).toString();
    }

    var validateDragZ_two = (targetPosition)=>{
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
        label4.text = "Z: " + Math.round(LinearInterpolation(leftBound, rightBound, zMin, zMax, plane_z_two.position.z)).toString();
    }

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

    function evaluateX_two(i) {
        return nodes[i].position.x < plane_x.position.x && nodes[i].position.x > plane_x_two.position.x;
    }

    function evaluateZ_two(i) {
        return nodes[i].position.z < plane_z.position.z && nodes[i].position.z > plane_z_two.position.z;
    }

    var selected = false;
    var currentName = null;
    scene.onPointerDown = function (evt, pickResult) {

        if(pickResult.hit){
            if(pickResult.pickedMesh.name == "plane_x" ||
                pickResult.pickedMesh.name == "plane_z" ||
                pickResult.pickedMesh.name == "plane_x_two" ||
                pickResult.pickedMesh.name == "plane_z_two"){
                //console.log(pickResult.pickedMesh.name);
                camera.inputs.remove(camera.inputs.attached.pointers);
                currentName = pickResult.pickedMesh.name;
                selected = true;
            }
        }

    };

    scene.onPointerUp = function (evt) {
        if(selected){
            camera.inputs.addPointers();
            selected = false;
            filterNodes();
        }
    };

    scene.onPointerMove = function (evt) {
        if(selected){
            startingPoint = getGroundPosition(evt);
            if(currentName == "plane_x")
                validateDragX(startingPoint);
            if(currentName == "plane_z")
                validateDragZ(startingPoint);
            if(currentName == "plane_x_two")
                validateDragX_two(startingPoint);
            if(currentName == "plane_z_two")
                validateDragZ_two(startingPoint);
        }
    };

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
            <SceneComponent antialias onSceneReady={onSceneReady} onRender={onRender} id='my-canvas' style={{width: '100%', height: window.innerHeight-59}}/>
        </div>
    );
}