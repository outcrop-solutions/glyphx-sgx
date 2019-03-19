
var scene;
var tags = new Map();
var displayedTags = new Map();
var filterManager = null;
var axisControl = null;
var advancedTexture = null;
var selectedGlyphs = new Array();
var isGeo = false;
var ground = null;
var si_control = false;
var si_button = null;
var expandImage = null;
var compressImage = null;
var si_pressed = false;
var currentSiGroup = null;
var unique_locations = new Map();
var rootIDs = new Array();

var si_enabled = true;
var si_sensitivity = 125;

//put the postmessage code here..

window.addEventListener("message", receiveMessage, false);

function receiveMessage(event)
{
	console.log('iframe function called');
	
	switch(event.data.action){
		case 'filter':
			filterGlyphs(event.data.args);
			break;
		case 'clear':
			filterGlyphs([]);
			break;
		case 'toggleAxis':
			isAxisVisible() ? enableAxisLines(0) : enableAxisLines(1);
			break;
		default:
	}
}

function updateXYZCoordinates(data, x, y, z) {
    console.log("updateXYZCoordinates was called");
    tags.set(axisControl.getX().id, "X / "+x);
    tags.set(axisControl.getY().id, "Y / "+y);
    tags.set(axisControl.getZ().id, "Z / "+z);
    advancedTexture.removeControl(displayedTags.get(axisControl.getX().id));
    advancedTexture.removeControl(displayedTags.get(axisControl.getY().id));
    advancedTexture.removeControl(displayedTags.get(axisControl.getZ().id));
    displayedTags.set(axisControl.getX().id, createLabel(axisControl.getX(), "X / "+x));
    displayedTags.set(axisControl.getY().id, createLabel(axisControl.getY(), "Y / "+y));
    displayedTags.set(axisControl.getZ().id, createLabel(axisControl.getZ(), "Z / "+z));
    unique_locations.clear();
    unique_locations = filterManager.updateXYZCoordinates(data);
    if(isGeo){
        ground.dispose();
        enableAxisLines(1);
        isGeo = false;
    }
    removeSIButton();
    hideAllTags();
}

function closeSceneView(){
	console.log("Closing out scene animations");
    filterManager.clearSelections();
}

function getSelectedGlyphIDs() {
    return filterManager.getSelectedIDs();
}

function filterGlyphs(glyphIDs) {

    if(glyphIDs.length != 0){
        filterManager.filterGlyphs(glyphIDs);
        var trgt = scene.activeCameras[0].target;
        if(unique_locations.get(trgt.x+""+trgt.y+""+trgt.z) != null && !glyphIDs.includes(unique_locations.get(trgt.x+""+trgt.y+""+trgt.z)[0])){
            console.log("Remove SI Group");
            removeSIButton();
        }
        hideAllTags();
    }else{
        filterManager.clearFilters();
    }
}

function hideAllTags(){
    this.displayedTags.forEach(function(value, key, map) {
        advancedTexture.removeControl(value);
        displayedTags.delete(key);
    });
}

var createLabel = function(mesh, tag) {
    var label = new BABYLON.GUI.Rectangle("label for " + mesh.name);
    label.background = "black"
    label.height = "20px";
    label.alpha = 0.5;
    label.width = (tag.length*10)+"px";
    //label.cornerRadius = 10;
    label.thickness = 0;
    label.linkOffsetX = (tag.length*5);
    advancedTexture.addControl(label); 
    label.linkWithMesh(mesh);

    var text1 = new BABYLON.GUI.TextBlock();
    text1.text = tag;
    text1.color = "white";
    label.addControl(text1);  

    return label;
}  

function setHideGlyphs(hide){
    filterManager.setHideGlyphs(hide);
}

function isAxisVisible() {
    return axisControl.isEnabled();
}

function enableAxisLines(enabled) {

    if(enabled && !axisControl.isEnabled()){
        axisControl.setEnabled(enabled);
        displayedTags.set(axisControl.getX().id, createLabel(axisControl.getX(), tags.get(axisControl.getX().id)));
        displayedTags.set(axisControl.getY().id, createLabel(axisControl.getY(), tags.get(axisControl.getY().id)));
        displayedTags.set(axisControl.getZ().id, createLabel(axisControl.getZ(), tags.get(axisControl.getZ().id)));
    }
    else if(!enabled && axisControl.isEnabled()){
        axisControl.setEnabled(enabled);
        advancedTexture.removeControl(displayedTags.get(axisControl.getX().id));
        displayedTags.delete(axisControl.getX().id);
        advancedTexture.removeControl(displayedTags.get(axisControl.getY().id));
        displayedTags.delete(axisControl.getY().id);
        advancedTexture.removeControl(displayedTags.get(axisControl.getZ().id));
        displayedTags.delete(axisControl.getZ().id);
    }
}

function removeSIButton(){
    if(si_control && si_button){
        si_button.removeControl(si_pressed? compressImage : expandImage);
        advancedTexture.removeControl(si_button);
        si_button = null;
        si_control = false;
        filterManager.selectionManager.si_sphere.setEnabled(0);
    }
}

