function loadMySQLPanel(data, entry) {

    var div = d3.select("#right-section");    
    div.selectAll("*").remove();

    $("#right-section").append($('<table id="jqGrid"></table>'));
    $("#right-section").append($('<div id="jqGridPager"></div>'));

    var names = [];
    var model = [];
    entry.field.values.forEach(function (value) {
        names.push(value.label);
        model.push({ align: value.align, width: value.width, /*label: value.label,*/ name: value.name, formatter: value.formatter });        
    });

    $("#jqGrid").jqGrid({
        datatype: "clientSide",
        data: data,
        colNames: names,
        colModel: model,
        caption: entry.title,
        pager: "#jqGridPager",
        rowNum: 100,
        viewrecords: true,
        loadonce: true,
        width: div.node().getBoundingClientRect().width,
        height: div.node().getBoundingClientRect().height - 84,
    });

    

    $("#jqGrid").jqGrid('navGrid', '#jqGridPager', {
        add: false,
        del: false,
        edit: false,       
        refresh: false,
        search: true,        
        view: true,
        position: "left"
    });
}