function setHistogramData(data, entry) {
	
	var margin = {
	        top : 10,
	        right : 20,
	        bottom : 33,
	        left : 20
	    };
	
	var div = d3.select("#" + entry.field.name);

    var maxValue = -1;
    for (var index in data) {

        if (data[index][1] > maxValue)
            maxValue = data[index][1];
    }

    var w = div.node().getBoundingClientRect().width - margin.left - margin.right;
    var h = div.node().getBoundingClientRect().height - margin.top - margin.bottom;
    
    var barPadding = 1;

    div.selectAll("*").remove();

    var tip = d3.tip()
    .attr('class', 'd3-tip')
    .offset([-10, 0])
    .html(function(d) {    	
      return "<strong>Value:</strong> <span style='color:red;'>" + d[1] + "</span>";
    });
    
    
    div = div.append("svg").attr("width", w + margin.left + margin.right).attr("height", h + margin.top + margin.bottom).append("g").attr("transform", "translate(" + margin.left + "," + margin.top + ")").call(tip);

    
    var colors = ['#007399', 'Gray']

    div.selectAll("rect").data(data).enter().append("rect")
    .attr("x", function(d, i) {
        return i * (w / data.length);
    }).attr("y", function(d) {
        return h - (d[1] / maxValue * h);
    }).attr("width", w / data.length - barPadding).attr("height", function(d) {
        return d[1] / maxValue * h;
    }).attr("fill", function(d) {
        return colors[0];
    })
    .on('mouseover', tip.show)
    .on('mouseout', tip.hide)
    .on('mouseup', function () { console.log("update"); })
    .each(function () {
        var selection = d3.select(this);
        var state = true;
        selection.on('mousedown', function () {
            if (state) {
                selection.style('fill', colors[1]);
            } else {
                selection.style('fill', colors[0]);
            }
            state = !state;
            div.selectAll("rect").each(function (d) { console.log(d3.select(this).style("fill")) });

        });
     });
    
    div.selectAll("text").data(data).enter().append("text")
    .text(function (d, i) {
    	return entry.field.values[d[0]];
    }).attr("cx", "0").attr("cy", "0")
    .attr("font-family", "sans-serif")
    .attr("font-size", "11px")
    .attr("fill", "white")
    .attr("text-anchor", "start")
    .attr("transform", function (d, i) {
        var x = Math.max(4, h - (d[1] / maxValue * h) - this.getComputedTextLength() - 4);
        var y = - (i * (w / data.length) + (w / data.length - barPadding) / 2) + 4;
        return 'rotate(90)translate(' + x + ',' + y + ')';
    });
}