
function makeChart(canvasId) {
    myChart = new Chart(document.getElementById(canvasId).getContext('2d'), {
        type: 'line',
        data: {
            "datasets": []
        },
        options: {
            scales: {
                yAxes: [{
                    id: 'y1',
                    type: 'linear',
                }],
                xAxes: [{
                    id: 'x1',
                    type: 'linear',
                    reversed: true,
                }]
            },
            animation: {
                duration: 0, // general animation time
            },
            hover: {
                animationDuration: 0, // duration of animations when hovering an item
            },
                responsiveAnimationDuration: 0, // animation duration after a resize
            },
    });

    return myChart;
}


function updatePlot(chartObject) {

    $.getJSON(chartObject.dataUrl, function(data) {
        console.log("updating plot " + chartObject.name);
        chartObject.chart.data = data;
        chartObject.chart.update();
    });
}


function mccChartsMain(charts, updatePeriodMs, plotDataRootUri) {

    charts = $.map(charts, function(chartObj) {

        chartObj.dataUrl = plotDataRootUri + "?chartName=" + chartObj.name;
        chartObj.chart = makeChart(chartObj.name + "-plot-canvas");

        return chartObj;
    });

    // Графики успешно созданы. Запускаем периодические обновления

    var intervals = $.map(charts, function(chartObj) {
        return setInterval(
            function() { updatePlot(chartObj); },
            updatePeriodMs
        );
    });

}

