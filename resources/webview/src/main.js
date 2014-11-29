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
