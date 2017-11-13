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
        function FilterManager () {
            this.rootGlyphs = new Map();
        }
        FilterManager.prototype.getClassName = function () {
            return "FilterManager";
        };
        FilterManager.prototype.getRootGlyph = function (id) {
            return this.rootGlyphs.get(id);
        };
        FilterManager.prototype.addRootGlyph = function (id, mesh) {
            this.rootGlyphs.set(id, mesh);
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
            this.rootGlyphs.forEach(function(value, key, map) {
               value.setEnabled(1);
            });
            return this;
        };
        return FilterManager;
    }());
    SYNGLYPHX.FilterManager = FilterManager;

})(SYNGLYPHX || (SYNGLYPHX = {}));