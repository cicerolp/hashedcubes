function loadMap() {
    var layers = {
        black: 'https://api.mapbox.com/styles/v1/calpahins/cjh7nygud63km2sq5zp0pikfw/tiles/256/{z}/{x}/{y}@2x?access_token=pk.eyJ1IjoiY2FscGFoaW5zIiwiYSI6ImNqaDduaGVtdDBhM28zM21qN2hoOTh1d2IifQ.JyQl2tr6nStL5271bNz7FA',
        white: 'https://api.mapbox.com/styles/v1/calpahins/cjh7nizdb615e2rk3btgrklul/tiles/256/{z}/{x}/{y}@2x?access_token=pk.eyJ1IjoiY2FscGFoaW5zIiwiYSI6ImNqaDduaGVtdDBhM28zM21qN2hoOTh1d2IifQ.JyQl2tr6nStL5271bNz7FA'
    }

    var baseLayer = L.tileLayer(layers[view_schemas[_schema].PLOTTING], {
        subdomains: "abcd",
        minZoom: 0,
        maxZoom: 20,        
        maxNativeZoom: 20,
        unloadInvisibleTiles: false,
        updateWhenIdle: false,
        reuseTiles: false,
        noWrap: true,
    });
    

    var layer_group = [];
    var layer_default = null;
    _view.tile.forEach(function (entry) {
        layer_group[entry.title] = heatmap_layer(entry.value);
        if (layer_default == null) layer_default = entry.title;
    });
    
    map = new L.Map('map', {
        layers: [baseLayer, layer_group[layer_default]],
        center : new L.LatLng(0, 0),
        zoom : 3,
        minZoom: 0,
        maxZoom: 20,
        worldCopyJump: true,
        bounceAtZoomLimits : false,
        zoomControl: true,
        wheelPxPerZoomLevel: 1000,
    });
        
    map.attributionControl.setPrefix("");

    L.control.layers(layer_group).addTo(map);

    map.on('mousedown', onMapMouseDown);
    map.on('mouseup',   onMapMouseUp);
    map.on('mouseout',  onMapMouseUp);
    map.on('mousemove', onMapMouseOver);

};

$(document).bind("contextmenu", function(event) {
    event.preventDefault();    
});

function LatLngPoint() {
	this.p0;
	this.p1;
	this.color;
}

var marker = null;
var tiles = null;

var drawing = false;

