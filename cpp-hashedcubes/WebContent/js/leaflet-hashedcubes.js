function heatmap_layer(value) {

    var canvas_layer = L.tileLayer.canvas({
        minZoom: 0,
        maxZoom: 20,
        unloadInvisibleTiles: false,
        updateWhenIdle: false,
        reuseTiles: false,
        noWrap: true
    });

    canvas_layer.drawTile = function (canvas, coords, zoom) {

        currTile = value;

        var context = canvas.getContext('2d');
        context.globalCompositeOperation = 'multiply';

        var query_map = "/tile/" + value + "/" + coords.x + "/" + coords.y + "/" + zoom + "/" + heatmap_resolution + where + tseries;

        $.ajax({
            type: 'GET',
            url: _queryURL + "/tile" + query_map,
            dataType: "json",
            success: function (data) {
                var entry = {
                    data: data,
                    context: context,
                    tile_x: coords.x,
                    tile_y: coords.y,
                    tile_zoom: zoom
                };

                color_tile(entry);
            }
        });
    };

    var update = function (options) {
        canvas_layer.redraw();
    };

    callbacks.add(update);

    return canvas_layer;
}

function color_tile(entry) {
    entry.context.clearRect(0, 0, 256, 256);

    var fs = pickDrawFuncs();

    entry.data.forEach(function (d) {
        var lon0 = tilex2lon(d[0], d[2]);
        var lat0 = tiley2lat(d[1], d[2]);
        var lon1 = tilex2lon(d[0] + 1, d[2]);
        var lat1 = tiley2lat(d[1] + 1, d[2]);

        var x0 = (lon2tilex(lon0, entry.tile_zoom) - entry.tile_x) * 256;
        var y0 = (lat2tiley(lat0, entry.tile_zoom) - entry.tile_y) * 256;
        var x1 = (lon2tilex(lon1, entry.tile_zoom) - entry.tile_x) * 256;
        var y1 = (lat2tiley(lat1, entry.tile_zoom) - entry.tile_y) * 256;

        var datum = {
            data_zoom: d[2],
            count: d[3],
            tile_zoom: entry.tile_zoom,
            x0: x0,
            y0: y0,
            x1: x1,
            y1: y1
        };

        entry.context.fillStyle = fs.color(d[3] * fs.count_transform(datum));
        
        fs.draw(entry.context, datum);
    });
}

function pickDrawFuncs() {
    var colormaps = {
        ryw: function (count) {
            var lc = Math.log(count + 1) / Math.log(10);

            var r = Math.floor(256 * Math.min(1, lc));
            var g = Math.floor(256 * Math.min(1, Math.max(0, lc - 1)));
            var b = Math.floor(256 * Math.min(1, Math.max(0, lc - 2)));

            var a = Math.min(1, lc);

            return "rgba(" + r + "," + g + "," + b + "," + a + ")";
        },
        bbb: d3.scale.linear()
            .domain([1, 10000])
            .range(['#87CEFA', 'black'])
            .clamp(true)
    };


    var drawfuncs = {
        circle: function draw_circle(context, datum) {
            var radius = 2.0;
            var midx = (datum.x0 + datum.x1) / 2;
            var midy = (datum.y0 + datum.y1) / 2;
            context.beginPath();
            context.arc(midx, midy, radius, 0, 2 * Math.PI);
            context.fill();
        },
        rect: function draw_rect(context, datum) {
            var width = datum.x1 - datum.x0;
            var height = datum.y1 - datum.y0;
            context.fillRect(datum.x0, datum.y0, width, height);
        }
    };

    var transforms = {
        density_scaling: function (datum) {
            /*
             * area ~ 2 ** (-data_zoom)
             * color ~ density = count / area = count * 2 ** data_zoom
             *                                          ^^^^^^^
             *
             * The range of areas on the map is so large that most nodes
             * are a very low or very high density. To "correct" for this,
             * we scale by our current zoom level also (things get lighter
             * as we zoom in), similar to nanocubes/brightkite.
             *
             * BRIGHTNESS is linked to the UI control (see bottom of file)
             */
            return Math.pow(2, datum.data_zoom - datum.tile_zoom + BRIGHTNESS);
        },
        no_scaling: function () {
            return 1;
        }
    };

    return {
        draw: drawfuncs[PLOTTING_MODE],
        count_transform: transforms[PLOTTING_TRANSFORM],
        color: colormaps[PLOTTING_COLOR_SCALE]
    };
}

/* Controls for color scale */
var BRIGHTNESS = -7;
var PLOTTING_MODE = view_schemas[_schema].PLOTTING_MODE;
var PLOTTING_COLOR_SCALE = view_schemas[_schema].PLOTTING_COLOR_SCALE;
var PLOTTING_TRANSFORM = view_schemas[_schema].PLOTTING_TRANSFORM;

var brighter = function () {
    ++BRIGHTNESS;
    callbacks.fire({ isColorOnly: true });
}

var fainter = function () {
    --BRIGHTNESS;
    callbacks.fire({ isColorOnly: true });
}

var setMode = function (mode) {
    PLOTTING_MODE = mode;
    console.log(PLOTTING_MODE);
    callbacks.fire({ isColorOnly: true });
};

var setScale = function (scale) {
    PLOTTING_COLOR_SCALE = scale;
    console.log(PLOTTING_COLOR_SCALE);
    callbacks.fire({ isColorOnly: true });
};

var setTransform = function (transform) {
    PLOTTING_TRANSFORM = transform;
    console.log(PLOTTING_TRANSFORM);
    callbacks.fire({ isColorOnly: true });
};
