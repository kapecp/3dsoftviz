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
			filePath: model.modulePath.split('/').slice(-2),

			// Chart data
			chart: {
				labels: ["Total", "Blank", "Code", "Comment", "Non empty"],
				data: [
					model.metrics.LOC.lines,
					model.metrics.LOC.lines_blank,
					model.metrics.LOC.lines_code,
					model.metrics.LOC.lines_comment,
					model.metrics.LOC.lines_nonempty
				],
				colours: Chart.defaults.global.colours,

				// Chart.js options
				options: {
					animateScale: true
				}
			}
		};

		// console.log($scope.vm.model);
	}
})();