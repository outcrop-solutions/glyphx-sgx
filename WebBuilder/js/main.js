angular
  .module('MyApp', ['ngMaterial', 'ngMaterialCollapsible', 'selectize'])
  .config(function($mdIconProvider, $mdAriaProvider) {
    $mdIconProvider.defaultIconSet('images/icons/core-icons.svg', 24);
    $mdAriaProvider.disableWarnings();
  })
  .service('sharedProperties', function () {
        var fields = ["Field_1", "Field_2", "Field_3", "Field_4", "Field_5", "Field_6", "Field_7", "Field_8", "Field_9", "Field_10", 
            "Field_11", "Field_12", "Field_13", "Field_14", "Field_15", "Field_16", "Field_17", "Field_18", "Field_19", "Field_20", 
            "Field_21", "Field_22", "Field_23", "Field_24", "Field_25", "Field_26", "Field_27", "Field_28", "Field_29", "Field_30"];

        var geometries = ['Cube','Sphere','Cone','Torus','Dodecahedron','Octahedron','Tetrahedron','Icosahedron','Pin','Cylinder'];
        var colors = ["White", "Black", "Red", "Green", "Blue", "Cyan", "Magenta", "Yellow", "Gray", "Purple", "Orange", "Brown", "Pink", "Gold", "Silver"];
        var maps = ["Position","Scale","Color","Geometry"];

        var selectedField = 0;
        var draggedInScene = false;

        var shapeElement = null;
        var colorElement = null;
        var mappElement = null;

        return {
            getFields: function () {
              return fields;
            },
            setFields: function (values) {
              fields = values;
            },
            getGeometries: function () {
              return geometries;
            },
            getColors: function () {
              return colors;
            },
            getMaps: function () {
              return maps;
            },
            getSelectedField: function () {
              return selectedField;
            },
            setSelectedField: function (value) {
              selectedField = value;
              //console.log("Picked up", value);
              //console.log("Selected Id", getSelectedId());
              //<i class="fa fa-cube fa-2x" aria-hidden="true"></i>
            },
            getLastMappingChoice: function () {
              //return document.getElementById('glyphwindow').contentWindow.getLastMappingChoice();
              return getLastMappingChoice();
            },
            setLastMappingChoice: function (value) {
              //document.getElementById('glyphwindow').contentWindow.setLastMappingChoice(value);
              setLastMappingChoice(value);
            },
            setDragInScene: function (value) {
              draggedInScene = value;
            },
            isDragInScene: function () {
              return draggedInScene;
            },
            updateProperty: function (name, id) {
              //console.log(name, id);
              var min = document.getElementById(name+'min');
              var max = document.getElementById(name+'max');
              var field = null;

              if(name != 'Srf'){
                field = fields.includes(id) ? id : id != undefined && fields.includes(document.getElementById(id).value) ? document.getElementById(id).value : null;
              }
              updateGlyphProperty(name, field, min ? min.value : null, max ? max.value : null);

            },
            updateShapeElement: function(el) {
              if(shapeElement != null){
                shapeElement.classList.remove('selected-class-name');
              }
              el.classList.add('selected-class-name');
              shapeElement = el;
            },
            updateColorElement: function(el) {
              if(colorElement != null){
                colorElement.classList.remove('selected-class-name');
              }
              el.classList.add('selected-class-name');
              colorElement = el;
            },
            updateMappElement: function(el) {
              if(mappElement != null){
                mappElement.classList.remove('selected-class-name');
              }
              el.classList.add('selected-class-name');
              mappElement = el;
            },
            clearSelectedElements: function() {
              if(shapeElement != null){
                shapeElement.classList.remove('selected-class-name');
                shapeElement = null;
              }
              if(colorElement != null){
                colorElement.classList.remove('selected-class-name');
                colorElement = null;
              }
              if(mappElement != null){
                mappElement.classList.remove('selected-class-name');
                mappElement = null;
              }
            },
            addNewGlyphMember: function() {
              if(shapeElement != null && colorElement != null && mappElement != null){
                //console.log(selectedField,shapeElement.getAttribute("aria-label"),colorElement.getAttribute("aria-label"),mappElement.getAttribute("aria-label"));
                addNewGlyphElement(geometries.indexOf(shapeElement.getAttribute("aria-label")),colors.indexOf(colorElement.getAttribute("aria-label")));

                var ids = ['PosX','Scl','Clr','Geo'];

                if(glyph.lastAddedId() == 0){
                  document.getElementById('Clrmax').value = getMaterialColor();
                  var hex = document.getElementById('Clrmax').value;
                  document.getElementById('Clrmin').value = rybComp(hex);
                  document.getElementById('Geomax').value = shapeElement.getAttribute("aria-label");
                  document.getElementById(ids[maps.indexOf(mappElement.getAttribute("aria-label"))]).value = selectedField;
                  document.getElementById('Tag').value = selectedField;
                }
                glyph.getElement(glyph.lastAddedId()).updateProperty('Geo', null, null, shapeElement.getAttribute("aria-label"));

                glyph.getElement(glyph.lastAddedId()).updateBindOnly(ids[maps.indexOf(mappElement.getAttribute("aria-label"))], selectedField);
                glyph.getElement(glyph.lastAddedId()).updateBindOnly('Tag', selectedField);
                
                glyph.getElement(glyph.getSelectedId()).updatePropertiesPane();
                //logProperties(glyph.lastAddedId());
              }
            }
        };
  })
  .directive('draggable', function(sharedProperties) {
    return function(scope, element) {
      // this gives us the native JS object
      var el = element[0];
      
      el.draggable = true;
      
      el.addEventListener(
        'dragstart',
        function(e) {
          e.dataTransfer.effectAllowed = 'move';
          e.dataTransfer.setData('Text', this.id);
          this.classList.add('drag');
          sharedProperties.setSelectedField(this.id);
          return false;
        },
        false
      );
      
      el.addEventListener(
        'dragend',
        function(e) {
          this.classList.remove('drag');
          if(sharedProperties.isDragInScene()){
            sharedProperties.addNewGlyphMember();
            sharedProperties.clearSelectedElements();
          }
          return false;
        },
        false
      );
    }
  })
  .directive('droppable', function(sharedProperties) {
    return {
      scope: {
        drop: '&',
        bin: '='
      },
      link: function(scope, element) {
        // again we need the native object
        var el = element[0];

        el.addEventListener(
          'dragenter',
          function(e) {
            //console.log(e);
            if(sharedProperties.getGeometries().includes(this.getAttribute("aria-label"))){
              sharedProperties.updateShapeElement(this);
            }
            else if(sharedProperties.getColors().includes(this.getAttribute("aria-label"))){
              sharedProperties.updateColorElement(this);
            }
            else if(sharedProperties.getMaps().includes(this.getAttribute("aria-label"))){
              sharedProperties.updateMappElement(this);
            }
            else{
              sharedProperties.setDragInScene(true);
            }
            return false;
          },
          false
        );

        el.addEventListener(
          'dragleave',
          function(e) {
            if(sharedProperties.isDragInScene() && e.screenX != 0 && e.screenY != 0){
              sharedProperties.setDragInScene(false);
            }
            return false;
          },
          false
        );

      }
    }
  })
  .filter('keyboardShortcut', function($window) {
    return function(str) {
      if (!str) return;
      var keys = str.split('-');
      var isOSX = /Mac OS X/.test($window.navigator.userAgent);

      var separator = (!isOSX || keys.length > 2) ? '+' : '';

      var abbreviations = {
        M: isOSX ? '⌘' : 'Ctrl',
        A: isOSX ? 'Option' : 'Alt',
        S: 'Shift'
      };

      return keys.map(function(key, index) {
        var last = index === keys.length - 1;
        return last ? key : abbreviations[key];
      }).join(separator);
    };
  })
  .controller('DemoBasicCtrl', function DemoCtrl($mdDialog) {
    this.settings = {
      printLayout: true,
      showRuler: true,
      showSpellingSuggestions: true,
      presentationMode: 'edit'
    };

    this.sampleAction = function(name, ev) {
      $mdDialog.show($mdDialog.alert()
        .title(name)
        .textContent('You triggered the "' + name + '" action')
        .ok('Great')
        .targetEvent(ev)
      );
    };
  })
  .controller('DataCtrl', function($scope, sharedProperties) {
    $scope.fields = Object.values(sharedProperties.getFields());
    
    $scope.selectedIndex = sharedProperties.getSelectedField();
    
    $scope.itemClicked = function ($index) {
      console.log($index);
      $scope.selectedIndex = $index;
      sharedProperties.setSelectedField($index);
    }
  })
  .controller('ShapeCtrl', function($scope, sharedProperties) {
    $scope.shapes = sharedProperties.getGeometries();
    
    $scope.selectedIndex = null;
    
    $scope.itemClicked = function ($index) {
      /*onsole.log("Shape:", sharedProperties.getGeometries()[$index]);
      $scope.selectedIndex = $index;
      document.getElementById('Geomin').value = sharedProperties.getGeometries()[$index];
      //document.getElementById('glyphwindow').contentWindow.changeShape(0, $index);
      changeShape(0, $index);*/
    }
  })
  .controller('ColorCtrl', function($scope, sharedProperties) {
    $scope.colors = sharedProperties.getColors();
    
    $scope.selectedIndex = null;
    
    $scope.itemClicked = function ($index) {
      /*console.log("Color:", $scope.colors[$index]);
      $scope.selectedIndex = $index;
      document.getElementById('Clrmax').value = changeColor(0, $index);//document.getElementById('glyphwindow').contentWindow.changeColor(0, $index);
      var hex = document.getElementById('Clrmax').value;
      console.log(hex);
      document.getElementById('Clrmin').value = rybComp(hex);
      console.log(rybComp(hex));
      sharedProperties.updateProperty('Clr','Clr');*/
    }
  })
  .controller('MapCtrl', function($scope, sharedProperties) {
    $scope.collection = [["images/icon-axis-scene.png","Position"], ["images/icon-show-groups.png","Scale"], ["images/icon-xyz-a.png","Color"], ["images/icon-xyz.png","Geometry"]];
    
    $scope.selectedIndex = null;
    
    $scope.itemClicked = function ($index) {
      /*console.log($index);
      var lastChoice = sharedProperties.getLastMappingChoice();
      var ids = ['PosX','Scl','Clr','Geo'];
      $scope.selectedIndex = $index;
      sharedProperties.setLastMappingChoice($index);
      document.getElementById(ids[$index]).value = sharedProperties.getFields()[sharedProperties.getSelectedField()];
      document.getElementById('Tag').value = sharedProperties.getFields()[sharedProperties.getSelectedField()];
      sharedProperties.updateProperty(ids[$index],ids[$index]);

      if($index == 1){
        document.getElementById(ids[$index]+'X').value = sharedProperties.getFields()[sharedProperties.getSelectedField()];
        document.getElementById(ids[$index]+'Y').value = sharedProperties.getFields()[sharedProperties.getSelectedField()];
        document.getElementById(ids[$index]+'Z').value = sharedProperties.getFields()[sharedProperties.getSelectedField()];
        sharedProperties.updateProperty('SclX','SclX');
        sharedProperties.updateProperty('SclY','SclY');
        sharedProperties.updateProperty('SclZ','SclZ');
      }
      if(lastChoice != null){
        document.getElementById(ids[lastChoice]).value = '';
        sharedProperties.updateProperty(ids[lastChoice],ids[lastChoice]);
        if(lastChoice == 1){
          document.getElementById(ids[lastChoice]+'X').value = '';
          document.getElementById(ids[lastChoice]+'Y').value = '';
          document.getElementById(ids[lastChoice]+'Z').value = '';
          sharedProperties.updateProperty('SclX','SclX');
          sharedProperties.updateProperty('SclY','SclY');
          sharedProperties.updateProperty('SclZ','SclZ');
        }
      }*/
    }
  })
  .controller('BaseImageCtrl', function($scope, sharedProperties) {

    $scope.typeOptions = ['Default','Downloaded Map','Local Image'];
    $scope.BIType = 'Default';
    $scope.dfltOptions = ['World','World (Grayscale)','Black','Gray','White','Clear'];
    $scope.BIImage = 'World';
    $scope.mapOptions = ['Map','Satellite','Hybrid'];
    $scope.BIMap = 'Map';
    $scope.Mapimgsize = 50;
    $scope.WSwidth = 360;
    $scope.WSheight = 180;

    $scope.propertyChanged = function(select) {
      
    }

    $scope.isSet = function(text) {
      return $scope.BIType === text;
    }

  })
  .controller('BackgroundCtrl', function($scope, sharedProperties) {

    $scope.propertyChanged = function(select) {
      updateBackgroundColor(select.Bgcolor);
    }

  })
  .controller('MappingCtrl', function($scope, sharedProperties) {
    $scope.disable = false;
    $scope.display = true;

    //=======================================================
    //Tagging
    //=======================================================
    $scope.tagOptions = sharedProperties.getFields();
    $scope.geoOptions = sharedProperties.getGeometries();
    $scope.srfOptions = ['Solid','Wireframe'];
    $scope.Geomax = 'Cube';
    $scope.Srf = 'Solid';
    $scope.Sclmin = 0.1, $scope.SclXmin = 0.1, $scope.SclYmin = 0.1, $scope.SclZmin = 0.1;
    $scope.Sclmax = 1.1, $scope.SclXmax = 1.1, $scope.SclYmax = 1.1, $scope.SclZmax = 1.1;
    $scope.Clrmin = '#00ff00';
    $scope.Clrmax = '#ff0000';
    $scope.Alphmin = 0;
    $scope.Alphmax = 255;

    $scope.propertyChanged = function(select) {
      var keys = ['PosX','PosY','PosZ',
                  'RotX','RotY','RotZ',
                  'Scl','SclX','SclY','SclZ',
                  'Clr','Alph',
                  'Geo','Srf','Rto',
                  'Tag','Url'];

      //console.log("propertyChanged");
      for(i in keys){
        if(select.hasOwnProperty(keys[i])){
          sharedProperties.updateProperty(keys[i], select[keys[i]]);
        }
        else if(select.hasOwnProperty(keys[i]+'min') || select.hasOwnProperty(keys[i]+'max')){
          if($scope.tagOptions.includes(document.getElementById(keys[i]).value)){
            sharedProperties.updateProperty(keys[i], document.getElementById(keys[i]).value);
          }
          else{
            sharedProperties.updateProperty(keys[i], keys[i]);
          }
        }
      }
      /*if(select.hasOwnProperty('Scl') || select.hasOwnProperty('Sclmin') || select.hasOwnProperty('Sclmax')){
        var mastmin = document.getElementById('Sclmin').value;
        var mastmax = document.getElementById('Sclmax').value;
        var mastfld = document.getElementById('Scl').value;
        var scls = ['SclX','SclY','SclZ'];
        for(i in scls){
          document.getElementById(scls[i]+'min').value = mastmin;
          document.getElementById(scls[i]+'max').value = mastmax;
          if($scope.tagOptions.includes(mastfld)){
            document.getElementById(scls[i]).value = mastfld;
          }
          sharedProperties.updateProperty(scls[i],scls[i]);
        }
      }*/
    }

  });


/**
Copyright 2016 Google Inc. All Rights Reserved. 
Use of this source code is governed by an MIT-style license that can be foundin the LICENSE file at http://material.angularjs.org/HEAD/license.
**/