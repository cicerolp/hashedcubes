var DEBUG = false;

var lBound = 0;
var uBound = 23;
var sliderRT = false;

var root = "./rest/";
var _queryURL = root + "query/" + _schema;

var map;

var ready = false;

function onReady(callback) {
    var intervalID = window.setInterval(checkReady, 500);

    function checkReady() {
        if (ready == false) {
            $.ajax({
                type : 'GET',
                url : root + "schema/" + _schema,
                dataType : "json",
                success : function(target) {
                    if (!jQuery.isEmptyObject(target)) {
                        ready = true;

                        total_count = target.totalcount;

                        lower_bound = new Date(target.mindate * 1000);
                        upper_bound = new Date(target.maxdate * 1000);

                        curr_lower_bound = lower_bound.getTime();
                        curr_upper_bound = upper_bound.getTime();

                        window.clearInterval(intervalID);
                        callback.call(this);                        
                    }
                },
                error : function(jqXHR, textStatus, errorThrown) {
                    console.log(errorThrown);
                }
            });
        }
    }

}

function show(id, value) {
    document.getElementById(id).style.display = value ? 'block' : 'none';
}

onReady(function () {
    callbacks = $.Callbacks();

    show('container', true);
    show('loading', false);
    
    loadUi();
    loadMap();

    a_getQuery();    
});

var waitForFinalEvent = (function () {
  var timers = {};
  return function (callback, ms, uniqueId) {
    if (!uniqueId) {
      uniqueId = "Don't call this twice without a uniqueId";
    }
    if (timers[uniqueId]) {
      clearTimeout (timers[uniqueId]);
    }
    timers[uniqueId] = setTimeout(callback, ms);
  };
})();

var window_resize = false;

$(window).resize(function () {
    waitForFinalEvent(function() {
      window_resize = true;
    a_getQuery();
    }, 500, "WinResizeEvent");
});

function setProgressBar(count) {
    if (curr_count != count) { curr_count = count; } else { return; }

    $("#progressbar .ui-progressbar-value").animate({
        width: (((curr_count) / total_count) * 100) + "%"
    }, {
        queue : false
    }, {
        duration : 1000
    });

    $("#label").text("Total Count: " + curr_count + " of " + total_count);
}

function updateDataRestrictions() {

    update = false;
    update_tile = false;

    if (curr_heatmap_resolution != heatmap_resolution) {
        update_tile = true;
    }
    heatmap_resolution = curr_heatmap_resolution;
    
    curr_region = "";
    for (var i = 0; i < marker.length; i++) {
    	if (marker[i] == null) continue;
    	
    	var b = L.latLngBounds(tiles[i].p0, tiles[i].p1);
    	
    	var lat0 = b._northEast.lat;
    	var lon0 = b._southWest.lng;
    	var lat1 = b._southWest.lat;
    	var lon1 = b._northEast.lng;

    	var z = map.getZoom() + 8;
        
    	var x0 = roundtile(lon2tilex(lon0, z), z);
    	var x1 = roundtile(lon2tilex(lon1, z), z);

    	if (x0 > x1) {
    	    x0 = 0;
    	    x1 = Math.pow(2, z);
    	}

        // /x0/y0/x1/y1/
    	curr_region += "/region/" + i + "/" + z
            + "/" + x0
            + "/" + roundtile(lat2tiley(lat0, z), z)
            + "/" + x1
            + "/" + roundtile(lat2tiley(lat1, z), z);
    }
    if (curr_region == "") curr_region = "/region/0/0/0/0/0/0";

    if (curr_region != region) {
        update = true;
    }
    region = curr_region;

    var query_where = false;
    curr_where = "/where/";
    
    _view.views.forEach(function (entry) {
    	if (entry.on_menu) {
    		var restriction = entry.field.name + "=";
    		
    		$("#tabs-" + entry.field.name + "-checkboxes" + " :checked").each(function() {
    			restriction += parseInt($(this).val()) + "|";
    	        query_where = true;
    	    });
    		restriction = restriction.substring(0, restriction.length - 1) + "&";
    		
    		curr_where += restriction; 
        }
    });

    if (query_where) {
    	curr_where = curr_where.substring(0, curr_where.length - 1);
    } else {
        curr_where = "";
    }
    
    if (curr_where != where) {
        update = true;
        update_tile = true;
    }
    where = curr_where;

    curr_tseries = "";

    _view.views.forEach(function (entry) {
        if (entry.type == "time-series") {            

            var tseries_from = "/" + Math.floor(Math.max(lower_bound.getTime(), curr_lower_bound) / 1000);
            var tseries_to = "/" + Math.ceil(Math.min(upper_bound.getTime(), curr_upper_bound) / 1000);

            curr_tseries += "/tseries/" + entry.field.name + tseries_from + tseries_to;
            console.log(curr_tseries);
        }
    });
    
    if (curr_tseries != tseries) {
        update = true;
        update_tile = true;
    }
    tseries = curr_tseries;
    
    if (window_resize) {
        update = true;
    }
    window_resize = false;
}

function a_getQuery() {
    if ($("#play").val() == 0) {
        return;
    }

    updateDataRestrictions();

    if (update_tile) callbacks.fire(heatmap_resolution + where + tseries);

    if (update) {
        var query = "/region" + region + tseries + where;
        $.ajax({
            type: 'GET',
            url: _queryURL + query,
            dataType: "json",
            success: function (data) {
                setProgressBar(data);
            },
            error: function (jqXHR, textStatus, errorThrown) {
                console.log(errorThrown);
            }
        });

    	_view.views.forEach(function(entry) {
    	    switch (entry.type) {
    	        case "mysql":
    	        {
    	            var query = "/mysql" + tseries + region + where;
    	            $.ajax({
    	                type: 'GET',
    	                url: _queryURL + query,
    	                dataType: "json",
    	                success: function (data) {
    	                    loadMySQLPanel(data, entry);
    	                },
    	                error: function (jqXHR, textStatus, errorThrown) {
    	                    console.log(errorThrown);
    	                }
    	            });
    	        } break;

        		case "histogram": 
        		{
        			var query = "/group/field/" + entry.field.name + region + where + tseries;
        	        $.ajax({
        	            type : 'GET',
        	            url : _queryURL + query,
        	            dataType : "json",
        	            success : function(data) {
        	                setHistogramData(data, entry);
        	            },
        	            error : function(jqXHR, textStatus, errorThrown) {
        	                console.log(errorThrown);
        	            }
        	        });	
        		} break;
        		
        		case "time-series": 
        		{
        			var query = "/tseries" + tseries + region + where;
        	        $.ajax({
        	            type : 'GET',
        	            url : _queryURL + query,
        	            dataType : "json",
        	            success : function(data) {
        	                loadLineChart(data, entry);
        	            },
        	            error : function(jqXHR, textStatus, errorThrown) {
        	                console.log(errorThrown);
        	            }
        	        });
        		} break;
        		
        		case "binned-scatterplot": 
        		{
    				var query = "/scatter/field/" + entry.field_x.name + "/field/" + entry.field_y.name + region;
    				
    		        $.ajax({
    		            type : 'GET',
    		            url : _queryURL + query,
    		            dataType : "json",
    		            success : function(data) {
    		                setSctterChart(data, entry);
    		            },
    		            error : function(jqXHR, textStatus, errorThrown) {
    		                console.log(errorThrown);
    		            }
    		        });
        		} break;
    		}    		
        });
    }
}