function setSuperImposed(enabled, sensitivity){
    si_sensitivity = sensitivity ? sensitivity : 125;
    if(si_enabled && !enabled){
        for(var i = 0; i < rootIDs.length; i++){
            var children = filterManager.getRootGlyph(rootIDs[i]).getChildren();
            for(var j = 0; j < children.length; j++){
                children[j].setEnabled(1);
            }
        }
    }
    si_enabled = enabled;
    console.log("SI: " + si_enabled + ", " + si_sensitivity);
}

function imageExists(image_url) {
/*
    var http = new XMLHttpRequest();
    http.open('HEAD', image_url, false);
    http.send();
    return http.status != 404;
*/
}

var createScene = function (engine) {
	scene = new BABYLON.Scene(engine);
    scene.clearColor = new BABYLON.Color3(0, 0, 0);
    scene.getAnimationRatio();

	var camera = new BABYLON.ArcRotateCamera("Camera", 3 * Math.PI / 2, Math.PI / 3, 50, BABYLON.Vector3.Zero(), scene);
    camera.lowerRadiusLimit = 3;
    //camera.wheelPrecision = 5;
	camera.attachControl(canvas, true);
    scene.activeCameras.push(camera);
    camera.viewport = new BABYLON.Viewport(0, 0, 1.0, 1.0);

    var camera2 = new BABYLON.ArcRotateCamera("Camera", 3 * Math.PI / 2, Math.PI / 3, 20, BABYLON.Vector3.Zero(), scene);
    camera2.setTarget(BABYLON.Vector3.Zero());
    camera2.attachControl(canvas, true);
    scene.activeCameras.push(camera2);
    var vp_size = 200;
    camera2.viewport = new BABYLON.Viewport(0, 1-vp_size/window.innerHeight, vp_size/window.innerWidth, vp_size/window.innerHeight);
    camera2.layerMask = 0x10000000;
    camera2.lowerRadiusLimit = 7;
    camera2.upperRadiusLimit = 7;

	//var light = new BABYLON.HemisphericLight("hemi", new BABYLON.Vector3(0, -1, 0), scene);
    var light = new BABYLON.DirectionalLight("Dir0", new BABYLON.Vector3(1, -1, 1), scene);
    //var light = new BABYLON.PointLight("Dir0", new BABYLON.Vector3(0, -1, 0), scene);
    light.specular = new BABYLON.Color3(0, 0, 0);
    // Default intensity is 1. Let's dim the light a small amount
    light.intensity = 0.6;
    light.parent = camera;

    advancedTexture = BABYLON.GUI.AdvancedDynamicTexture.CreateFullscreenUI("ui1");

    var center = BABYLON.Mesh.CreateSphere('Sphere', 16, 0.125, scene);
    center.layerMask = 0x10000000;

    var x = BABYLON.Mesh.CreateCylinder("Cylinder", 3, 0.125, 0.125, 12, 1, scene);
    x.rotation = new BABYLON.Vector3(0, 0, Math.PI/2);
    var xcone1 = BABYLON.MeshBuilder.CreateCylinder("Cone", {height: 0.5, diameterTop: 0, diameterBottom: 0.25, tessellation: 12}, scene);
    xcone1.position.x = -1.5;
    xcone1.rotation = new BABYLON.Vector3(0, 0, 1.57);
    var xcone2 = xcone1.createInstance("xcone2");
    xcone2.position.x = 1.5;
    xcone2.rotation = new BABYLON.Vector3(0, 0, -1.57);
    var red = new BABYLON.StandardMaterial("axis_red", scene);
    red.emissiveColor = new BABYLON.Color3(1,0,0);
    red.diffuseColor = new BABYLON.Color3(1,0,0);
    x.material = red;
    xcone1.material = red;
    x.parent = center;
    xcone1.parent = center;
    xcone2.parent = center;
    x.layerMask = 0x10000000;
    xcone1.layerMask = 0x10000000;
    xcone2.layerMask = 0x10000000;

    var y = BABYLON.Mesh.CreateCylinder("Cylinder", 3, 0.125, 0.125, 12, 1, scene);
    y.rotation = new BABYLON.Vector3(Math.PI/2, 0, 0);
    var ycone1 = BABYLON.MeshBuilder.CreateCylinder("Cone", {height: 0.5, diameterTop: 0, diameterBottom: 0.25, tessellation: 12}, scene);
    ycone1.position.z = -1.5;
    ycone1.rotation = new BABYLON.Vector3(-1.57, 0, 0);
    var ycone2 = ycone1.createInstance("ycone2");
    ycone2.position.z = 1.5;
    ycone2.rotation = new BABYLON.Vector3(1.57, 0, 0);
    var green = new BABYLON.StandardMaterial("axis_green", scene);
    green.emissiveColor = new BABYLON.Color3(0,1,0);
    green.diffuseColor = new BABYLON.Color3(0,1,0);
    y.material = green;
    ycone1.material = green;
    y.parent = center;
    ycone1.parent = center;
    ycone2.parent = center;
    y.layerMask = 0x10000000;
    ycone1.layerMask = 0x10000000;
    ycone2.layerMask = 0x10000000;

    var z = BABYLON.Mesh.CreateCylinder("Cylinder", 3, 0.125, 0.125, 12, 1, scene);
    z.rotation = new BABYLON.Vector3(00, 0, 0);
    var zcone1 = BABYLON.MeshBuilder.CreateCylinder("Cone", {height: 0.5, diameterTop: 0.25, diameterBottom: 0, tessellation: 12}, scene);
    zcone1.position.y = -1.5;
    zcone1.rotation = new BABYLON.Vector3(0, 0, 0);
    var zcone2 = BABYLON.MeshBuilder.CreateCylinder("Cone", {height: 0.5, diameterTop: 0, diameterBottom: 0.25, tessellation: 12}, scene);
    zcone2.position.y = 1.5;
    zcone2.rotation = new BABYLON.Vector3(0, 1.57, 0);
    var blue = new BABYLON.StandardMaterial("axis_blue", scene);
    blue.emissiveColor = new BABYLON.Color3(0,0,1);
    blue.diffuseColor = new BABYLON.Color3(0,0,1);
    z.material = blue;
    zcone1.material = blue;
    zcone2.material = blue;
    z.parent = center;
    zcone1.parent = center;
    zcone2.parent = center;
    z.layerMask = 0x10000000;
    zcone1.layerMask = 0x10000000;
    zcone2.layerMask = 0x10000000;

    scene.cameraToUseForPointers = camera;

    var axis_center = center.createInstance("axis_center");
    axis_center.position = new BABYLON.Vector3(-180, 0, -180);
    axis_center.scaling = new BABYLON.Vector3(30, 30, 30);

    var x_axis = x.createInstance("x_axis");
    x_axis.rotation = new BABYLON.Vector3(0, 0, Math.PI/2);
    x_axis.position = new BABYLON.Vector3(6, 0, 0);
    x_axis.scaling = new BABYLON.Vector3(0.5, 4, 0.5);
    var x_axis_cone = xcone1.createInstance("x_axis_cone");
    x_axis_cone.position.x = 12.0;
    x_axis_cone.rotation = new BABYLON.Vector3(0, 0, -1.57);
    x_axis_cone.scaling = new BABYLON.Vector3(0.5, 1, 0.5);
    x_axis.parent = axis_center;
    x_axis_cone.parent = axis_center;

    var y_axis = y.createInstance("y_axis");
    y_axis.rotation = new BABYLON.Vector3(Math.PI/2, 0, 0);
    y_axis.position = new BABYLON.Vector3(0, 0, 6);
    y_axis.scaling = new BABYLON.Vector3(0.5, 4, 0.5);
    var y_axis_cone = ycone1.createInstance("y_axis_cone");
    y_axis_cone.position.z = 12.0;
    y_axis_cone.rotation = new BABYLON.Vector3(1.57, 0, 0);
    y_axis_cone.scaling = new BABYLON.Vector3(0.5, 1, 0.25);
    y_axis.parent = axis_center;
    y_axis_cone.parent = axis_center;

    var z_axis = z.createInstance("z_axis");
    z_axis.rotation = new BABYLON.Vector3(0, 0, 0);
    z_axis.position = new BABYLON.Vector3(0, 6, 0);
    z_axis.scaling = new BABYLON.Vector3(0.5, 4, 0.5);
    var z_axis_cone = zcone2.createInstance("z_axis_cone");
    z_axis_cone.position.y = 12.0;
    z_axis_cone.rotation = new BABYLON.Vector3(0, 1.57, 0);
    z_axis_cone.scaling = new BABYLON.Vector3(0.5, 1, 0.5);
    z_axis.parent = axis_center;
    z_axis_cone.parent = axis_center;

    axisControl = new SYNGLYPHX.AxisControl(axis_center);
    axisControl.setEndPoints(x_axis_cone, y_axis_cone, z_axis_cone);

    var image = new BABYLON.GUI.Image("but", "./sgx_inside.png");
    image.stretch = BABYLON.GUI.Image.STRETCH_UNIFORM;
    image.horizontalAlignment = BABYLON.GUI.Control.HORIZONTAL_ALIGNMENT_RIGHT;
    image.verticalAlignment = BABYLON.GUI.Control.VERTICAL_ALIGNMENT_TOP;
    image.paddingTop = "10px";
    image.width = 0.16;
    image.height = "40px";
    advancedTexture.addControl(image); 

    function createBaseImage(image, width, height) {
        
        var base_image_light = new BABYLON.DirectionalLight("Dir0", new BABYLON.Vector3(0, -1, 0), scene);
        base_image_light.specular = new BABYLON.Color3(0, 0, 0);
        base_image_light.intensity = 1.0;

        var baseImage = new BABYLON.StandardMaterial("ground", scene);
        baseImage.diffuseTexture = new BABYLON.Texture(image, scene);

        // create a built-in "ground" shape; its constructor takes the same 5 params as the sphere's one
        ground = BABYLON.Mesh.CreateGround('ground', width, height, 2, scene);
        ground.position = new BABYLON.Vector3(0, -3, 0);
        ground.material = baseImage;

        base_image_light.includedOnlyMeshes.push(ground);
    }

    var createAxisLines = function(x, y, z) {
        tags.set(x_axis_cone.id, "X / "+x);
        tags.set(y_axis_cone.id, "Y / "+y);
        tags.set(z_axis_cone.id, "Z / "+z);
        displayedTags.set(x_axis_cone.id, createLabel(x_axis_cone, "X / "+x));
        displayedTags.set(y_axis_cone.id, createLabel(y_axis_cone, "Y / "+y));
        displayedTags.set(z_axis_cone.id, createLabel(z_axis_cone, "Z / "+z));
    }

    function createShape(geometry, color, root) {
        mult = root && geometry == 7 ? 3 : 1;
        switch(geometry){
            case 1:
                return BABYLON.Mesh.CreateBox("Cube"+color, 1.0*mult, scene);
            case 3:
                return BABYLON.Mesh.CreateSphere("Sphere"+color, 16, 1, scene);
            case 5:
                return BABYLON.MeshBuilder.CreateCylinder("Cone"+color, {height: 1, diameterTop: 0, tessellation: 12}, scene);
            case 7:
                return BABYLON.Mesh.CreateTorus("Torus"+color, 1*mult, 0.2, 16, scene);
            case 9:
                return BABYLON.MeshBuilder.CreatePolyhedron("Dodecahedron"+color, {type: 2, size: 0.5}, scene);
            case 11:
                return BABYLON.MeshBuilder.CreatePolyhedron("Octahedron"+color, {type: 1, size: 0.5}, scene);
            case 13:
                return BABYLON.MeshBuilder.CreatePolyhedron("Tetrahedron"+color, {type: 0, size: 0.5}, scene);
            case 15:
                return BABYLON.MeshBuilder.CreatePolyhedron("Icosahedron"+color, {type: 3, size: 0.5}, scene);
            case 16:
                return BABYLON.MeshBuilder.CreateCylinder("Pin"+color, {diameterTop: 0.5, diameterBottom: 0, tessellation: 12}, scene);
            case 19:
                return BABYLON.Mesh.CreateCylinder("Cylinder"+color, 3, 0.25, 0.25, 12, 1, scene);
            default:
                return BABYLON.Mesh.CreateBox("Cube"+color, 1.0*mult, scene);
        }
    }

    /*
    Topologies:
            Cube = 1, PRIORITY 5
            Sphere = 2, DONE
            Torus = 3, PRIORITY 1
            Cylinder = 4, CONSOLIDATE 3
            Pin = 5, PRIORITY 4
            Rod = 6, CONSOLIDATE 2
            Point = 7, UNUSED
    */

    Math.radians = function(degrees) {
        return degrees * Math.PI / 180;
    };
 
    function translateMeshPosition(r, p, isRoot, topo) {
        if(isRoot){
            return [p[0], p[2], p[1]]
        }

        if(topo == 2){
            pos = [p[0],90-p[1],p[2]]

            s = Math.radians(pos[0])
            t = Math.radians(pos[1])

            x = r * Math.cos(s) * Math.sin(t)
            y = r * Math.sin(s) * Math.sin(t)
            z = r * Math.cos(t)

            return [x*-1, z, y*-1] //Babylon coordinate plane has an inversed y and z position from ours
        }
        else if(topo == 12){ //topo 12 is used specifically to make the High School Profiles viz work. Should be unused everywhere else
            pos = [p[0],90-p[1],p[2]]

            s = Math.radians(pos[0])
            t = Math.radians(pos[1])

            x = r * Math.cos(s) * Math.sin(t)
            y = r * Math.sin(s) * Math.sin(t)
            z = pos[2]//r * Math.cos(t)

            if(z == 1.5){
                return [p[0]/100, z, p[1]/100]
            }

            return [x*-1, z, y*-1] //Babylon coordinate plane has an inversed y and z position from ours
        }
        return [p[0]/100,p[2]/100,p[1]/100];
    }

    function adjustMeshScaling(g, s, isRoot) {
        if(isRoot){
            return [s[0], s[2], s[1]] //Same coordinate plane issue as with position
        }
        else if(g == 7){
            return [s[0]/2, s[2]/2, s[1]/2];
        }
        return [s[0]/3, s[2]/3, s[1]/3];
    }

    function translateMeshRotation(g, s, r, isRoot, topo) {

        x = Math.radians(r[0]);
        y = Math.radians(r[1]);
        z = Math.radians(r[2]);

        m = s[1]/s[2];
        if(m != 1 && topo == 2){
            return [x, y, Math.PI/2];
        }
        return [x, y, z];
    }
    /*
    BABYLON.Effect.ShadersStore["fadePixelShader"] =
            "precision highp float;" +
            "varying vec2 vUV;" +
            "uniform sampler2D textureSampler; " +
            "uniform float fadeLevel; " +
            "void main(void){" +
            "vec4 baseColor = texture2D(textureSampler, vUV) * fadeLevel;" +
            "baseColor.a = 1.0;" +
            "gl_FragColor = baseColor;" +
    "}";

    var fadeLevel = 1.0;
    var postProcess = new BABYLON.PostProcess("Fade", "fade", ["fadeLevel"], null, 1.0, camera);
    postProcess.onApply = (effect) => {
        effect.setFloat("fadeLevel", fadeLevel);
    };  
    */

    function findParent(mesh){
        if(mesh.parent == null){
            return mesh;
        }
        return findParent(mesh.parent);
    }

    expandImage = new BABYLON.GUI.Image("expand_icon", "expand-64.png");
    expandImage.stretch = BABYLON.GUI.Image.STRETCH_FILL;
    expandImage.horizontalAlignment = BABYLON.GUI.Control.HORIZONTAL_ALIGNMENT_LEFT;
    compressImage = new BABYLON.GUI.Image("compress_icon", "compress-64.png");
    compressImage.stretch = BABYLON.GUI.Image.STRETCH_FILL;
    compressImage.horizontalAlignment = BABYLON.GUI.Control.HORIZONTAL_ALIGNMENT_LEFT;
    
    function createSuperimposedGlyphIndicator(mesh){
        //Sphere
        var sphere = filterManager.selectionManager.superImposedIndicator(mesh);

        si_button = new BABYLON.GUI.Button("si_button");
        // Adding image
        si_button.addControl(expandImage);

        //si_button = BABYLON.GUI.Button.CreateImageOnlyButton("but", "expand-64.png");
        si_button.width = "30px";
        si_button.height = "30px";
        si_button.thickness = 0;
        si_button.cornerRadius = 0;
        //console.log(unique_locations.get(mesh.position.x+""+mesh.position.y+""+mesh.position.z));

        si_button.onPointerUpObservable.add(function() {
            //console.alert("Show superimposed glyphs");
            if(si_pressed){
                console.log("add expanded image");
                filterManager.clearSelections();
                si_button.removeControl(compressImage);
                si_button.addControl(expandImage);
                currentSiGroup = null;
                filterManager.fadeGlyphs(mesh, currentSiGroup);
                si_pressed = false;
                filterManager.addSingleSelection(mesh, findParent(mesh));
            }
            else{
                console.log("add compressed image");
                si_button.removeControl(expandImage);
                si_button.addControl(compressImage);
                currentSiGroup = unique_locations.get(mesh.position.x+""+mesh.position.y+""+mesh.position.z);
                filterManager.fadeGlyphs(mesh, currentSiGroup);
                si_pressed = true;
                filterManager.clearSelections();
            }
        });
        advancedTexture.addControl(si_button);
        si_button.linkWithMesh(sphere);
        si_button.linkOffsetY = 10;
        si_control = true;
    }

    var meshMap = new Map();
    var materialMap = new Map();
    var allGlyphs = new Map();
    var transGlyphs = new Map();
    var topoIDs = new Map();
    var glyphURLs = new Map();
    filterManager = new SYNGLYPHX.FilterManager(camera);

    var avg_x = 0;
    var avg_y = 0;
    var avg_z = 0;

    $.ajax({url: json_path, success: function(data){
        /*
        // Environment Texture
        var hdrTexture = new BABYLON.HDRCubeTexture("test.hdr", scene, 512);

        // Skybox
        var hdrSkybox = BABYLON.Mesh.CreateBox("hdrSkyBox", 1000.0, scene);
        var hdrSkyboxMaterial = new BABYLON.PBRMaterial("skyBox", scene);
        hdrSkyboxMaterial.backFaceCulling = false;
        hdrSkyboxMaterial.reflectionTexture = hdrTexture.clone();
        hdrSkyboxMaterial.reflectionTexture.coordinatesMode = BABYLON.Texture.SKYBOX_MODE;
        hdrSkyboxMaterial.microSurface = 1.0;
        hdrSkyboxMaterial.cameraExposure = 0.6;
        hdrSkyboxMaterial.cameraContrast = 1.6;
        hdrSkyboxMaterial.disableLighting = true;
        hdrSkybox.material = hdrSkyboxMaterial;
        hdrSkybox.infiniteDistance = true;
        */
        var rootNum = 0;
        var bg = data.background;
        var urls = data.urls;
        //console.log(Object.keys(data));
        //console.log(Object.keys(urls));
        if(bg["image"].length >= 5) {
            var splt = json_path.substring(json_path.lastIndexOf('%2F')+3, json_path.length);
            var img_path = json_path.replace(splt,bg["image"]);
            console.log(img_path);
            createBaseImage(img_path, bg["width"], bg["height"]);
        }
        createAxisLines(bg["axis"][0], bg["axis"][1], bg["axis"][2]);
        if(bg["geo"]) {
            enableAxisLines(0);
            isGeo = true;
        }

        var obj = data.nodes;
        for (var i = 0; i < obj.length; i++){

            if(obj[i]["alpha"] != 0){
                topoIDs.set(obj[i]["id"], obj[i]["topo"]);

                if(obj[i]["color"] == "#ffffff"){
                    obj[i]["color"] = "#e5e5e5";
                }

                var root = obj[i]["parent_id"] == 0 ? "root" : "";
                var mesh;
                var tmesh;
                if(meshMap.get(root+obj[i]["geometry"]+obj[i]["color"]) == null){

                    meshMap.set(root+obj[i]["geometry"]+obj[i]["color"], createShape(obj[i]["geometry"], obj[i]["color"], obj[i]["parent_id"] == 0));
                    mesh = meshMap.get(root+obj[i]["geometry"]+obj[i]["color"]);

                    meshMap.set("t"+root+obj[i]["geometry"]+obj[i]["color"], createShape(obj[i]["geometry"], obj[i]["color"], obj[i]["parent_id"] == 0));
                    tmesh = meshMap.get("t"+root+obj[i]["geometry"]+obj[i]["color"]);

                    /*if(obj[i]["parent_id"] != 0){
                        mesh.addLODLevel(150, null);
                        tmesh.addLODLevel(150, null);
                    }*/
                }
                else{
                    mesh = meshMap.get(root+obj[i]["geometry"]+obj[i]["color"]).createInstance("mesh"+i);
                    tmesh = meshMap.get("t"+root+obj[i]["geometry"]+obj[i]["color"]).createInstance("tmesh"+i);
                }

                if(obj[i]["id"] in urls){
                    glyphURLs.set(mesh.id, urls[obj[i]["id"]]);
                }

                var topo = obj[i]["parent_id"] != 0 ? topoIDs.get(obj[i]["parent_id"]) : 0; 
                pos = translateMeshPosition(0.7, obj[i]["pos"], obj[i]["parent_id"] == 0, topo)
                mesh.position = new BABYLON.Vector3(pos[0], pos[1], pos[2]);
                tmesh.position = new BABYLON.Vector3(pos[0], pos[1], pos[2]);

                scale = adjustMeshScaling(obj[i]["geometry"], obj[i]["scale"], obj[i]["parent_id"] == 0)
                mesh.scaling = new BABYLON.Vector3(scale[0], scale[1], scale[2]);
                tmesh.scaling = new BABYLON.Vector3(scale[0], scale[1], scale[2]);
                
                rotate = translateMeshRotation(obj[i]["geometry"], obj[i]["scale"], obj[i]["rotate"], obj[i]["parent_id"] == 0, topo);
                mesh.rotation = new BABYLON.Vector3(rotate[0], rotate[1], rotate[2]);
                tmesh.rotation = new BABYLON.Vector3(rotate[0], rotate[1], rotate[2]);

                allGlyphs.set(obj[i]["id"], mesh);
                transGlyphs.set(obj[i]["id"], tmesh);
                if(obj[i]["parent_id"] != 0){
                    mesh.parent = allGlyphs.get(obj[i]["parent_id"]);
                    tmesh.parent = transGlyphs.get(obj[i]["parent_id"]);
                }
                else{
                    var ignore = false;
                    if(unique_locations.get(pos[0]+""+pos[1]+""+pos[2]) == null){
                        unique_locations.set(pos[0]+""+pos[1]+""+pos[2], new Array()); 
                    }
                    else{
                        ignore = true;
                    }
                    filterManager.addRootGlyph(++rootNum, mesh, tmesh, ignore);
                    unique_locations.get(pos[0]+""+pos[1]+""+pos[2]).push(rootNum);
                    rootIDs.push(rootNum);
                    avg_x += pos[0];
                    avg_y += pos[1];
                    avg_z += pos[2];
                    mesh.scaling = new BABYLON.Vector3(scale[0]*2, scale[1]*2, scale[2]*2);
                    tmesh.scaling = new BABYLON.Vector3(scale[0]*2, scale[1]*2, scale[2]*2);
                    //mesh.actionManager = new BABYLON.ActionManager(scene);
                    //mesh.actionManager.registerAction(action);
                }
                mesh.freezeWorldMatrix();
                tmesh.freezeWorldMatrix();

                if(materialMap.get(obj[i]["color"]) == null){
                    materialMap.set(obj[i]["color"], new BABYLON.StandardMaterial("sm", scene));
                    materialMap.set("t"+obj[i]["color"], new BABYLON.StandardMaterial("tsm", scene));
                }

                function shadeColor(color, percent) {   
                    var f=parseInt(color.slice(1),16),t=percent<0?0:255,p=percent<0?percent*-1:percent,R=f>>16,G=f>>8&0x00FF,B=f&0x0000FF;
                    return "#"+(0x1000000+(Math.round((t-R)*p)+R)*0x10000+(Math.round((t-G)*p)+G)*0x100+(Math.round((t-B)*p)+B)).toString(16).slice(1);
                }
                materialMap.get(obj[i]["color"]).emissiveColor = new BABYLON.Color3.FromHexString(shadeColor(obj[i]["color"], -0.5));
                materialMap.get(obj[i]["color"]).diffuseColor = new BABYLON.Color3.FromHexString(obj[i]["color"]);
                materialMap.get("t"+obj[i]["color"]).emissiveColor = new BABYLON.Color3.FromHexString(shadeColor(obj[i]["color"], -0.5));
                materialMap.get("t"+obj[i]["color"]).diffuseColor = new BABYLON.Color3.FromHexString(obj[i]["color"]);
                mesh.material = materialMap.get(obj[i]["color"]);
                mesh.material.needDepthPrePass = true;
                tmesh.material = materialMap.get("t"+obj[i]["color"]);
                tmesh.material.needDepthPrePass = true;
                tmesh.visibility = 0.05;
                tags.set(mesh.id, obj[i]["tag"]);
            }
        }

        console.log("URL Count: " + glyphURLs.size);

        camera.setTarget(new BABYLON.Vector3(avg_x/filterManager.rootCount(),avg_y/filterManager.rootCount(),avg_z/filterManager.rootCount()));
        camera.beta = 1.0;
        camera2.alpha = camera.alpha;
        camera2.beta = camera.beta;

        console.log("Glyph Count:" + filterManager.rootCount() + ", Unique Locations:" + unique_locations.size);

        var vis = 0;
        filterManager.rootGlyphs.forEach(function(value, key, map) {
            vis += value.isEnabled() ? 1 : 0;
        });
        console.log("Enabled Glyphs: " + vis);

    }});

    var selection_box = null;
    function createSelectionBox(start, end) {
        function topLeftRelativeToCanvas(start, end) {
            var centerX = canvas.width/2;
            var centerY = canvas.height/2;
            var x = Math.min(start[0], end[0])+(Math.abs(start[0]-end[0])/2);
            var y = Math.min(start[1], end[1])+(Math.abs(start[1]-end[1])/2);
            return [x-centerX, y-centerY];
        }
        selection_box = new BABYLON.GUI.Rectangle();
        selection_box.width = Math.abs(start[0]-end[0])+"px";
        selection_box.height = Math.abs(start[1]-end[1])+"px";
        selection_box.thickness = 0;
        selection_box.background = "gray";
        selection_box.alpha = 0.5; 
        var pos = topLeftRelativeToCanvas(start, end);
        selection_box.top = pos[1];
        selection_box.left = pos[0];
        advancedTexture.addControl(selection_box);  
    }

    var startPoint;
    var box_selection = false;
    var lastPickedMesh = null;
    scene.onPointerDown = function (evt, pickResult) {
        if (evt.shiftKey) {
            filterManager.clearSelections();
            //console.log("Start Point - X: " + scene._pointerX + " Y: " + scene._pointerY);
            camera.inputs.remove(camera.inputs.attached.pointers);
            camera2.inputs.remove(camera2.inputs.attached.pointers);
            box_selection = true;
            startPoint = [scene._pointerX, scene._pointerY];
        }
        else if (pickResult.hit) {
            var ignoredMats = ['tsm','sim','sgm','axis_red','axis_green','axis_blue'];
            if(pickResult.pickedMesh.name != 'ground' && pickResult.pickedMesh.name != 'tagplane' && !ignoredMats.includes(pickResult.pickedMesh.material.name)){
                lastPickedMesh = pickResult.pickedMesh;
                var mesh = findParent(pickResult.pickedMesh);
                console.log(currentSiGroup+", "+filterManager.getIdFromMesh(mesh));
                console.log(si_control);
                if(si_control && si_button && !currentSiGroup){
                    console.log("removing si_button");
                    si_button.removeControl(si_pressed? compressImage : expandImage);
                    advancedTexture.removeControl(si_button);
                    si_button = null;
                    si_control = false;
                }
                if(!currentSiGroup || currentSiGroup.includes(filterManager.getIdFromMesh(mesh))){
                    filterManager.clearSelections();
                    filterManager.addSingleSelection(pickResult.pickedMesh, mesh);
                    //camera.setTarget(new BABYLON.Vector3(mesh.position.x, mesh.position.y, mesh.position.z));
                    setMeshTarget(camera.target, mesh.position);
                    camera2.alpha = camera.alpha;
                    camera2.beta = camera.beta;
                    if(!currentSiGroup){
                        filterManager.selectionManager.superImposedIndicator(mesh).setEnabled(0);
                        //console.log("ul: "+unique_locations.get(mesh.position.x+""+mesh.position.y+""+mesh.position.z));
                        if(unique_locations.get(mesh.position.x+""+mesh.position.y+""+mesh.position.z).length > 1){
                            console.log("creating si glyph");
                            createSuperimposedGlyphIndicator(mesh);
                            filterManager.selectionManager.superImposedIndicator(mesh).setEnabled(1);
                        }
                    }
                    
                    if(bbDisplayed){
                        var childMeshes = mesh.getChildMeshes();
                        var tagString = "<table id='bbcard'>";
                        for(var i = 0; i < childMeshes.length; i++){
                            if(tags.get(childMeshes[i].id) != "No Tag"){
                                tagString += "<tr><td>" + tags.get(childMeshes[i].id) + "</td></tr>";
                            }
                        }
                        tagString += "</table>";
                        $("#card").html(tagString);
                        //$("#card").dialog( "option", "position", { my: "right bottom", at: "right bottom", of: window } );
                    }
                }
            }
        }
    };

    scene.onPointerUp = function (evt) {
        if(box_selection){
            //console.log("End Point - X: " + scene._pointerX + " Y: " + scene._pointerY);
            camera.inputs.addPointers();
            camera2.inputs.addPointers();
            box_selection = false;
            advancedTexture.removeControl(selection_box); 
            filterManager.setSelectedGlyphs(camera, startPoint, [scene._pointerX, scene._pointerY]);
            if(filterManager.getSelectedIDs().length > 0){
                //console.log(filterManager.getSelectionCenter());
                //camera.setTarget(filterManager.getSelectionCenter());
                setMeshTarget(camera.target, filterManager.getSelectionCenter());
                camera2.alpha = camera.alpha;
                camera2.beta = camera.beta;
            }
        }
    };

    var bbDisplayed = false;
    document.onkeyup = function(evt) {
        if(evt.keyCode == 68) {
            if(bbDisplayed){
                $("#card").dialog("close");
                bbDisplayed = false;
            }
            else if(lastPickedMesh != null){
                var root = findParent(lastPickedMesh);
                var childMeshes = root.getChildMeshes();
                var tagString = "<table id='bbcard'>";
                for(var i = 0; i < childMeshes.length; i++){
                    if(tags.get(childMeshes[i].id) != "No Tag"){
                        tagString += "<tr><td>" + tags.get(childMeshes[i].id) + "</td></tr>";
                    }
                }
                tagString += "</table>";
                $("#card").html(tagString);
                $("#card").dialog();
                $("#card").dialog( "option", "position", { my: "right bottom", at: "right bottom", of: window } );
                bbDisplayed = true;
            }
        }
        else if(evt.keyCode == 73) { // I
            if(!displayedTags.has(lastPickedMesh.id)){
                displayedTags.set(lastPickedMesh.id, createLabel(lastPickedMesh, tags.get(lastPickedMesh.id)));
            }
        }
        else if(evt.keyCode == 79) { // O
            if(displayedTags.has(lastPickedMesh.id)){
                advancedTexture.removeControl(displayedTags.get(lastPickedMesh.id));
                displayedTags.delete(lastPickedMesh.id);
            }
        }
        else if(evt.keyCode == 77) { // M
            //console.log(camera.radius/40);
            //setSuperImposed(si_enabled ? false : true, 50);

        }
        else if(evt.keyCode == 85) { // U
            //console.log("ID: " + lastPickedMesh.id);
            if(glyphURLs.get(lastPickedMesh.id) != null){
                //console.log("URL: " + glyphURLs.get(lastPickedMesh.id));
                var glyphurl = glyphURLs.get(lastPickedMesh.id);
                var win = window.open(glyphurl.includes("http://") || glyphurl.includes("https://") ? glyphurl : "http://" + glyphurl);
                //win.focus();
            }
        }
    }  

    scene.onPointerMove = function (evt) {
        if(box_selection){
            if(selection_box != null){
                advancedTexture.removeControl(selection_box); 
            }
            createSelectionBox(startPoint, [scene._pointerX, scene._pointerY]);
        }
    };

    function setMeshTarget(fromMesh, toMesh) {        
        var starting_radius = camera.radius;
        BABYLON.Animation.CreateAndStartAnimation("camTranslate", camera, "target", 60, 30, fromMesh, toMesh, BABYLON.Animation.ANIMATIONLOOPMODE_CONSTANT);
        BABYLON.Animation.CreateAndStartAnimation("camRadius", camera, "radius", 60, 30, camera.radius, starting_radius, BABYLON.Animation.ANIMATIONLOOPMODE_CONSTANT);      
    }

    var loops = 0;
    var observer = scene.onBeforeRenderObservable.add(function () {
        if(si_enabled){
            if(loops == 0 ){
                for(var i = 0; i < rootIDs.length; i++){
                    var distanceToCamera = (filterManager.getRootGlyph(rootIDs[i]).getBoundingInfo().boundingSphere).centerWorld.subtract(camera.globalPosition).length();
                    var children = filterManager.getRootGlyph(rootIDs[i]).getChildren();
                    for(var j = 0; j < children.length; j++){
                        children[j].setEnabled(si_sensitivity > distanceToCamera? 1 : 0);
                    }
                }
            }
            if(loops++ == 9){
                loops = 0;
            }
        }
    });

	return scene;
};
