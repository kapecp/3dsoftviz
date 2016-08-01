(function () {
	angular
		.module('app')
		.controller('GitController', ['$scope', '$window', 'Service', GitController]);

	function GitController($scope, $window, Service) {

		// There is only one model because we are using single template
		var model = Service.objectToArray($window.models)[0];

		// Setup view model
		$scope.vm = {
			model: model,

			// Last two path elements from modulePath
			//file: model.modulePath.split('/').slice(-1)[0],

			// Chart data
			chart: {
				labels: ["Blank", "Code", "Comment"],
				data: [
					model.metrics.changedBlank,
					model.metrics.changedLinesCode,
					model.metrics.changedLinesComment
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