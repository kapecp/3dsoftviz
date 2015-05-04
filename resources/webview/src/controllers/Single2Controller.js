(function () {
	angular
		.module('app')
		.controller('Single2Controller', ['$scope', '$window', 'Service', Single2Controller]);

	function Single2Controller($scope, $window, Service) {
		// There is only one model because we are using single template
		var model = Service.objectToArray($window.models)[0];

		// Setup view model
		$scope.vm = {
			model: model
		};

		// console.log($scope.vm.model);
	}
})();