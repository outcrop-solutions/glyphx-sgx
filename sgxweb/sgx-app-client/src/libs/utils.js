
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