function loadLineChart(data, entry) {
    if (data.length <= 1) return;

    var margin = {
        top: 10,
        right: 40,
        bottom: 20,
        left: 0
    };

    var div = d3.select("#" + entry.field.name);
    
    var width = div.node().getBoundingClientRect().width - margin.left - margin.right;
    var height = div.node().getBoundingClientRect().height - margin.top - margin.bottom - 32;

    var x = d3.time.scale()
        .range([0, width]);

    var y = d3.scale.linear()
        .range([height, 0]);

    var xAxis = d3.svg.axis()
        .scale(x)
        .orient("bottom")
        .tickSize(-height)
        .tickPadding(6);

   

    var area = d3.svg.area()
        .interpolate("cardinal")
        //.interpolate("monotone")
        .x(function (d) { return x(d[0]); })
        .y0(y(0))
        .y1(function (d) { return y(d[1]); });

    var yAxis = d3.svg.axis()
       .scale(y)
       .orient("right")
       .tickSize(-width)
       .tickPadding(6);

    var line = d3.svg.line()
        .interpolate("cardinal")
        //.interpolate("monotone")
        .x(function (d) { return x(d[0]); })
        .y(function (d) { return y(d[1]); });

    // Select the svg element, if it exists.
    var svg = div.selectAll('svg').data([data]);

    svg.enter()
        .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g").attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    var zoom = d3.behavior.zoom()
        .on("zoom", draw)
        .on("zoomend", update_linechart);

    var gradient = svg.append("defs").append("linearGradient")
        .attr("id", "gradient")
        .attr("x2", "0%")
        .attr("y2", "100%");

    gradient.append("stop")
        .attr("offset", "50%")
        .attr("stop-color", "#007399")
        .attr("stop-opacity", 1);

    gradient.append("stop")
        .attr("offset", "100%")
        .attr("stop-color", "red")
        .attr("stop-opacity", 1);

    svg.append("clipPath")
        .attr("id", "clip")
        .append("rect")
        .attr("x", x(0))
        .attr("y", y(1))
        .attr("width", x(1) - x(0))
        .attr("height", y(0) - y(1));

    svg.append("g")
        .attr("class", "y axis")
        .attr("transform", "translate(" + width + ",0)")
        .attr("font-family", "sans-serif").attr("font-size", "10px").attr("fill", "white").attr("text-anchor", "middle");

    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .attr("font-family", "sans-serif").attr("font-size", "10px").attr("fill", "white").attr("text-anchor", "middle");;

    svg.append("path")
        .attr("class", "area")
        .attr("clip-path", "url(#clip)")
        .style("fill", "url(#gradient)");

    

    svg.append("path")
        .attr("class", "line")
        .attr("clip-path", "url(#clip)");

    svg.append("rect")
        .attr("class", "pane")
        .attr("width", width)
        .attr("height", height)
        .call(zoom);

    data.forEach(function (d) {
        d[0] = new Date(d[0] * 1000);
    });

    x.domain([curr_lower_bound, curr_upper_bound]);
    y.domain([0, d3.max(data, function (d) { return d[1]; })]);
    zoom.x(x);

    svg.select("path.area").data([data]);
    svg.select("path.line").data([data]);
    draw();

    function draw() {
        svg.select("g.x.axis").call(xAxis);
        svg.select("g.y.axis").call(yAxis);
        
        svg.select("path.area").attr("d", area);
        svg.select("path.line").attr("d", line);
    }
    function update_linechart() {
        curr_lower_bound = xAxis.scale().domain()[0].getTime();
        curr_upper_bound = xAxis.scale().domain()[1].getTime();

        a_getQuery();
    }

    created = true;
}