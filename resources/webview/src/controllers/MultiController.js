(function () {
    angular
        .module('app')
        .controller('MultiController', ['$scope', MultiController]);

    function MultiController($scope) {
        $scope.model = {
            test: "MULTI controller"
        };
    }
})()