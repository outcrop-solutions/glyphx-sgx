
var scene;
var tags = new Map();
var displayedTags = new Map();
var filterManager = null;
var axisControl = null;
var advancedTexture = null;

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

function filterGlyphs(glyphIDs) {

    if(glyphIDs.length != 0){
        filterManager.filterGlyphs(glyphIDs);
    }else{
        filterManager.clearFilters();
    }
}

var createLabel = function(mesh, tag) {
    var label = new BABYLON.GUI.Rectangle("label for " + mesh.name);
    label.background = "black"
    label.height = "20px";
    label.alpha = 0.5;
    label.width = (tag.length*10)+"px";
    label.cornerRadius = 10;
    label.thickness = 1;
    label.linkOffsetX = (tag.length*5);
    advancedTexture.addControl(label); 
    label.linkWithMesh(mesh);

    var text1 = new BABYLON.GUI.TextBlock();
    text1.text = tag;
    text1.color = "white";
    label.addControl(text1);  

    return label;
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

	//var light = new BABYLON.HemisphericLight("hemi", new BABYLON.Vector3(0, 1, 0), scene);
    var light = new BABYLON.DirectionalLight("Dir0", new BABYLON.Vector3(1, -1, 0), scene);
    // Default intensity is 1. Let's dim the light a small amount
    light.intensity = 0.7;

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
    var red = new BABYLON.StandardMaterial("sm", scene);
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
    var green = new BABYLON.StandardMaterial("sm", scene);
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
    var blue = new BABYLON.StandardMaterial("sm", scene);
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
    image.width = 0.2;
    image.height = "50px";
    advancedTexture.addControl(image); 

    function createBaseImage(image, width, height) {
        
        var baseImage = new BABYLON.StandardMaterial("ground", scene);
        baseImage.diffuseTexture = new BABYLON.Texture(image, scene);

        // create a built-in "ground" shape; its constructor takes the same 5 params as the sphere's one
        var ground = BABYLON.Mesh.CreateGround('ground', width, height, 2, scene);
        ground.position = new BABYLON.Vector3(0, -3, 0);
        ground.material = baseImage;
    }

    var createAxisLines = function(x, y, z) {
        tags.set(x_axis_cone.id, "X / "+x);
        tags.set(y_axis_cone.id, "Y / "+y);
        tags.set(z_axis_cone.id, "Z / "+z);
        displayedTags.set(x_axis_cone.id, createLabel(x_axis_cone, "X / "+x));
        displayedTags.set(y_axis_cone.id, createLabel(y_axis_cone, "Y / "+y));
        displayedTags.set(z_axis_cone.id, createLabel(z_axis_cone, "Z / "+z));
    }

    function createShape(geometry, root) {
        mult = root ? 1 : 1;
        switch(geometry){
            case 1:
                return BABYLON.Mesh.CreateBox("Cube", 1.0*mult, scene);
            case 3:
                return BABYLON.Mesh.CreateSphere('Sphere', 16, 1, scene);
            case 5:
                return BABYLON.MeshBuilder.CreateCylinder("Cone", {height: 1, diameterTop: 0, tessellation: 12}, scene);
            case 7:
                return BABYLON.Mesh.CreateTorus("Torus", 1*mult, 0.2, 16, scene);
            case 9:
                return BABYLON.MeshBuilder.CreatePolyhedron("Dodecahedron", {type: 2, size: 0.5}, scene);
            case 11:
                return BABYLON.MeshBuilder.CreatePolyhedron("Octahedron", {type: 1, size: 0.5}, scene);
            case 13:
                return BABYLON.MeshBuilder.CreatePolyhedron("Tetrahedron", {type: 0, size: 0.5}, scene);
            case 15:
                return BABYLON.MeshBuilder.CreatePolyhedron("Icosahedron", {type: 3, size: 0.5}, scene);
            case 16:
                return BABYLON.MeshBuilder.CreateCylinder("Pin", {diameterTop: 0.5, diameterBottom: 0, tessellation: 12}, scene);
            case 19:
                return BABYLON.Mesh.CreateCylinder("Cylinder", 3, 0.25, 0.25, 12, 1, scene);
            default:
                return BABYLON.Mesh.CreateBox("Cube", 1.0*mult, scene);
        }
    }

    /*
    Topologies:
            Cube = 1,
            Sphere = 2,
            Torus = 3,
            Cylinder = 4,
            Pin = 5,
            Rod = 6,
            Point = 7
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
        return [p[0]/100,p[2]/100,p[1]/100];
    }

    function adjustMeshScaling(g, s, isRoot) {
        if(isRoot){
            return [s[0], s[2], s[1]] //Same coordinate plane issue as with position
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

    var meshMap = new Map();
    var materialMap = new Map();
    var allGlyphs = new Map();
    var topoIDs = new Map();
    filterManager = new SYNGLYPHX.FilterManager();

    var avg_x = 0;
    var avg_y = 0;
    var avg_z = 0;

    var unique_locations = new Map();

    //console.log(json_path);

    $.ajax({url: json_path, success: function(data){

        var rootNum = 0;
        var bg = data.background;
        if(bg["image"].length >= 5) {
            var splt = json_path.substring(json_path.lastIndexOf('%2F')+3, json_path.length);
            var img_path = json_path.replace(splt,bg["image"]);
            console.log(img_path);
            createBaseImage(img_path, bg["width"], bg["height"]);
        }
        createAxisLines(bg["axis"][0], bg["axis"][1], bg["axis"][2]);
        if(bg["geo"]) {
            enableAxisLines(0);
        }

        var obj = data.nodes;
        for (var i = 0; i < obj.length; i++){

            if(obj[i]["alpha"] != 0){
                topoIDs.set(obj[i]["id"], obj[i]["topo"]);

                var mesh;
                if(meshMap.get(obj[i]["geometry"]+obj[i]["color"]) == null){
                    meshMap.set(obj[i]["geometry"]+obj[i]["color"], createShape(obj[i]["geometry"], obj[i]["parent_id"] == 0));
                    mesh = meshMap.get(obj[i]["geometry"]+obj[i]["color"]);
                }
                else{
                    mesh = meshMap.get(obj[i]["geometry"]+obj[i]["color"]).createInstance("mesh"+i);
                }

                var topo = obj[i]["parent_id"] != 0 ? topoIDs.get(obj[i]["parent_id"]) : 0; 
                pos = translateMeshPosition(0.7, obj[i]["pos"], obj[i]["parent_id"] == 0, topo)
                mesh.position = new BABYLON.Vector3(pos[0], pos[1], pos[2]);

                scale = adjustMeshScaling(obj[i]["geometry"], obj[i]["scale"], obj[i]["parent_id"] == 0)
                mesh.scaling = new BABYLON.Vector3(scale[0], scale[1], scale[2]);
                if(obj[i]["geometry"] == 7){
                    console.log("id: "+obj[i]["id"]+", "+obj[i]["scale"]);
                }

                rotate = translateMeshRotation(obj[i]["geometry"], obj[i]["scale"], obj[i]["rotate"], obj[i]["parent_id"] == 0, topo);
                mesh.rotation = new BABYLON.Vector3(rotate[0], rotate[1], rotate[2]);

                allGlyphs.set(obj[i]["id"], mesh);
                if(obj[i]["parent_id"] != 0){
                    mesh.parent = allGlyphs.get(obj[i]["parent_id"]);
                }
                else{
                    filterManager.addRootGlyph(++rootNum, mesh);
                    avg_x += pos[0];
                    avg_y += pos[1];
                    avg_z += pos[2];
                    mesh.scaling = new BABYLON.Vector3(scale[0]*2, scale[1]*2, scale[2]*2);
                    if(unique_locations.get(pos[0]+""+pos[1]+""+pos[2]) == null){
                        unique_locations.set(pos[0]+""+pos[1]+""+pos[2], 1); 
                    }
                    else{
                        unique_locations.set(pos[0]+""+pos[1]+""+pos[2], unique_locations.get(pos[0]+""+pos[1]+""+pos[2])+1);
                    }
                }
                mesh.freezeWorldMatrix();

                if(materialMap.get(obj[i]["color"]) == null){
                    materialMap.set(obj[i]["color"], new BABYLON.StandardMaterial("sm", scene));
                }
                materialMap.get(obj[i]["color"]).emissiveColor = new BABYLON.Color3.FromHexString(obj[i]["color"]);
                materialMap.get(obj[i]["color"]).diffuseColor = new BABYLON.Color3.FromHexString(obj[i]["color"]);
                mesh.material = materialMap.get(obj[i]["color"]);
                mesh.material.needDepthPrePass = true;
                tags.set(mesh.id, obj[i]["tag"]);
            }
        }

        camera.setTarget(new BABYLON.Vector3(avg_x/filterManager.rootCount(),avg_y/filterManager.rootCount(),avg_z/filterManager.rootCount()));
        camera.beta = 1.0;
        camera2.alpha = camera.alpha;
        camera2.beta = camera.beta;

        console.log("Glyph Count:" + filterManager.rootCount() + ", Unique Locations:" + unique_locations.size);

    }});

    function findParent(mesh){
        if(mesh.parent == null){
            return mesh;
        }
        return findParent(mesh.parent);
    }

    var lastPickedMesh = null;
    scene.onPointerDown = function (evt, pickResult) {
        if (pickResult.hit) {
            if(pickResult.pickedMesh.name != 'ground' && pickResult.pickedMesh.name != 'tagplane'){
                lastPickedMesh = pickResult.pickedMesh;
                var mesh = findParent(pickResult.pickedMesh);
                camera.setTarget(new BABYLON.Vector3(mesh.position.x, mesh.position.y, mesh.position.z));
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
        else if(evt.keyCode == 73) {
            if(!displayedTags.has(lastPickedMesh.id)){
                displayedTags.set(lastPickedMesh.id, createLabel(lastPickedMesh, tags.get(lastPickedMesh.id)));
            }
        }
        else if(evt.keyCode == 79) {
            if(displayedTags.has(lastPickedMesh.id)){
                advancedTexture.removeControl(displayedTags.get(lastPickedMesh.id));
                displayedTags.delete(lastPickedMesh.id);
            }
        }
        else if(evt.keyCode == 65) {
            enableAxisLines(0);
        }
        else if(evt.keyCode == 83) {
            enableAxisLines(1);
        }
    }  

	return scene;
};
