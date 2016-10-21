function setSctterChart(data, entry) {

    var margin = {
        top : 0,
        right : 15,
        bottom : 23,
        left : 25
    };
    
    var div = d3.select("#" + entry.field.name);
    
    var width = div.node().getBoundingClientRect().width;
    var height = div.node().getBoundingClientRect().height - 32;
    
    var scatter_width = width - margin.left -margin.right;
    var scatter_height = height - margin.top - margin.bottom;
    
    var max = d3.max(data, function(d) { return d[2]; });
    
    var x = d3.scale.linear()
              .domain([0, 23])
              .range([ 0, scatter_width - (scatter_width / 24)]);
    
    var y = d3.scale.linear()
              .domain([0, 6])
              .range([ scatter_height - (scatter_height / 7), 0 ]);
              
    div.selectAll("*").remove();
    
    var tip = d3.tip()
        .attr('class', 'd3-tip')
        .offset([-10, 0])
        .html(function(d) {
            return "<strong>Day:</strong> <span style='color:red'>" + d[1] + "</span>" +
      		       "</br><strong>Hour:</strong> <span style='color:red'>" + d[0] + "</span>" +
			       "</br><strong>Value:</strong> <span style='color:red'>" + d[2] + "</span>";
        });
    
    var main = div
    .append("svg")
    .attr("width", width)
    .attr('height', height)
    .attr("class", "chart");
    
    var chart = main.append("g")
    .attr("transform", "translate(" + (margin.left)+ "," + (margin.top) + ")")
    .attr("width", scatter_width)
    .attr("height", scatter_height)
    .attr("class", "main")
    .call(tip);  
        
    // draw the x axis
    var xAxis = d3.svg.axis()
    .scale(x)
    .ticks(24)
    .orient('bottom');

    main.append('svg:g')
    .attr('transform', 'translate(' + (margin.left + (scatter_width / 48)) + ',' + (height - margin.bottom) + ')')
    .attr('class', 'main axis a')
    .call(xAxis).attr("font-family", "sans-serif").attr("font-size", "11px").attr("fill", "white").attr("text-anchor", "middle");;

    // draw the y axis
    var yAxis = d3.svg.axis()
    .scale(y)
    .ticks(7)
    .orient('left');

    main.append('svg:g')
    .attr('transform', 'translate(' + (margin.left) + ',' + (margin.top + (scatter_height / 14)) + ')')
    .attr('class', 'main axis b')
    .call(yAxis).attr("font-family", "sans-serif").attr("font-size", "11px").attr("fill", "white").attr("text-anchor", "middle");;
        
    var color = d3.scale.linear()
    .domain([0.3, 1])
    .range(["black", "steelblue"]);
    
    chart.selectAll("scatter-dots")
        .data(data)
        .enter().append("rect")
        .attr("x", function(d) { return d[0] * (scatter_width / 24); })
        .attr("y", function(d) { return (6 - d[1]) * (scatter_height / 7); })
        .attr("width", (scatter_width / 24) + .5)
        .attr("height", (scatter_height / 7) + .5)
        .attr("fill", function(d) { return color((d[2] / max)); })
        .on('mouseover', tip.show)
        .on('mouseout', tip.hide);   
}  