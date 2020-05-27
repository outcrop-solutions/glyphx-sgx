var SYNGLYPHX;
(function (SYNGLYPHX) {
	/**
     * Node is the basic class for all scene objects.
     */
    var AxisControl = (function () {
        /**
         * @constructor
         * @param {BABYLON.Mesh} the center mesh of the axis lines
         */
        function AxisControl(axis_center) {
            this.center = axis_center;
            this.endPoints = new Array();
        }
        AxisControl.prototype.getClassName = function () {
            return "AxisControl";
        };
        AxisControl.prototype.getCenter = function () {
            return this.center;
        };
        AxisControl.prototype.setEndPoints = function (x, y, z) {
            this.endPoints[0] = x;
            this.endPoints[1] = y;
            this.endPoints[2] = z;
            return this;
        };
        AxisControl.prototype.getX = function () {
            return this.endPoints[0];
        };
        AxisControl.prototype.getY = function () {
            return this.endPoints[1];
        };
        AxisControl.prototype.getZ = function () {
            return this.endPoints[2];
        };
        AxisControl.prototype.setEnabled = function (enabled) {
            this.center.setEnabled(enabled);
            return this;
        };
        AxisControl.prototype.isEnabled = function () {
            return this.center.isEnabled();
        };
        return AxisControl;
    }());
	SYNGLYPHX.AxisControl = AxisControl;

    var FilterManager = (function () {
        /**
         * @constructor
         */
        function FilterManager (camera) {
            this.rootGlyphs = new Map();
            this.transparentGlyphs = new Map();
            this.selectedGlyphs = new Array();
            this.ignoredGlyphs = new Array();
            this.selectionManager = new SYNGLYPHX.SelectionManager(camera);
            this.superImposedGlyphs = new SYNGLYPHX.SuperImposedGlyphs();
            this.currentGlyphs = null;
            this.hideGlyphs = true;
        }
        FilterManager.prototype.getClassName = function () {
            return "FilterManager";
        };
        FilterManager.prototype.getRootGlyph = function (id) {
            return this.rootGlyphs.get(id);
        };
        FilterManager.prototype.setHideGlyphs = function (hide) {
            this.hideGlyphs = hide;
            return this;
        };
        FilterManager.prototype.addRootGlyph = function (id, mesh, tmesh, ignore) {
            this.rootGlyphs.set(id, mesh);
            if(ignore){
                mesh.setEnabled(0);
                this.ignoredGlyphs.push(id);
            }
            this.transparentGlyphs.set(id, tmesh);
            tmesh.setEnabled(0);
            return this;
        };
        FilterManager.prototype.rootCount = function () {
            return this.rootGlyphs.size;
        };
        FilterManager.prototype.filterGlyphs = function (glyphIDs) {
            this.clearSelections();
            var that = this;
            this.rootGlyphs.forEach(function(value, key, map) {
                if(!that.ignoredGlyphs.includes(key)){
                    value.setEnabled(glyphIDs.includes(key) ? 1 : 0);
                    if(!that.hideGlyphs){
                        that.transparentGlyphs.get(key).setEnabled(glyphIDs.includes(key) ? 0 : 1);
                    }
                }
            });
            this.currentGlyphs = glyphIDs;
            return this;
        };
        FilterManager.prototype.clearFilters = function () {
            var that = this;
            this.rootGlyphs.forEach(function(value, key, map) {
                if(!that.ignoredGlyphs.includes(key)){
                    value.setEnabled(1);
                    that.transparentGlyphs.get(key).setEnabled(0);
                }
            });
            this.currentGlyphs = null;
            return this;
        };
        FilterManager.prototype.getSelectedIDs = function () {
            return this.selectedGlyphs;
        };
        FilterManager.prototype.getIdFromMesh = function (mesh) {
            var id;
            this.rootGlyphs.forEach(function(value, key, map) {
                if(value.id == mesh.id){
                    id = key;
                    return;
                }
            });
            return id;
        };
        FilterManager.prototype.addSingleSelection = function (mesh, parent) {
            //var selectedGlyphs = new Array();
            var that = this;
            this.rootGlyphs.forEach(function(value, key, map) {
                if(value.id == parent.id){
                    that.selectedGlyphs.push(key);
                    //console.log(key);
                    that.selectionManager.createIndicator(mesh);
                }
            });
            if(this.selectedGlyphs.length > 0){
                this.selectionManager.runLoop();
            }
            //this.selectedGlyphs = selectedGlyphs;
            return this;
        };
        FilterManager.prototype.setSelectedGlyphs = function (camera, start, end) {
            var topLeft = [Math.min(start[0], end[0]), Math.min(start[1], end[1])];
            var bottomRight = [Math.max(start[0], end[0]), Math.max(start[1], end[1])];
            //var selectedGlyphs = new Array();
            var that = this;
            this.rootGlyphs.forEach(function(value, key, map) {
                if(value.isEnabled()){
                    var transformationMatrix = camera.getViewMatrix().multiply(camera.getProjectionMatrix());
                    var position = value.getBoundingInfo().boundingSphere.center;
                    var projectedPosition = BABYLON.Vector3.Project(position, value.computeWorldMatrix(true), transformationMatrix, camera.viewport.toGlobal(camera.getEngine()));
                    if(projectedPosition.x >= topLeft[0] && projectedPosition.x <= bottomRight[0]
                        && projectedPosition.y >= topLeft[1] && projectedPosition.y <= bottomRight[1]){
                        that.selectedGlyphs.push(key);
                        that.selectionManager.createIndicator(value);
                    }
                }
            });
            if(this.selectedGlyphs.length > 0){
                this.selectionManager.runLoop();
            }
            //this.selectedGlyphs = selectedGlyphs;
            return this;
        };
        FilterManager.prototype.getSelectionCenter = function () {
            var x = 0, y = 0, z = 0;
            for(var i = 0; i < this.selectedGlyphs.length; i++){
                var pos = this.rootGlyphs.get(this.selectedGlyphs[i]).position;
                x += pos.x;
                y += pos.y;
                z += pos.z;
            }
            return new BABYLON.Vector3(x/this.selectedGlyphs.length, y/this.selectedGlyphs.length, z/this.selectedGlyphs.length);
        };
        FilterManager.prototype.clearSelections = function () {
            this.selectedGlyphs = new Array();
            this.selectionManager.destroyIndicators();
            this.selectionManager.endLoop();
            //this.selectionManager.si_sphere.setEnabled(0);
            return this;
        };
        FilterManager.prototype.fadeGlyphs = function (mesh, currentSiGroup) {
            var that = this;
            if(currentSiGroup != null){
                this.superImposedGlyphs.setInitialPosition(mesh.position);
                this.superImposedGlyphs.setCurrentGroup(currentSiGroup, 6);
            }else{
                this.superImposedGlyphs.clearGroup();
            }
            this.rootGlyphs.forEach(function(value, key, map) {
                if(currentSiGroup != null){
                    if(currentSiGroup.includes(key)){
                        value.setEnabled(1);
                        that.transparentGlyphs.get(key).setEnabled(0);
                        that.superImposedGlyphs.addMesh(key, value);
                    }else{
                        value.setEnabled(0);
                        if(that.currentGlyphs != null){
                            that.transparentGlyphs.get(key).setEnabled(that.currentGlyphs.includes(key) ? 1 : 0);
                        }else{
                            that.transparentGlyphs.get(key).setEnabled(1);
                        }
                        if(that.ignoredGlyphs.includes(key)){
                            that.transparentGlyphs.get(key).setEnabled(0);
                        }
                    }
                }else{
                    if(!that.ignoredGlyphs.includes(key)){
                        if(that.currentGlyphs != null){
                            value.setEnabled(that.currentGlyphs.includes(key) ? 1 : 0);
                        }else{
                            value.setEnabled(1);
                        }
                    }
                    else{
                        value.setEnabled(0);
                    }
                    that.transparentGlyphs.get(key).setEnabled(0);
                }
            });
            return this;
        };
        FilterManager.prototype.updateXYZCoordinates = function (data) {
            this.clearSelections();
            var unique_locations = new Map();
            for(var i = 0; i < data.length; i++){
                var root = this.rootGlyphs.get(data[i]["rowID"]);
                var troot = this.transparentGlyphs.get(data[i]["rowID"]);
                this.unfreezeMeshMatrix(root);
                this.unfreezeMeshMatrix(troot);
                root.position = new BABYLON.Vector3(data[i]["X"], data[i]["Z"], data[i]["Y"]);
                troot.position = new BABYLON.Vector3(data[i]["X"], data[i]["Z"], data[i]["Y"]);
                this.freezeMeshMatrix(root);
                this.freezeMeshMatrix(troot);
                if(unique_locations.get(data[i]["X"]+""+data[i]["Z"]+""+data[i]["Y"]) == null){
                    unique_locations.set(data[i]["X"]+""+data[i]["Z"]+""+data[i]["Y"], new Array()); 
                }
                unique_locations.get(data[i]["X"]+""+data[i]["Z"]+""+data[i]["Y"]).push(data[i]["rowID"]);
            }
            return unique_locations;
        };
        FilterManager.prototype.freezeMeshMatrix = function (mesh) {
            mesh.freezeWorldMatrix();
            var children = mesh.getChildren();
            for(var i = 0; i < children.length; i++){
                this.freezeMeshMatrix(children[i]);
            }
            return this;
        };
        FilterManager.prototype.unfreezeMeshMatrix = function (mesh) {
            mesh.unfreezeWorldMatrix();
            var children = mesh.getChildren();
            for(var i = 0; i < children.length; i++){
                this.unfreezeMeshMatrix(children[i]);
            }
            return this;
        };
        return FilterManager;
    }());
    SYNGLYPHX.FilterManager = FilterManager;

    var SelectionManager= (function () {
        /**
         * @constructor
         * @param {BABYLON.Mesh} the center mesh of the axis lines
         */
        function SelectionManager(camera) {
            this.activeSelections = new Array();
            //SELECTION INDICATOR
            this.yellow = new BABYLON.StandardMaterial("sim", scene);
            this.yellow.emissiveColor = new BABYLON.Color3.Yellow();
            this.yellow.diffuseColor = new BABYLON.Color3.Yellow();
            this.yellow.alpha = 0.5;
            this.timer = null;
            //END
            //SUPERIMPOSED GLYPH INDICATOR
            this.white = new BABYLON.StandardMaterial("sgm", scene);
            this.white.emissiveColor = new BABYLON.Color3.White();
            this.white.diffuseColor = new BABYLON.Color3.White();
            this.white.alpha = 0.1;
            this.si_sphere = BABYLON.Mesh.CreateSphere("Sphere", 16, 1, scene);
            this.si_sphere.material = this.white;
            this.si_sphere.setEnabled(0);
            //END
            this.camera = camera;
        }
        SelectionManager.prototype.getClassName = function () {
            return "SelectionManager";
        };
        SelectionManager.prototype.createIndicator = function (mesh) {
            this.activeSelections.push(new SYNGLYPHX.SelectionIndicator(mesh, this.yellow));
            return this;
        };
        SelectionManager.prototype.destroyIndicators = function () {
            //console.log(this.activeSelections.length);
            for(var i = 0; i < this.activeSelections.length; i++){
                this.activeSelections[i].getClone().dispose();   
            }
            this.activeSelections = new Array();
            return this;
        };
        SelectionManager.prototype.superImposedIndicator = function (mesh) {
            //console.log(mesh.position);
            this.si_sphere.position = mesh.position;
            var scale = Math.max(mesh.scaling.x, Math.max(mesh.scaling.y, mesh.scaling.z))*2;
            this.si_sphere.scaling = new BABYLON.Vector3(scale, scale, scale);
            return this.si_sphere;
        };
        SelectionManager.prototype.runLoop = function () {
            var that = this;
            this.timer = setInterval(function(){ 
                var increment = -0.025;
                var scaling = that.camera.radius/500;
                if(that.yellow.alpha <= 0.005){
                    that.yellow.alpha = 0.5;
                    for(var  i = 0; i < that.activeSelections.length; i++){
                        that.activeSelections[i].resetScale();
                    }
                }
                that.yellow.alpha += increment;
                for(var  i = 0; i < that.activeSelections.length; i++){
                    that.activeSelections[i].incrementScale(scaling);
                }
            }, 200);
            return this;
        };
        SelectionManager.prototype.endLoop = function () {
            clearInterval(this.timer);
            this.yellow.alpha = 0.5;
            return this;
        };
        return SelectionManager;
    }());
    SYNGLYPHX.SelectionManager = SelectionManager;

    var SelectionIndicator = (function () {
        /**
         * @constructor
         * @param {BABYLON.Mesh} the center mesh of the axis lines
         */
        function SelectionIndicator(mesh, material) {
            if(typeof mesh.sourceMesh === "undefined"){
                this.clone = mesh.clone("clone", false, true);
            }else{
                this.clone = mesh.sourceMesh.clone("clone", false, true);
            }
            this.clone.material = material;
            this.clone.position = mesh.getAbsolutePosition();

            function absoluteScale(mesh){
                if(mesh.parent == null){
                    return mesh.scaling;
                }
                return absoluteScale(mesh.parent).multiply(mesh.scaling);
            }
            var rotMatrix = mesh.getWorldMatrix().getRotationMatrix();
            var rotVector = new BABYLON.Vector3.Zero();
            BABYLON.Quaternion.FromRotationMatrix(rotMatrix).toEulerAnglesToRef(rotVector);
            //console.log("Rotation: " + rotVector);
            this.clone.rotation = rotVector;
      
            this.scale = absoluteScale(mesh);
            this.clone.scaling = absoluteScale(mesh);
            console.log("Scaling: " + this.clone.scaling);

            var factor = [this.scale.x,this.scale.y,this.scale.z];
            factor.sort();
            this.xscale = this.scale.x / factor[2];
            this.yscale = this.scale.y / factor[2];
            this.zscale = this.scale.z / factor[2];
        }
        SelectionIndicator.prototype.getClassName = function () {
            return "SelectionIndicator";
        };
        SelectionIndicator.prototype.getClone = function () {
            return this.clone;
        };
        SelectionIndicator.prototype.resetScale = function () {
            this.clone.scaling = this.scale;//new BABYLON.Vector3(this.scale[0]*0.95, this.scale[1]*0.95, this.scale[2]*0.95);
            return this;
        };
        SelectionIndicator.prototype.incrementScale = function (increment) {
            var scale = [this.clone.scaling.x, this.clone.scaling.y, this.clone.scaling.z];
            this.clone.scaling = new BABYLON.Vector3(scale[0]+(increment*this.xscale), scale[1]+(increment*this.yscale), scale[2]+(increment*this.zscale));
            return this;
        };
        return SelectionIndicator;
    }());
    SYNGLYPHX.SelectionIndicator = SelectionIndicator;

    var SuperImposedGlyphs = (function () {
 
        function SuperImposedGlyphs() {
            this.position = null;
            this.currentGroup = null;
            this.newPositions = null;
            this.meshes = null;
        }
        SuperImposedGlyphs.prototype.getClassName = function () {
            return "SuperImposedGlyphs";
        };
        SuperImposedGlyphs.prototype.setInitialPosition = function (pos) {
            this.position = pos;
            //console.log(pos);
            return this;
        };
        SuperImposedGlyphs.prototype.setCurrentGroup = function (group, box) {
            this.currentGroup = group;
            var row_size = Math.ceil(Math.sqrt(group.length));
            var x_size = box*1.5;
            var y_size = box*1.5;
            //console.log(x_size+", "+y_size);
            var start_x = this.position.x;
            var start_y = this.position.z-y_size;
            this.meshes = new Array();
            this.newPositions = new Map();
            var i = 0;
            /*for(var i = 0; i < group.length; i++){
                this.newPositions.set(group[i], new BABYLON.Vector3(this.position.x, this.position.y, this.position.z-((i+1)*2)));
            }*/
            for(var x = 0; x < row_size; x++){
                for(var y = 0; y < row_size; y++){
                    if(i++ >= group.length){break;}
                    this.newPositions.set(group[(x*row_size)+y], new BABYLON.Vector3(start_x+(x_size*x), this.position.y, start_y-(y_size*y)));
                }
            }
            return this;
        };
        SuperImposedGlyphs.prototype.addMesh = function (id, mesh) {
            //console.log("id: "+id+", "+mesh.position+", "+mesh.visibility);
            this.unfreezeMeshMatrix(mesh);

            mesh.position = this.newPositions.get(id);
            //console.log("id: "+id+", "+mesh.position+", "+mesh.visibility);
            this.meshes.push(mesh);
            //console.log(mesh.position);
            return this;
        };
        SuperImposedGlyphs.prototype.clearGroup = function () {
            for(var i = 0; i < this.meshes.length; i++){
                this.meshes[i].position = this.position;
                this.freezeMeshMatrix(this.meshes[i]);
            }
            this.meshes = null;
            return this;
        };
        SuperImposedGlyphs.prototype.freezeMeshMatrix = function (mesh) {
            mesh.freezeWorldMatrix();
            var children = mesh.getChildren();
            for(var i = 0; i < children.length; i++){
                this.freezeMeshMatrix(children[i]);
            }
            return this;
        };
        SuperImposedGlyphs.prototype.unfreezeMeshMatrix = function (mesh) {
            mesh.unfreezeWorldMatrix();
            var children = mesh.getChildren();
            for(var i = 0; i < children.length; i++){
                this.unfreezeMeshMatrix(children[i]);
            }
            return this;
        };
        return SuperImposedGlyphs;
    }());
    SYNGLYPHX.SuperImposedGlyphs = SuperImposedGlyphs;

})(SYNGLYPHX || (SYNGLYPHX = {}));