function qDataReady() {

    $(document).ready(function(){
        // Get context with jQuery - using jQuery's .get() method.

        /*
        var ctx = $("#myChart").get(0).getContext("2d");

        var data = [
        {
            value: 300,
            color:"#F7464A",
            highlight: "#FF5A5E",
            label: "Red"
        },
        {
            value: 50,
            color: "#46BFBD",
            highlight: "#5AD3D1",
            label: "Green"
        },
        {
            value: 100,
            color: "#FDB45C",
            highlight: "#FFC870",
            label: "Yellow"
        }
        ]

        // This will get the first returned node in the jQuery collection.
        var myNewChart = new Chart(ctx).Pie(data);

        */

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

        $("#test").append("<pre>" + qData.models.length + "</pre>");
        $("#test").append("<pre>" + qData.models[0].id + "</pre>");
        $("#test").append("<pre>" + qData.models[0].label + "</pre>");
        $("#test").append("<pre>" + qData.models[0].params.metrics.LOC.lines + "</pre>");
    });
}

function getMethods(obj) {
    var result = [];
    for (var id in obj) {
        try {
            result.push(id + ": " + obj[id].toString());
        } catch (err) {
            result.push(id + ": inaccessible");
        }
    }
    return result;
}

function tempModelItemReady() {
    qData.models.push(tempModelItem);
}
