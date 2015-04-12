
var qData = {};

// This is draft. Will be refactored

// Only for testing in desktop browser (prevent double initialization)
var isChrome = navigator.userAgent.toLowerCase().indexOf('chrome') > -1;
if(isChrome){
    $(document).ready(function(){
        $("#single").show();
        initLOC(36, 4, 4);
        initComplexity(7, 5, 6, 8);

        /*
        $("#multiple").show();

        var data = {
            labels: ["factorial", "sum", "service", "interface"],
            datasets: [
            {
                label: "My First dataset",
                fillColor: "rgba(220,220,220,0.8)",
                strokeColor: "rgba(220,220,220,1)",
                data: [120, 50, 120, 30]
            }
            ]
        };

        initMultipleLOC(data);
        */
    });
}

/*
 * Initialize lines of code metrics polar area chart (pie) for single model
 */
function initLOC(codeLines, commentLines, blankLines) {
    var $fig = $("#fig1");
    var ctx = $fig.find("canvas").get(0).getContext("2d");

    var data = [
        {
            value: codeLines,
            color: "#FDB45C",
            label: "Code lines"
        },
        {
            value: commentLines,
            color: "#336699",
            label: "Comment lines"
        },
        {
            value: blankLines,
            color:"#F7464A",
            label: "Blank lines"
        }
    ]

    // Chart options (modified template)
    var options = {
        legendTemplate : "<ul class=\"<%=name.toLowerCase()%>-legend\"><% for (var i=0; i<segments.length; i++){%><li><span style=\"background-color:<%=segments[i].fillColor%>\">&nbsp;&nbsp;&nbsp;</span><%if(segments[i].label){%><%=segments[i].label%><%}%> (<%=segments[i].value%>)</li><%}%></ul>"
    };

    var myChart = new Chart(ctx).PolarArea(data, options);

    $fig.find(".legend").html(myChart.generateLegend());
}

/*
* Initialize lines of code metrics bar chart for multiple model (test)
*/
function initMultipleLOC(data) {
    var $fig = $("#fig10");
    var ctx = $fig.find("canvas").get(0).getContext("2d");

    var myChart = new Chart(ctx).Bar(data);
}

/*
* Initialize lines of code metrics line chart for multiple model (test)
*/
function initMultipleLOC2(data) {
    var $fig = $("#fig10");
    var ctx = $fig.find("canvas").get(0).getContext("2d");

    var myChart = new Chart(ctx).Line(data);
}

/*
* Initialize complexity metrics bar chart for single model
*/
function initComplexity(conditions, decisions, lowerBound, upperBound) {
    var $fig = $("#fig2");
    var ctx = $fig.find("canvas").get(0).getContext("2d");

    var data = {
        labels: ["Conditions", "Decisions", "Lower Bound", "Upper Bound"],
        datasets: [
        {
            label: "My First dataset",
            fillColor: "rgba(220,220,220,0.5)",
            strokeColor: "rgba(220,220,220,0.8)",
            highlightFill: "rgba(220,220,220,0.75)",
            highlightStroke: "rgba(220,220,220,1)",
            data: [conditions, decisions, lowerBound, upperBound]
        }
        ]
    };

    var myChart = new Chart(ctx).Bar(data);
}

/*
 * Called when qData variable is populated with model(s) from Qt
 */
function qDataReady() {

    $(document).ready(function(){

        if(qData.models.length == 1){
            $("#single").show();
            $("#single-label").html(qData.models[0].params.name);
            $("#single-type").html(qData.models[0].params.type);
            $("#single-path").html(qData.models[0].params.modulePath);

            initLOC(
                qData.models[0].params.metrics.LOC.lines_code,
                qData.models[0].params.metrics.LOC.lines_comment,
                qData.models[0].params.metrics.LOC.lines_blank
            );

            initComplexity(
                qData.models[0].params.metrics.cyclomatic.conditions,
                qData.models[0].params.metrics.cyclomatic.decisions,
                qData.models[0].params.metrics.cyclomatic.lowerBound,
                qData.models[0].params.metrics.cyclomatic.upperBound
            );
        }else{
            $("#multiple").show();

            // Get data from multiple nodes
            var labelsArray = [];
            var dataArray = [];
            for(var i=0; i<qData.models.length; i++){
                var model = qData.models[i];
                labelsArray.push(model.params.name + " (" + model.params.type + ")");
                dataArray.push(model.params.metrics.LOC.lines);
            }

            var data = {
                labels: labelsArray,
                datasets: [
                {
                    label: "My First dataset",
                    fillColor: "rgba(220,220,220,0.8)",
                    strokeColor: "rgba(220,220,220,1)",
                    data: dataArray
                }
                ]
            };

            initMultipleLOC(data);

            // test line
            //initMultipleLOC2(data);
        }

        console.log("logtest");
        console.error("normal error");
        consolee.log("error test");

    });
}
