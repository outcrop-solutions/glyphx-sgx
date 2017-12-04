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
            this.selectedGlyphs = new Array();
            this.ignoredGlyphs = new Array();
            this.selectionManager = new SYNGLYPHX.SelectionManager(camera);
        }
        FilterManager.prototype.getClassName = function () {
            return "FilterManager";
        };
        FilterManager.prototype.getRootGlyph = function (id) {
            return this.rootGlyphs.get(id);
        };
        FilterManager.prototype.addRootGlyph = function (id, mesh, ignore) {
            this.rootGlyphs.set(id, mesh);
            if(ignore){
                mesh.setEnabled(0);
                this.ignoredGlyphs.push(id);
            }
            return this;
        };
        FilterManager.prototype.rootCount = function () {
            return this.rootGlyphs.size;
        };
        FilterManager.prototype.filterGlyphs = function (glyphIDs) {
            this.rootGlyphs.forEach(function(value, key, map) {
                value.setEnabled(glyphIDs.includes(key) ? 1 : 0);
            });
            return this;
        };
        FilterManager.prototype.clearFilters = function () {
            console.log(this.ignoredGlyphs.length);
            this.rootGlyphs.forEach(function(value, key, map) {
                value.setEnabled(1);
            });
            return this;
        };
        FilterManager.prototype.getSelectedIDs = function () {
            return this.selectedGlyphs;
        };
        FilterManager.prototype.addSingleSelection = function (mesh) {
            //var selectedGlyphs = new Array();
            var that = this;
            this.rootGlyphs.forEach(function(value, key, map) {
                if(value.id == mesh.id){
                    that.selectedGlyphs.push(key);
                    that.selectionManager.createIndicator(value);
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
                var transformationMatrix = camera.getViewMatrix().multiply(camera.getProjectionMatrix());
                var position = value.getBoundingInfo().boundingSphere.center;
                var projectedPosition = BABYLON.Vector3.Project(position, value.computeWorldMatrix(true), transformationMatrix, camera.viewport.toGlobal(camera.getEngine()));
                if(projectedPosition.x >= topLeft[0] && projectedPosition.x <= bottomRight[0]
                    && projectedPosition.y >= topLeft[1] && projectedPosition.y <= bottomRight[1]){
                    that.selectedGlyphs.push(key);
                    that.selectionManager.createIndicator(value);
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
            this.yellow = new BABYLON.StandardMaterial("yellow", scene);
            this.yellow.emissiveColor = new BABYLON.Color3.Yellow();
            this.yellow.diffuseColor = new BABYLON.Color3.Yellow();
            this.yellow.alpha = 0.5;
            this.camera = camera;
            this.timer = null;
        }
        SelectionManager.prototype.getClassName = function () {
            return "SelectionManager";
        };
        SelectionManager.prototype.createIndicator = function (mesh) {
            this.activeSelections.push(new SYNGLYPHX.SelectionIndicator(mesh, this.yellow));
            return this;
        };
        SelectionManager.prototype.destroyIndicators = function () {
            console.log(this.activeSelections.length);
            for(var i = 0; i < this.activeSelections.length; i++){
                this.activeSelections[i].getClone().dispose();   
            }
            this.activeSelections = new Array();
            return this;
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
            this.clone.position = mesh.position;
            this.clone.rotation = mesh.rotation;
            this.scale = [mesh.scaling.x, mesh.scaling.y, mesh.scaling.z];
            this.clone.scaling = new BABYLON.Vector3(this.scale[0]*0.95, this.scale[1]*0.95, this.scale[2]*0.95);
        }
        SelectionIndicator.prototype.getClassName = function () {
            return "SelectionIndicator";
        };
        SelectionIndicator.prototype.getClone = function () {
            return this.clone;
        };
        SelectionIndicator.prototype.resetScale = function () {
            this.clone.scaling = new BABYLON.Vector3(this.scale[0]*0.95, this.scale[1]*0.95, this.scale[2]*0.95);
            return this;
        };
        SelectionIndicator.prototype.incrementScale = function (increment) {
            var scale = [this.clone.scaling.x, this.clone.scaling.y, this.clone.scaling.z];
            this.clone.scaling = new BABYLON.Vector3(scale[0]+increment, scale[1]+increment, scale[2]+increment);
            return this;
        };
        return SelectionIndicator;
    }());
    SYNGLYPHX.SelectionIndicator = SelectionIndicator;

})(SYNGLYPHX || (SYNGLYPHX = {}));