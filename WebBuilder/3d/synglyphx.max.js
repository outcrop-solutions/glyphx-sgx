var SYNGLYPHX;
(function (SYNGLYPHX) {
	
    var Glyph = (function () {
        /**
         * @constructor
         */
        function Glyph() {
            this.elements = new Array();
            this.childScaling = 0.5;
            this.childSpacing = 1.4;
            this.selectedId = 0;
        }
        Glyph.prototype.getClassName = function () {
            return "Glyph";
        };
        Glyph.prototype.addElement = function (mesh, parent) {
            if(this.hasRoot()){
                mesh.parent = this.elements[parent].getMesh();
                this.elements[parent].addChild(this.elements.length);
                mesh.scaling.scaleInPlace(mesh.parent.scaling.x*this.childScaling);

                var parent_radius = Math.max(this.elements[parent].getMesh().getHalfSize().x, this.elements[parent].getMesh().getHalfSize().z) * this.childSpacing;
                var child_radius = Math.max(mesh.getHalfSize().x, mesh.getHalfSize().z) * this.childScaling;
                var child_dist = parent_radius + child_radius;

                this.elements.push(new SYNGLYPHX.Element(mesh));
                this.elements[this.lastAddedId()].bindings.get('SclX').setMax(mesh.scaling.x);
                this.elements[this.lastAddedId()].bindings.get('SclY').setMax(mesh.scaling.y);
                this.elements[this.lastAddedId()].bindings.get('SclZ').setMax(mesh.scaling.z);
                if(mesh.scaling.x == mesh.scaling.y && mesh.scaling.x == mesh.scaling.z){
                    this.elements[this.lastAddedId()].bindings.get('Scl').setMax(mesh.scaling.x);
                }

                if(this.elements[parent].children.length == 1){
                    mesh.position.z = child_dist * Math.cos(Math.radians(0));
                    this.elements[this.lastAddedId()].bindings.get('PosX').update(null, 0.0, 0.0);
                    this.elements[this.lastAddedId()].bindings.get('PosY').update(null, 0.0, 0.0);
                }
                else{
                    this.elements[this.elements[parent].children[0]].getMesh().position.z = child_dist * Math.cos(Math.radians(0));
                    this.elements[this.elements[parent].children[0]].bindings.get('PosY').update(null, 0.0, Math.round(child_dist * Math.cos(Math.radians(0)) * 100) / 100);
                    var increment = 360.0/this.elements[parent].children.length;
                    for(var i = 1; i < this.elements[parent].children.length; i++){
                        //console.log("Child position:",this.elements[parent].children[i]);
                        var x = child_dist * Math.sin(Math.radians(i*increment));
                        var z = child_dist * Math.cos(Math.radians(i*increment));
                        this.elements[this.elements[parent].children[i]].getMesh().position.x = x;
                        this.elements[this.elements[parent].children[i]].getMesh().position.z = z;

                        //this.elements[this.elements[parent].children[i]].bindings.get('PosX').update(null, 0.0, i*increment);
                        //this.elements[this.elements[parent].children[i]].bindings.get('PosY').update(null, 0.0, 0.0);

                        this.elements[this.elements[parent].children[i]].bindings.get('PosX').update(null, 0.0, Math.round(x * 100) / 100);
                        this.elements[this.elements[parent].children[i]].bindings.get('PosY').update(null, 0.0, Math.round(z * 100) / 100);

                        if(this.elements[this.elements[parent].children[i]].isSelected()){
                            this.selectElement(this.elements[parent].children[i]);
                        }
                    }
                }
                this.elements[this.elements.length-1].getMesh().name = this.elements.length-1;
            }
            else{
                this.elements.push(new SYNGLYPHX.Element(mesh));
                this.elements[this.elements.length-1].getMesh().name = 0;
                this.selectElement(0);
            }
            return this.elements[this.elements.length-1];
        };
        Glyph.prototype.updateColor = function (id) {
            mdc=this.elements[id].getMesh().material.diffuseColor;
            function fromRGBto32(rgbArr) {
              return '#' + rgbArr.reduce(function(s, v) {
                return s + ('0' + v.toString(16)).slice(-2);
              },'')
            }
            var max = fromRGBto32([mdc.r*255,mdc.g*255,mdc.b*255]);
            var min = rybComp(max);
            this.elements[id].bindings.get('Clr').setMin(min);
            this.elements[id].bindings.get('Clr').setMax(max);
            return this;
        };
        Glyph.prototype.getElement = function (id) {
            return this.elements[id];
        };
        Glyph.prototype.hasRoot = function () {
            return this.elements.length > 0;
        };
        Glyph.prototype.getSelectedId = function () {
            return this.selectedId;
        };
        Glyph.prototype.lastAddedId = function () {
            return this.elements.length-1;
        };
        Glyph.prototype.createSelectionIndicator = function (scene) {
            this.selectionIndicator = new SYNGLYPHX.SelectionIndicator(scene);
            return this;
        };
        Glyph.prototype.selectElement = function (id) {
            this.selectionIndicator.moveToElement(this.getElement(id).getMesh());
            if(this.selectedId){
                this.getElement(this.selectedId).setSelected(false);
            }
            this.getElement(id).setSelected(true);
            this.selectedId = id;
            this.getElement(id).updatePropertiesPane();
            return this;
        };
        Glyph.prototype.logProperties = function (id) {
            console.log("Properties for element", id);
            this.elements[id].logProperties();
            return this;
        };
        return Glyph;
    }());
	SYNGLYPHX.Glyph = Glyph;

    var Element = (function () {
        /**
         * @constructor
         */
        function Element(mesh) {
            this.mesh = mesh;
            this.bindings = new Map();
            var keys = ['PosX','PosY','PosZ','RotX','RotY','RotZ','Scl',
                'SclX','SclY','SclZ','Clr','Alph','Geo','Srf','Rto','Tag','Url'];

            this.bindings.set('PosX', new SYNGLYPHX.Binding(-180.0, 180.0));
            this.bindings.set('PosY', new SYNGLYPHX.Binding(-90.0, 90.0));
            this.bindings.set('PosZ', new SYNGLYPHX.Binding(0.0, 0.0));

            this.bindings.set('RotX', new SYNGLYPHX.Binding(0.0, 0.0));
            this.bindings.set('RotY', new SYNGLYPHX.Binding(0.0, 0.0));
            this.bindings.set('RotZ', new SYNGLYPHX.Binding(0.0, 0.0));

            this.bindings.set('Scl', new SYNGLYPHX.Binding(0.1, 1.0));
            this.bindings.set('SclX', new SYNGLYPHX.Binding(0.1, 1.0));
            this.bindings.set('SclY', new SYNGLYPHX.Binding(0.1, 1.0));
            this.bindings.set('SclZ', new SYNGLYPHX.Binding(0.1, 1.0));

            this.bindings.set('Clr', new SYNGLYPHX.Binding(null, null));
            this.bindings.set('Alph', new SYNGLYPHX.Binding(0.0, 255.0));

            this.bindings.set('Geo', new SYNGLYPHX.Binding(null, null));
            this.bindings.set('Srf', new SYNGLYPHX.Binding(null, 'Solid'));
            this.bindings.set('Rto', new SYNGLYPHX.Binding(0.0, 0.1));

            this.bindings.set('Tag', new SYNGLYPHX.Binding(null, null));
            this.bindings.set('Url', new SYNGLYPHX.Binding(null, null));

            this.lastMappingChoice = null;
            this.children = new Array();
            this.selectedId = false;
        }
        Element.prototype.getClassName = function () {
            return "Element";
        };
        Element.prototype.setMesh = function (mesh) {
            this.mesh = mesh;
            return this;
        };
        Element.prototype.getMesh = function () {
            return this.mesh;
        };
        Element.prototype.addChild = function (id) {
            this.children.push(id);
            return this;
        };
        Element.prototype.isSelected = function () {
            return this.selectedId;
        };
        Element.prototype.setSelected = function (selected) {
            this.selectedId = selected;
            return this;
        };
        Element.prototype.updateProperty = function (property, value, min, max) {
            console.log("updateProperty", property);
            if(property == 'Clr'){
                this.changeColor(max);
            }
            else if(property == 'Geo' && this.bindings.get('Geo').getMax()){
                this.changeShape(max);
            }
            else if(property == 'Scl'){
                this.changeScale(max);
            }
            else if(property == 'Alph'){
                this.changeAlpha(max);
            }
            else if(property == 'Srf'){
                this.changeSurfaceType(max);
            }
            this.bindings.get(property).update(value, min, max);
            return this;
        };
        Element.prototype.updateBindOnly = function (property, value) {
            this.bindings.get(property).setValue(value);
            return this;
        };
        Element.prototype.changeScale = function (max) {
            /*console.log("Current Max:", this.bindings.get('Scl').getMax(), "New Max:", max);
            console.log("Current X:", this.mesh.scaling.x, "New X:", max);
            console.log("Current Y:", this.mesh.scaling.y, "New Y:", max);
            console.log("Current Z:", this.mesh.scaling.z, "New Z:", max);*/
            for(var i = 0; i < this.children.length; i++){
                var child = this.children[i];
                var currentRatio = glyph.elements[child].bindings.get('Scl').getMax()/this.mesh.scaling.x;
                var updatedValue = max * currentRatio;
                //console.log("ID:",child,"Child max:", glyph.elements[child].bindings.get('Scl').getMax(),"New max:",updatedValue);
                glyph.elements[child].mesh.scaling.x = updatedValue;
                glyph.elements[child].mesh.scaling.y = updatedValue;
                glyph.elements[child].mesh.scaling.z = updatedValue;
                glyph.elements[child].bindings.get('Scl').setMax(updatedValue);
                glyph.elements[child].bindings.get('SclX').setMax(updatedValue);
                glyph.elements[child].bindings.get('SclY').setMax(updatedValue);
                glyph.elements[child].bindings.get('SclZ').setMax(updatedValue);
            }
            if(max > 0 && max < 5){
                this.mesh.scaling.x = max;
                this.mesh.scaling.y = max;
                this.mesh.scaling.z = max;
            }
            return this;
        };
        Element.prototype.changeColor = function (hex) {
            if(this.mesh.material){
                this.mesh.material.dispose();
            }
            function shadeColor(color, percent) {   
                var f=parseInt(color.slice(1),16),t=percent<0?0:255,p=percent<0?percent*-1:percent,R=f>>16,G=f>>8&0x00FF,B=f&0x0000FF;
                return "#"+(0x1000000+(Math.round((t-R)*p)+R)*0x10000+(Math.round((t-G)*p)+G)*0x100+(Math.round((t-B)*p)+B)).toString(16).slice(1);
            }
            var material = new BABYLON.StandardMaterial("sm", scene);
            material.emissiveColor = new BABYLON.Color3.FromHexString(shadeColor(hex, -0.5));
            material.diffuseColor = new BABYLON.Color3.FromHexString(hex);
            this.mesh.material = material;
            return this;
        };
        Element.prototype.changeAlpha = function (alpha) {
            this.mesh.visibility = alpha/255;
            return this;
        };
        Element.prototype.changeShape = function (shape) {
            console.log("Element - changeShape called");
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
            //console.log("Child count before:", this.mesh.getChildMeshes());
            var geometries = ['Cube','Sphere','Cone','Torus','Dodecahedron','Octahedron','Tetrahedron','Icosahedron','Pin','Cylinder'];
            var mx = createShape(geometries.indexOf(shape));
            mx.position = this.mesh.position;
            mx.scaling = this.mesh.scaling;
            mx.material = this.mesh.material;
            BABYLON.VertexData.ExtractFromMesh(mx).applyToMesh(this.mesh);
            mx.dispose();
            console.log("Child count after:", this.mesh.getChildMeshes().length);
            for(var i = 0; i < this.mesh.getChildMeshes().length; i++){
                console.log("Child position:",this.mesh.getChildMeshes()[i].position);
            }
            return this;
        };
        Element.prototype.changeSurfaceType = function (srf) {
            if(srf == 'Wireframe'){
                this.mesh.material.wireframe = true;
            }
            else{
                this.mesh.material.wireframe = false;
            }
            return this;
        };
        Element.prototype.updatePropertiesPane = function () {

            //console.log("Real pos:",this.mesh.position);
            //console.log("Real scale:",this.mesh.scaling);

            document.getElementById('PosXmin').value = this.bindings.get('PosX').getMin();
            document.getElementById('PosXmax').value = this.bindings.get('PosX').getMax();
            document.getElementById('PosX').value = this.bindings.get('PosX').getValue();
            document.getElementById('PosYmin').value = this.bindings.get('PosY').getMin();
            document.getElementById('PosYmax').value = this.bindings.get('PosY').getMax();
            document.getElementById('PosY').value = this.bindings.get('PosY').getValue();
            document.getElementById('PosZmin').value = this.bindings.get('PosZ').getMin();
            document.getElementById('PosZmax').value = this.bindings.get('PosZ').getMax();
            document.getElementById('PosZ').value = this.bindings.get('PosZ').getValue();

            document.getElementById('RotXmin').value = this.bindings.get('RotX').getMin();
            document.getElementById('RotXmax').value = this.bindings.get('RotX').getMax();
            document.getElementById('RotX').value = this.bindings.get('RotX').getValue();
            document.getElementById('RotYmin').value = this.bindings.get('RotY').getMin();
            document.getElementById('RotYmax').value = this.bindings.get('RotY').getMax();
            document.getElementById('RotY').value = this.bindings.get('RotY').getValue();
            document.getElementById('RotZmin').value = this.bindings.get('RotZ').getMin();
            document.getElementById('RotZmax').value = this.bindings.get('RotZ').getMax();
            document.getElementById('RotZ').value = this.bindings.get('RotZ').getValue();

            document.getElementById('Sclmin').value = this.bindings.get('Scl').getMin();
            document.getElementById('Sclmax').value = this.bindings.get('Scl').getMax();
            document.getElementById('Scl').value = this.bindings.get('Scl').getValue();
            document.getElementById('SclXmin').value = this.bindings.get('SclX').getMin();
            document.getElementById('SclXmax').value = this.bindings.get('SclX').getMax();
            document.getElementById('SclX').value = this.bindings.get('SclX').getValue();
            document.getElementById('SclYmin').value = this.bindings.get('SclY').getMin();
            document.getElementById('SclYmax').value = this.bindings.get('SclY').getMax();
            document.getElementById('SclY').value = this.bindings.get('SclY').getValue();
            document.getElementById('SclZmin').value = this.bindings.get('SclZ').getMin();
            document.getElementById('SclZmax').value = this.bindings.get('SclZ').getMax();
            document.getElementById('SclZ').value = this.bindings.get('SclZ').getValue();

            document.getElementById('Clrmin').value = this.bindings.get('Clr').getMin();
            document.getElementById('Clrmax').value = this.bindings.get('Clr').getMax();
            angular.element(document.getElementById('Clrmax')).scope().Clrmax = this.bindings.get('Clr').getMax();
            document.getElementById('Clr').value = this.bindings.get('Clr').getValue();
            document.getElementById('Alphmin').value = this.bindings.get('Alph').getMin();
            document.getElementById('Alphmax').value = this.bindings.get('Alph').getMax();
            document.getElementById('Alph').value = this.bindings.get('Alph').getValue();

            document.getElementById('Geomax').value = this.bindings.get('Geo').getMax();
            angular.element(document.getElementById('Geomax')).scope().Geomax = this.bindings.get('Geo').getMax();
            document.getElementById('Geo').value = this.bindings.get('Geo').getValue();
            document.getElementById('Srfmax').value = this.bindings.get('Srf').getMax();
            document.getElementById('Rtomax').value = this.bindings.get('Rto').getMax();

            document.getElementById('Tag').value = this.bindings.get('Tag').getValue();
            document.getElementById('Url').value = this.bindings.get('Url').getValue();

            return this;
        };
        Element.prototype.logProperties = function () {
            this.bindings.forEach(function(value, key, map) {
                console.log(key, value.getValue(), value.getMin(), value.getMax());
            });
            return this;
        };
        return Element;
    }());
    SYNGLYPHX.Element = Element;

    var Binding = (function () {
        /**
         * @constructor
         */
        function Binding(min, max) {
            this.update(null, min, max);
        }
        Binding.prototype.getClassName = function () {
            return "Binding";
        };
        Binding.prototype.update = function (value, min, max) {
            this.setValue(value);
            this.setMin(min);
            this.setMax(max);
            return this;
        };
        Binding.prototype.setValue = function (value) {
            this.value = value;
            return this;
        };
        Binding.prototype.getValue = function () {
            return this.value;
        };
        Binding.prototype.setMin = function (min) {
            this.min = min;
            return this;
        };
        Binding.prototype.getMin = function () {
            return this.min;
        };
        Binding.prototype.setMax = function (max) {
            this.max = max;
            return this;
        };
        Binding.prototype.getMax = function () {
            return this.max;
        };
        return Binding;
    }());
    SYNGLYPHX.Binding = Binding;

    var SelectionIndicator = (function () {
        /**
         * @constructor
         */
        function SelectionIndicator(scene) {
            var material = new BABYLON.StandardMaterial("sm", scene);
                material.emissiveColor = new BABYLON.Color3.FromHexString("#ffff00");
                material.diffuseColor = new BABYLON.Color3.FromHexString("#ffff00");
                material.wireframe = true;
                material.alpha = 0.025;
            var pin = BABYLON.MeshBuilder.CreateCylinder("Pin", {height: 1, diameterTop: 0.35, diameterBottom: 0, tessellation: 32}, scene);
                pin.material = material;
            var sphere = BABYLON.Mesh.CreateSphere("Sphere", 12, 0.35, scene);
                sphere.position.y = 1;
                sphere.setEnabled(false);
            var cube = BABYLON.Mesh.CreateBox("Cube", 1.0, scene);
                cube.position.y += 0.499;
                cube.setEnabled(false);
            var sphereCSG = BABYLON.CSG.FromMesh(sphere);
                sphereCSG.subtractInPlace(BABYLON.CSG.FromMesh(cube));
            var ball = sphereCSG.toMesh("top ball", material, scene, false);
                ball.parent = pin;
                ball.position.y = 0.5;
            this.indicatorMesh = BABYLON.Mesh.MergeMeshes([pin,ball]);
            this.indicatorMesh.isPickable = false;
            this.indicatorMesh.setEnabled(false);
            this.scaledToChild = false;
        }
        SelectionIndicator.prototype.getClassName = function () {
            return "SelectionIndicator";
        };
        SelectionIndicator.prototype.moveToElement = function (mesh) {
            var scaler = mesh.scaling.x;
            this.indicatorMesh.scaling.x = scaler;
            this.indicatorMesh.scaling.y = scaler;
            this.indicatorMesh.scaling.z = scaler;
            var pos = mesh.getAbsolutePosition();
            this.indicatorMesh.position.x = pos.x
            this.indicatorMesh.position.y = pos.y + mesh.getHalfSize().y + (0.35*scaler);
            this.indicatorMesh.position.z = pos.z
            this.indicatorMesh.setEnabled(true);
            return this;
        };
        return SelectionIndicator;
    }());
    SYNGLYPHX.SelectionIndicator = SelectionIndicator;

})(SYNGLYPHX || (SYNGLYPHX = {}));

BABYLON.AbstractMesh.prototype.getHalfSize = function() {
    var boundingBox = this.getBoundingInfo().boundingBox;

    var minWorldBounds = boundingBox.minimumWorld.clone();
    var maxWorldBounds = boundingBox.maximumWorld.clone();

    minWorldBounds = minWorldBounds.subtract(this.position);
    maxWorldBounds = maxWorldBounds.subtract(this.position);

    var halfSize = BABYLON.Vector3.Zero();
    halfSize.x = (Math.abs(minWorldBounds.x) + Math.abs(maxWorldBounds.x)) / 2;
    halfSize.y = (Math.abs(minWorldBounds.y) + Math.abs(maxWorldBounds.y)) / 2;
    halfSize.z = (Math.abs(minWorldBounds.z) + Math.abs(maxWorldBounds.z)) / 2;
    return halfSize;
}