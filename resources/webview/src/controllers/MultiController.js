(function () {
	angular
		.module('app')
		.controller('MultiController', ['$scope', '$window', 'Service', MultiController]);

	function MultiController($scope, $window, Service) {

		var models = Service.objectToArray($window.models);

		var data = d3.range(0, models.length, 1)
			.map(function (i) {
				return {
					"LOC": models[i].metrics.LOC.lines,
					"Cyclomatic": models[i].metrics.cyclomatic.upperBound_all,
					"Infoflow": models[i].metrics.infoflow.interface_complexity
				};
			});

		// Setup view model
		$scope.vm = {
			chart: {
				data: data
			}
		};

		// console.log($scope);
	}
})();