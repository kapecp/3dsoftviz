(function () {
    angular
        .module('app')
        .controller('MultiController', ['$scope', '$window', MultiController]);

    function MultiController($scope, $window) {
        $scope.vm = {
            test: "MULTI controller"
        };
    }
})();