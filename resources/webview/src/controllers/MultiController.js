(function () {
	angular
		.module('app')
		.controller('MultiController', ['$scope', '$window', 'Service', MultiController]);

	function MultiController($scope, $window, Service) {

		var models = Service.objectToArray($window.models);

		// Translate array of models to array of module names
		var modulePaths = $.map(models, function (model) {
			return model.type + " " + model.name;
		});

		// Setup view model
		$scope.vm = {
			models: models,
			modulePaths: modulePaths,

			// Chart data
			chart: {
				labels: modulePaths,
				series: ["LOC", "Cyclomatic", "Infoflow"],
				data: $.map(models, function (model) {
					return [[
						model.metrics.LOC.lines,
						model.metrics.cyclomatic.upperBound_all,
						model.metrics.infoflow.interface_complexity
					]];
				}),

				// Chart.js options
				options: {
					bezierCurve: false,
					datasetFill : false
				}
			}
		};

		console.log($scope);
	}
})();