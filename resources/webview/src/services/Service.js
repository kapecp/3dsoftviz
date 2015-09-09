(function () {
    angular
        .module('app')
        .service('Service', Service);

    function Service() {
        return {
            objectToArray: objectToArray
        };

        function objectToArray(myObj) {
            return $.map(myObj, function(value, key) {
                return [value];
            });
        }
    }
})();