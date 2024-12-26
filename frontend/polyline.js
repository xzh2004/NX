var map = L.map('mapDiv').setView([31.30021, 121.49918], 14);
    L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
        maxZoom: 19,
        attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
    }).addTo(map);
    var start_node = L.marker([31.300555, 121.498994, -0.09]).addTo(map);
    var end_node = L.marker([31.02841, 121.43128, -0.09]).addTo(map);
        var latlngs = [
            [45.51, -122.68],
            [37.77, -122.43],
            [34.04, -118.2]
        ];
        var polyline = L.polyline(latlngs, {
            color: 'red',
            weight: 3,
            smoothFactor: 3
        }).addTo(map);
// 绘制带箭头的线条（路径）
// var arrow = L.polyline(latlngs, {}).addTo(map);
// var arrowHead = L.polylineDecorator(arrow, {
//     patterns: [
//         {offset: '100%', repeat: 0, symbol: L.Symbol.arrowHead({pixelSize: 15, polygon: false, pathOptions: {stroke: true}})}
//     ]
// }).addTo(map);
     

    // map.fitBounds(polyline.getBounds());