function loadUi() {
    
	marker = new Array(_view.tile.length);
	for(var i = 0; i < marker.length; i++) {
		marker[i] = null;
	}
	tiles = new Array(_view.tile.length);
	for(var i = 0; i < tiles.length; i++) {
		tiles[i] = new LatLngPoint();
		tiles[i].color = _view.tile[i].color;
	}
	

	var sections = { "#right-section": 0, "#section": 0, "#top-section": 0 };
	_view.views.forEach(function (entry) {
        if (entry.type != "mysql")
    	    $(entry.div).append($('<div id="' + entry.field.name + '">' + entry.title + '</div>'));
    	
    	d3.select("#" + entry.field.name)
    		.style("text-align", "center")
    		.style("font-weight", "bold")
    		.style("color", "white")
    		.style("line-height", "200%")
    		.style("width", entry.size + "%")
    		.style("height", "100%")
    		.style("float", "right");
    	
    	sections[entry.div] += entry.size;

        if (entry.on_menu) {        	
        	var name = "tabs-" + entry.field.name;
        	
            $("ul.tabs-ul").append($('<li><a href="#' + name + '">' + entry.title + "</a></li>"));
            
            $("#tabs").append($('<div id="' + name + '"></div>'));
            
            if (entry.type == "histogram") {

            	$("#" + name).append($('<div id="' + name + "-checkboxes" + '"></div>'));
            	
            	for (var i = 0, len = entry.field.values.length; i < len; i++) {
            		$("#" + name + "-checkboxes").append($('<input type="checkbox" id="check' + name + i + '" value="' + i + '"><label for="check' + name + i + '">' + entry.field.values[i] + '</label>'));            		  
        		}
            	
            	$("#" + name).buttonset();
            	
            	$("#" + name).on("change", function(event, ui) {
                    a_getQuery();
                });
            	
            }
        }        
	});

	if (sections["#right-section"] != 0) {
        d3.select("#container")
            .style("width", 75 + "vw");
        d3.select("#right-section")
           .style("width", 25 + "vw")
        d3.select("#section")
           .style("width", 75 + "vw")
        d3.select("#top-section")
           .style("width", 75 + "vw")
    }

    if (sections["#top-section"] == 0)
        d3.select("#top-section").style("width", 0 + "vw")

    $("ul.tabs-ul").append($("<li><a href=" + '"#tabs-settings"' + ">" + "Settings" + "</a></li>"));
    $("#tabs").append($('<div id="tabs-settings"></div>'));    

    // definition of `brighter`, `fainter`, and `setMode` in leaflet-hashedcubes.js
    $("#tabs-settings").append($('<div id="tabs-brightness-settings"></div>'));
    $("#tabs-brightness-settings").append($('<button>Brighter</button>').click(brighter).button());
    $("#tabs-brightness-settings").append($('<button>Fainter</button>').click(fainter).button());
    $("#tabs-brightness-settings").buttonset();

    $("#tabs-settings").append($('<div id="tabs-draw-settings"></div>'));
    $("#tabs-draw-settings").append($('<button>Rect</button>').click(function() { setMode('rect'); }).button());
    $("#tabs-draw-settings").append($('<button>Circle</button>').click(function() { setMode('circle'); }).button());
    $("#tabs-draw-settings").buttonset();

    $("#tabs-settings").append($('<div id="tabs-color-settings"></div>'));
    $("#tabs-color-settings").append($('<button>Red-Yellow-White</button>').click(function() { setScale('ryw'); }).button());
    $("#tabs-color-settings").append($('<button>Light-Blue-Dark</button>').click(function() { setScale('bbb'); }).button());
    $("#tabs-color-settings").buttonset();

    $("#tabs-settings").append($('<div id="tabs-transform-settings"></div>'));
    $("#tabs-transform-settings").append($('<button>Not Density Aware</button>').click(function() { setTransform('no_scaling'); }).button());
    $("#tabs-transform-settings").append($('<button>Density aware</button>').click(function() { setTransform('density_scaling'); }).button());
    $("#tabs-transform-settings").buttonset();

    $("#tabs").tabs({
        collapsible : true,
        active : false,
        event : "mousedown"
    });

    $("#tabs").mouseleave(function() {
        $(this).tabs({
            active : false
        });
    });
        
    $("#progressbar").progressbar({
        value : 0.1
    });

    $("#progressbar .ui-progressbar-value").addClass("ui-corner-right");

    $(function () {
        $("#slider").slider({
            min: 0,
            max: 20,
            step: 1,
            change: function (event, ui) {
                $("#amount").val(ui.value);

                curr_heatmap_resolution = ui.value;
                a_getQuery();
            },
            slide: function (event, ui) {
                $("#amount").val(ui.value);
            },
        });
    });

    $("#slider").slider("value", 8);
    
    $("#checkTime").button({
        text : false,
        icons : {
            primary : "ui-icon-check"
        }
    }).click(function() {
        var options;
        if ($(this).val() === "1") {
            $(this).val("0");
            options = {
                icons : {
                    primary : "ui-icon-check"
                }
            };
        } else {
            $(this).val("1");
            options = {
                icons : {
                    primary : "ui-icon-cancel"
                }
            };
        }
        $(this).button("option", options);
    });

    $("#play").button({
        text : true,
        icons : {
            primary : "ui-icon-play"
        }
    }).click(function() {
        var options;
        if ($(this).val() === "1") {
            $(this).val("0");
            options = {
                icons : {
                    primary : "ui-icon-pause"
                }
            };
        } else {
            $(this).val("1");
            options = {
                icons : {
                    primary : "ui-icon-play"
                }
            };
        }
        $(this).button("option", options);
        a_getQuery();
    });

    $("#play-slider").button({
        text : true,
        icons : {
            primary : "ui-icon-pause"
        }
    }).click(function() {
        var options;
        if ($(this).val() === "1") {
            $(this).val("0");
            sliderRT = false;
            options = {
                icons : {
                    primary : "ui-icon-pause"
                }
            };
        } else {
            $(this).val("1");
            sliderRT = true;
            options = {
                icons : {
                    primary : "ui-icon-play"
                }
            };
        }
        $(this).button("option", options);
    });    
}

function onMapMouseOver(e) {
    if (drawing == false) return;

    tiles[currTile].p1 = e.latlng; 
    
    if (marker[currTile] != null) {
        map.removeLayer(marker[currTile]);        
    }
    
    marker[currTile] =  L.rectangle([tiles[currTile].p0,tiles[currTile].p1], {color: tiles[currTile].color, weight: 1});
    map.addLayer(marker[currTile]);
}

function onMapMouseDown(e) {
    if (e.originalEvent.button != 2) {
        return;
    }
    
    if (marker[currTile] != null) {        
        map.removeLayer(marker[currTile]);
        marker[currTile] = null;
    }
        
    drawing = true;
    tiles[currTile].p0 = e.latlng;
}

function onMapMouseUp(e) {
    if (e.originalEvent.button != 2 || drawing == false) {
        return;
    }
    
    tiles[currTile].p1 = e.latlng; 
    
    if (tiles[currTile].p0.lat == tiles[currTile].p1.lat && tiles[currTile].p0.lng == tiles[currTile].p1.lng) {
        drawing = false;
        
        if (marker[currTile] != null) {        
            map.removeLayer(marker[currTile]);
            marker[currTile] = null;
        }
        
        a_getQuery();        
        return;
    }
    
    drawing = false;
    
    a_getQuery();
    
    if (marker[currTile] != null) {
        map.removeLayer(marker[currTile]);
    }
    
    marker[currTile] =  L.rectangle([tiles[currTile].p0,tiles[currTile].p1], {color: tiles[currTile].color, weight: 1});
    map.addLayer(marker[currTile]);
}
