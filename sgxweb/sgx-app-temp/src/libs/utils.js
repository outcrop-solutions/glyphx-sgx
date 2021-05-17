
import { Vector3, Color3, DirectionalLight, MeshBuilder, StandardMaterial, Texture } from '@babylonjs/core';

const PI = 3.14159;
const MaxZoomLevel = 18;
const MetersPerPixelAtZoom0 = 156543.034;
const EarthRadiusInMeters = 6372798.2;
const MetersPerDegreeLongitude = 111319.892;
const MetersPerDegreeLatitude = 110540.0;
const DegToRad = PI / 180.0;

export function GetZoomLevel(minX, maxX, minY, maxY, centerX, centerY, useBestFit){

    let NorthCenter = [maxY, centerX];
    let SouthCenter = [minY, centerX];
    let EastCenter = [centerY, maxX];
    let WestCenter = [centerY, minX];
    let Center = [centerY, centerX];
    let width = maxX - minX;
    let imageSize = [2048, 1024];
    /*console.log("Centers:");
    console.log(NorthCenter);
    console.log(SouthCenter);
    console.log(EastCenter);
    console.log(WestCenter);*/

    let m_mapQuestMetersPerPixelAtEquator = [];
    for (let i = 1; i <= MaxZoomLevel; ++i) {
        m_mapQuestMetersPerPixelAtEquator[i] = MetersPerPixelAtZoom0 / Math.pow(2.0, i);
    }

    let cosineAtCenter = Math.cos(Center[0] * DegToRad);
    
	let hDistanceInMeters = getDistance(WestCenter, EastCenter);
	if (width > 180.0) {

		hDistanceInMeters = (EarthRadiusInMeters * cosineAtCenter * 2.0 * PI) - hDistanceInMeters;
	}
    let vDistanceInMeters = getDistance(NorthCenter, SouthCenter);
    console.log(hDistanceInMeters);
    console.log(vDistanceInMeters);

    if (useBestFit) {

		for (let i = MaxZoomLevel; i > 0; --i) {

			let metersPerPixelAtZoom = m_mapQuestMetersPerPixelAtEquator[i] * cosineAtCenter;
			let imageSizeAtZoom = [];
			imageSizeAtZoom[0] = hDistanceInMeters / metersPerPixelAtZoom;
			imageSizeAtZoom[1] = vDistanceInMeters / metersPerPixelAtZoom;
			if ((imageSizeAtZoom[0] < imageSize[0]) && (imageSizeAtZoom[1] < imageSize[1])) {

				imageSize[0] = Math.ceil(imageSizeAtZoom[0]);
                imageSize[1] = Math.ceil(imageSizeAtZoom[1]);
                let metersPerPixelAtCurrentZoom = m_mapQuestMetersPerPixelAtEquator[i] * cosineAtCenter;
                let lonRadiusInDegrees = (metersPerPixelAtCurrentZoom  * (imageSize[0] / 2.0)) / (MetersPerDegreeLongitude  * cosineAtCenter);
                let latRadiusInDegrees = (metersPerPixelAtCurrentZoom * (imageSize[1] / 2.0)) / MetersPerDegreeLatitude;
                let m_swCorner = [centerX - lonRadiusInDegrees, centerY - latRadiusInDegrees];
                let m_neCorner = [centerX + lonRadiusInDegrees, centerY + latRadiusInDegrees];
                console.log("In function corners:");
                console.log(m_swCorner);
                console.log(m_neCorner);
				return [i, imageSize[0], imageSize[1]];
			}
		}
    }
    
    let hZoomLevel = Math.log((MetersPerPixelAtZoom0 * cosineAtCenter) / (hDistanceInMeters / imageSize[0])) / Math.log(2.0); // / cosineAtCenter);
    let vZoomLevel = Math.log((MetersPerPixelAtZoom0 * cosineAtCenter) / (vDistanceInMeters / imageSize[1])) / Math.log(2.0);

    return [Math.floor(Math.min(Math.min(hZoomLevel, vZoomLevel), MaxZoomLevel)), imageSize[0], imageSize[1]];
}

