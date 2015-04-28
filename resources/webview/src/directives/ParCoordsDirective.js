(function () {
	angular
		.module('app')
		.directive('parCoords', parCoords);

	function parCoords() {
		var directive = {
			scope: {
				data: '='
			},
			link: link,
			restrict: 'A'
		};
		return directive;

		function link(scope, element, attrs) {
			d3.parcoords({
				margin: {top: 50, right: 0, bottom: 12, left: 0}
			})(element.context)
				.data(scope.data)
				.render()
				.createAxes(["aa", "bb"])
				.brushMode("1D-axes");
		}
	}
})();