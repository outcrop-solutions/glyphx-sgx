var scene = null;
var glyph = new SYNGLYPHX.Glyph();

function updateBackgroundColor() {
    console.log(document.getElementById("bgColor").value);
    scene.clearColor = BABYLON.Color3.FromHexString(document.getElementById("bgColor").value);
}

function logProperties(id) {
    glyph.logProperties(id);
}

function getSelectedId() {
    return glyph.getSelectedId();
}

function updateGlyphProperty(property, value, min, max) {
    //console.log(glyph.getSelectedId());
    //console.log(property, value, min, max);
    glyph.getElement(glyph.getSelectedId()).updateProperty(property, value, min, max);
    glyph.selectElement(glyph.getSelectedId());
}

function changeShape(obj, index) {
    var mesh = glyph.getElement(glyph.getSelectedId()).getMesh();
    var material = mesh.material;
    mesh.dispose();
    mesh = createShape(index);
    mesh.material = material;
    glyph.getElement(glyph.getSelectedId()).setMesh(mesh);
}

function getLastMappingChoice() {
    return glyph.getElement(glyph.getSelectedId()).lastMappingChoice;
}

function setLastMappingChoice(index) {
    glyph.getElement(glyph.getSelectedId()).lastMappingChoice = index;
}

function addNewGlyphElement(shape, color) {
    //console.log(glyph.lastAddedId(), glyph.getSelectedId());
    mesh = createShape(shape);
    glyph.addElement(mesh, glyph.getSelectedId());
    changeColor(color);
    glyph.updateColor(glyph.lastAddedId());
    //logProperties(0);
}

function getMaterialColor() {
    mdc=glyph.getElement(glyph.getSelectedId()).getMesh().material.diffuseColor;
    function fromRGBto32(rgbArr) {
      return '#' + rgbArr.reduce(function(s, v) {
        return s + ('0' + v.toString(16)).slice(-2);
      },'')
    }
    return fromRGBto32([mdc.r*255,mdc.g*255,mdc.b*255]);
}

function changeColor(index) {
    if(glyph.getElement(glyph.lastAddedId()).getMesh().material){
        glyph.getElement(glyph.lastAddedId()).getMesh().material.dispose();
    }
    switch(index){
        case 0: return updateColor("#ffffff");break;
        case 1: return updateColor("#000000");break;
        case 2: return updateColor("#ff0000");break;
        case 3: return updateColor("#008000");break;
        case 4: return updateColor("#0000ff");break;
        case 5: return updateColor("#00ffff");break;
        case 6: return updateColor("#ff00ff");break;
        case 7: return updateColor("#ffff00");break;
        case 8: return updateColor("#808080");break;
        case 9: return updateColor("#800080");break;
        case 10: return updateColor("#ffa500");break;
        case 11: return updateColor("#a52a2a");break;
        case 12: return updateColor("#ffc0cb");break;
        case 13: return updateColor("#ffd700");break;
        case 14: return updateColor("#c0c0c0");break;
        default: return updateColor("#ff0000");
    }
}

function createShape(geometry) {
    switch(geometry){
        case 0:
            return BABYLON.Mesh.CreateBox("Cube", 1.0, scene);
        case 1:
            return BABYLON.Mesh.CreateSphere("Sphere", 16, 1, scene);
        case 2:
            return BABYLON.MeshBuilder.CreateCylinder("Cone", {height: 1, diameterTop: 0, tessellation: 12}, scene);
        case 3:
            return BABYLON.Mesh.CreateTorus("Torus", 1, 0.2, 16, scene);
        case 4:
            return BABYLON.MeshBuilder.CreatePolyhedron("Dodecahedron", {type: 2, size: 0.5}, scene);
        case 5:
            return BABYLON.MeshBuilder.CreatePolyhedron("Octahedron", {type: 1, size: 0.5}, scene);
        case 6:
            return BABYLON.MeshBuilder.CreatePolyhedron("Tetrahedron", {type: 0, size: 0.5}, scene);
        case 7:
            return BABYLON.MeshBuilder.CreatePolyhedron("Icosahedron", {type: 3, size: 0.5}, scene);
        case 8:
            return BABYLON.MeshBuilder.CreateCylinder("Pin", {diameterTop: 0.5, diameterBottom: 0, tessellation: 12}, scene);
        case 9:
            return BABYLON.Mesh.CreateCylinder("Cylinder", 3, 0.25, 0.25, 12, 1, scene);
        default:
            return BABYLON.Mesh.CreateBox("Cube", 1.0, scene);
    }
}

function updateColor(hex) {
    glyph.getElement(glyph.lastAddedId()).changeColor(hex);
    return hex;
}

var createScene = function (engine) {
	scene = new BABYLON.Scene(engine);
    scene.clearColor = new BABYLON.Color3(0, 0, 0);
    glyph.createSelectionIndicator(scene);

	var camera = new BABYLON.ArcRotateCamera("Camera", 3 * Math.PI / 2, Math.PI / 3, 15, BABYLON.Vector3.Zero(), scene);
    camera.lowerRadiusLimit = 3;
    //camera.wheelPrecision = 5;
	camera.attachControl(canvas, true);

	//var light = new BABYLON.HemisphericLight("hemi", new BABYLON.Vector3(0, -1, 0), scene);
    var light = new BABYLON.DirectionalLight("Dir0", new BABYLON.Vector3(1, -1, 1), scene);
    //var light = new BABYLON.PointLight("Dir0", new BABYLON.Vector3(0, -1, 0), scene);
    light.specular = new BABYLON.Color3(0, 0, 0);
    // Default intensity is 1. Let's dim the light a small amount
    light.intensity = 0.6;
    light.parent = camera;

    advancedTexture = BABYLON.GUI.AdvancedDynamicTexture.CreateFullscreenUI("ui1");

    var image = new BABYLON.GUI.Image("but", "3d/sgx_inside.png");
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
    function findParent(mesh){
        if(mesh.parent == null){
            return mesh;
        }
        return findParent(mesh.parent);
    }

    mesh = createShape(0);
    glyph.addElement(mesh);

    updateColor("#ff0000");
*/

    scene.onPointerDown = function (evt, pickResult) {
        if(pickResult.hit){
            console.log("Picked:", pickResult.pickedMesh.name, pickResult.pickedMesh.position);
        }
        if (pickResult.hit) {
            glyph.selectElement(pickResult.pickedMesh.name);
        }
    }

    document.onkeyup = function(evt) {
        if(evt.keyCode == 76) { // L
            logProperties(glyph.getSelectedId());
        }
    }  

	return scene;
};
