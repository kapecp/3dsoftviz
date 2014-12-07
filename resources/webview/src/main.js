// Only for testing in desktop browser (prevent double initialization)
var isChrome = navigator.userAgent.toLowerCase().indexOf('chrome') > -1;
if(isChrome){
    $(document).ready(function(){
        $("#single").show();
        initLOC(36, 4, 4);
        initComplexity(7, 5, 6, 8);
    });
}

/*
 * Initialize lines of code metrics polar area chart (pie) for single model
 */
function initLOC(codeLines, commentLines, blankLines) {
    // Get context with jQuery - using jQuery's .get() method.

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

    // This will get the first returned node in the jQuery collection.
    var myChart = new Chart(ctx).PolarArea(data, options);

    $fig.find(".legend").html(myChart.generateLegend());
}

/*
* Initialize complexity metrics bar chart for single model
*/
function initComplexity(conditions, decisions, lowerBound, upperBound) {
    // Get context with jQuery - using jQuery's .get() method.
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

    // This will get the first returned node in the jQuery collection.
    var myChart = new Chart(ctx).Bar(data);
}

/*
 * Called when qData variable is populated with model(s) from Qt
 */
function qDataReady() {

    $(document).ready(function(){

        if(qData.models.length == 1){
            $("#single").show();
            $("#single-label").html(qData.models[0].label);
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
        }
        // Test qData
        //alert(qData.toString());
        //alert($("#test"));
        //$("#test").html("Selected nodes count: " + qData.getModels() + " m: " + getMethods(qData));
        //alert(getMethods(qData));

        //$("#test").append(qData + "<br>");
        // $("#test").append(qData.getName() + "<br>");
        // $("#test").append(qData.getModel + "<br>");
        // $("#test").append(qData.getModel() + "<br>");
        // $("#test").append(qData.getModel().constructor + "<br>");
        // $("#test").append(qData.model + "<br>");
        // $("#test").append("1: " + qData + "<br>");
        // $("#test").append("2: " + qData.models.length + "<br>");
        // $("#test").append("3: " + qData.models[0].rootItem + "<br>");
        // $("#test").append("5: <pre>" + getMethods(qData.models[0].rootItem) + "</pre><br>");
        // $("#test").append("6: " + qData.models[0].test + "<br>");
        // $("#test").append("7: " + qData.models[0].test() + "<br>");
        // $("#test").append("8: " + qData.models[0].rootItem.hello + "<br>");
        //console.log(qData);

        // $("#test").append("<pre>" + qData.models.length + "</pre>");
        // $("#test").append("<pre>" + qData.models[0].id + "</pre>");
        // $("#test").append("<pre>" + qData.models[0].label + "</pre>");
        // $("#test").append("<pre>" + qData.models[0].params.metrics.LOC.lines + "</pre>");
    });
}
