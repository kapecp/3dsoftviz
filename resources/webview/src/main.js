function qDataReady() {

    $(document).ready(function(){
        // Get context with jQuery - using jQuery's .get() method.
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
        $("#test").append("1: " + qData + "<br>");
        $("#test").append("2: " + qData.models.length + "<br>");
        //console.log(qData);
    });
}

function tempModelItemReady() {
    qData.models.push(tempModelItemReady);
}