function getDistance(pointA, pointB) { 

    const degreesToRadians = degrees => degrees * (Math.PI / 180);
    const radiansToDegrees = radians => radians * (180 / Math.PI);

    const centralSubtendedAngle = (locationX, locationY) => {
        const locationXLatRadians = degreesToRadians(locationX.latitude)
        const locationYLatRadians = degreesToRadians(locationY.latitude)
      return radiansToDegrees(
          Math.acos(
            Math.sin(locationXLatRadians) * Math.sin(locationYLatRadians) +
              Math.cos(locationXLatRadians) *
                Math.cos(locationYLatRadians) *
                Math.cos(
                  degreesToRadians(
                    Math.abs(locationX.longitude - locationY.longitude)
                  )
             )
          )
        )
      }

    const greatCircleDistance = angle => 2 * Math.PI * EarthRadiusInMeters * (angle / 360);
    const distanceBetweenLocations = (locationX, locationY) =>
        greatCircleDistance(centralSubtendedAngle(locationX, locationY));

    let a = {latitude: pointA[0], longitude: pointA[1]};
    let b = {latitude: pointB[0], longitude: pointB[1]};
    return distanceBetweenLocations(a, b);
}

export function LinearInterpolation(x1, x3, y1, y3, x2) {
  if(x3-x1 === 0){
    if(x2 >= y1 && x2 <= y3){
      return x2;
    }else{
      return y3;
    }
  }
  return (((x2-x1)*(y3-y1))/(x3-x1))+y1;
}

export function TextInterpolation(field, y1, y3, x2_str, sub, sorted){
  let dataCpy;
  if(field in sorted){
      dataCpy = sorted[field];
  }
  else{
      dataCpy = [...sub[field]];
      dataCpy.sort();
      sorted[field] = dataCpy;
  }

  let x1 = 0;
  let x3 = dataCpy.length;
  let x2 = dataCpy.indexOf(x2_str);
  return LinearInterpolation(x1, x3, y1, y3, x2);
}

