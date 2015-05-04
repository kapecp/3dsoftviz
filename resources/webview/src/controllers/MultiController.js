(function () {
	angular
		.module('app')
		.controller('MultiController', ['$scope', '$window', 'Service', MultiController]);

	function MultiController($scope, $window, Service) {

		var models = Service.objectToArray($window.models);

		var data = d3.range(0, models.length, 1)
			.map(function (i) {
				//console.log(models[i])
				return {
					LOC: models[i].metrics.LOC.lines,
					Cyclo: models[i].metrics.cyclomatic.upperBound,
					Compl: models[i].metrics.infoflow.interface_complexity,
					ON: models[i].metrics.halstead.number_of_operands,
					OT: models[i].metrics.halstead.number_of_operators,
					"UON": models[i].metrics.halstead.unique_operands,
					"UOT": models[i].metrics.halstead.unique_operators
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