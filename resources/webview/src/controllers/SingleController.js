(function () {
	angular
		.module('app')
		.controller('SingleController', ['$scope', '$window', 'Service', SingleController]);

	function SingleController($scope, $window, Service) {

		// There is only one model because we are using single template
		var model = Service.objectToArray($window.models)[0];

		// Setup view model
		$scope.vm = {
			model: model,

			// Last two path elements from modulePath
			file: model.modulePath.split('/').slice(-1)[0],

			// Chart data
			chart: {
				labels: ["Blank", "Code", "Comment"],
				data: [
					model.metrics.LOC.lines_blank,
					model.metrics.LOC.lines_code,
					model.metrics.LOC.lines_comment
				],
				colours: Chart.defaults.global.colours,

				// Chart.js options
				options: {
					animateRotate: false,
					animateScale: false
				}
			}
		};

		// console.log($scope.vm.model);
	}
})();