export function ProcessSubset(scene, subset, fields, field_data, sorted, glyphs, xInt, yInt, zInt, needsMap) {
  let sub;
  try {
    sub = JSON.parse(subset);
  }
  catch(e) {
    sub = subset;
  }
  if(fields['xAxis'] !== "" || fields['yAxis'] !== ""){
      let total_count = sub[Object.keys(sub)[0]].length;
      console.log(glyphs.length, total_count);
      for(let i = 0; i < total_count; i++){
          let glyph;

          if(needsMap){
              if(fields['xAxis'].toLowerCase().includes('long') &&
                  fields['yAxis'].toLowerCase().includes('lat')) {

                  let x = field_data[fields['xAxis']];
                  let y = field_data[fields['yAxis']];
                  let minX = parseFloat(x['min']), maxX = parseFloat(x['max']);
                  let minY = parseFloat(y['min']), maxY = parseFloat(y['max']);
                  let centerX = ((minX + maxX) / 2.0);
                  let centerY = ((minY + maxY) / 2.0);
                  let center = centerY + "," + centerX;
                  console.log("Zoom:", GetZoomLevel(minX, maxX, minY, maxY, centerX, centerY, true));
                  let gzl = GetZoomLevel(minX, maxX, minY, maxY, centerX, centerY, true);
                  let zoom = gzl[0];
                  let mapWidth = gzl[1], mapHeight = gzl[2];
                  //let mapWidth = 2048, mapHeight = 1024;
                  let size = mapWidth+","+mapHeight;
                  let groundWidth = mapWidth/25, groundHeight = mapHeight/25;

                  //let apiKey = "AIzaSyDPO-xd-zLjtZ4-dYkGrU-qE7NvmTMM52s";
                  //let mapUrl = "https://maps.googleapis.com/maps/api/staticmap?center="+center+"&zoom="+zoom+"&size="+size+"&key="+apiKey;
                  let map = false, satellite = false;
                  let apiKey = "pba2AzAMjfkGrvf7GUuA2nWX9RHclALU";
                  let mapUrl = "http://open.mapquestapi.com/staticmap/v4/getmap?key=" + apiKey;
                  mapUrl += "&center=" + center;
                  mapUrl += "&zoom=" + zoom + "&size=" + size;
                  mapUrl += "&type=";
                  if (map) {
                      mapUrl += "map";
                  }
                  else if (satellite) {
                      mapUrl += "sat";
                  }
                  else {
                      mapUrl += "hyb";
                  }
                  mapUrl += "&imagetype=png";
                  mapUrl += "&pois=";
                  console.log(mapUrl);

                  var base_image_light = new DirectionalLight("Dir0", new Vector3(0, -1, 0), scene);
                  base_image_light.specular = new Color3(0, 0, 0);
                  base_image_light.intensity = 1.0;

                  var baseImage = new StandardMaterial("ground", scene);
                  baseImage.diffuseTexture = new Texture(mapUrl, scene);

                  let ground = MeshBuilder.CreateGround("ground", {width: groundWidth, height: groundHeight}, scene);
                  ground.position.y = -1;
                  ground.material = baseImage;
                  var bbMax = ground.getBoundingInfo().boundingBox.maximum;
                  xInt = bbMax.x;
                  yInt = bbMax.z;
                  console.log(xInt, yInt);
                  base_image_light.includedOnlyMeshes.push(ground);

                  needsMap = false;
              }
          }
          // Our built-in 'box' shape.
          if(glyphs.length !== total_count){
              glyph = MeshBuilder.CreateBox("box", {size: 0.5}, scene);
              glyphs.push(glyph);
          }
          else{
              glyph = glyphs[i];
          }

          if(fields['gType'] !== ""){
              let toChange = Math.floor(Math.random() * Math.floor(2));
              if(toChange === 1){
                  let mat = glyph.material;
                  glyph.dispose();
                  glyph = MeshBuilder.CreateSphere("Sphere", {segments: 16, diameter: 1}, scene);
                  glyphs[i] = glyph;
                  glyph.material = mat;
              }
          }
          if(fields['xAxis'] !== ""){
              let f = field_data[fields['xAxis']];
              if(field_data[fields['xAxis']]["type"] !== "string"){
                  glyph.position.x = LinearInterpolation(f["min"], f["max"], xInt*-1, xInt, sub[fields['xAxis']][i]);
              }
              else{
                  glyph.position.x = TextInterpolation(fields['xAxis'], xInt*-1, xInt, sub[fields['xAxis']][i], sub, sorted);
              }
          }
          if(fields['yAxis'] !== ""){
              let f = field_data[fields['yAxis']];
              if(field_data[fields['yAxis']]["type"] !== "string"){
                  glyph.position.z = LinearInterpolation(f["min"], f["max"], yInt*-1, yInt, sub[fields['yAxis']][i]);
              }
              else{
                  glyph.position.z = TextInterpolation(fields['yAxis'], yInt*-1, yInt, sub[fields['yAxis']][i], sub, sorted);
              }
          }
          if(fields['zAxis'] !== ""){
              let f = field_data[fields['zAxis']];
              if(field_data[fields['zAxis']]["type"] !== "string"){
                  glyph.position.y = LinearInterpolation(f["min"], f["max"], 0, zInt, sub[fields['zAxis']][i]);
              }
              else{
                  glyph.position.y = TextInterpolation(fields['zAxis'], 0, zInt, sub[fields['zAxis']][i], sub, sorted);
              }
          }
          if(fields['gColor'] !== ""){
              let f = field_data[fields['gColor']];
              let mat = new StandardMaterial("sm", scene);
              let r, g, b;
              if(field_data[fields['gColor']]["type"] !== "string"){
                  r = LinearInterpolation(f["min"], f["max"], 1, 0, sub[fields['gColor']][i]);
                  g = LinearInterpolation(f["min"], f["max"], 0, 0.5, sub[fields['gColor']][i]);
                  b = LinearInterpolation(f["min"], f["max"], 0, 0, sub[fields['gColor']][i]);
              }
              else{
                  r = TextInterpolation(fields['gColor'], 1, 0, sub[fields['gColor']][i], sub, sorted);
                  g = TextInterpolation(fields['gColor'], 0, 0.5, sub[fields['gColor']][i], sub, sorted);
                  b = TextInterpolation(fields['gColor'], 0, 0, sub[fields['gColor']][i], sub, sorted);
              }
              mat.emissiveColor = new Color3(r, g, b);
              mat.diffuseColor = new Color3(r, g, b);
              glyph.material = mat;
          }
          if(fields['gSize'] !== ""){
              let f = field_data[fields['gSize']];
              let value;
              if(field_data[fields['gSize']]["type"] !== "string"){
                  value = LinearInterpolation(f["min"], f["max"], 0.5, 1, sub[fields['gSize']][i]);
              }
              else{
                  value = TextInterpolation(fields['gSize'], 0.5, 1, sub[fields['gSize']][i], sub, sorted);
              }
              glyph.scaling = new Vector3(value, value, value);
          }
      }
      console.log(fields);
      console.log(scene.getNodes().length);
  }else{
      //let box;
      // Our built-in 'box' shape.
      //box = MeshBuilder.CreateBox("box", {size: 2}, scene);
      //MeshBuilder.CreateSphere("Sphere"+color, 16, 1, scene);

      // Move the box upward 1/2 its height
      //box.position.y = 1;

      // Our built-in 'ground' shape.
      //MeshBuilder.CreateGround("ground", {width: 6, height: 6}, scene);

      //glyphs.push(box);
  }

}