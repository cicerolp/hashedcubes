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

    div.selectAll("rect").data(data).enter().append("rect")
    .attr("x", function(d, i) {
        return i * (w / data.length);
    }).attr("y", function(d) {
        return h - (d[1] / maxValue * h);
    }).attr("width", w / data.length - barPadding).attr("height", function(d) {
        return d[1] / maxValue * h;
    }).attr("fill", function(d) {
        return "rgb(" + Math.floor((1.0 - d[1] / maxValue) * 255) + ","  + Math.floor((d[1] / maxValue) * 154) + ", " + Math.floor((d[1] / maxValue) * 216) + ")";
    })
    .on('mouseover', tip.show)
    .on('mouseout', tip.hide);
    
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