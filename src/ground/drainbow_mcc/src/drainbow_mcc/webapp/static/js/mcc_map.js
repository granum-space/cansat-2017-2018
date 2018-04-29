function mccMapMain() {
    var map = new ol.Map({
        layers: [
            new ol.layer.Tile({
                source: new ol.source.TileArcGISRest({
                    url: 'http://server.arcgisonline.com/arcgis/rest/services/ESRI_Imagery_World_2D/MapServer'
                })
            })
        ],
        target: 'map',
        controls: ol.control.defaults({
          attributionOptions: {
            collapsible: false
          }
        }),
        view: new ol.View({
          center: [ 4188426.7147939987, 7508764.236877314 ],
          zoom: 12
        })
    });

    var source = new ol.source.Vector({wrapX: false});

    var vector = new ol.layer.Vector({
        source: source
    });

    map.addLayer(vector)

    var feature = new ol.Feature({
        geometry: new ol.geom.Point([ 4188426.7147939987, 7508764.236877314 ]),
        name: 'My Point'
    });
    feature.geometry

    source.addFeature(feature);
}